###############################################################################
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# 
###############################################################################
package PcieRanges;
use strict;
use warnings FATAL=>q(all);
use File::Basename;
use BcmUtils;
use BcmDt::Devices;
use Math::BigInt;
use Storable;


my $P = basename $0;

my @pcie_nodes;
my $num_pcie;
my $DEBUG = !!$ENV{BOLT_DBG_PCIE};


########################################################################
# PARAMS:
#   $x -- a hex string that can potentially represent greater > 32bit.
#         The number must be a multiple of megabytes (eg '0x300000' but
#         not '0x300001').
#
# RETURNS:
#   A scalar 32 bit int, which is the input in MB.
#
# ASSUMES:
#   the input is less than 2^52.
########################################################################
sub hexstr_to_mbytes($)
{
	my $x = shift;
	die if $x !~ /^0[xX]/;
	$x = Math::BigInt->new($x);
	die if ($x & 0xfffff);
	my $x_mb = $x / (1024 * 1024);
	die if ($x_mb >> 32 != 0);
	return hex $x_mb->as_hex();
}


########################################################################
# PARAMS:
#   $x -- a scalar int containing a number (representable by 30 bits).
#
# RETURNS:
#   The value of the smallest power of two that is greater than or
#   equal to $x.  It is assumed we won't go over 2^31.
########################################################################
sub round_up_to_pow2($)
{
	my $x = shift;
	my $pow2_val = 1;
	my $log2 = 0;

	while ($x > $pow2_val) {
		$pow2_val *= 2;
		$log2++;
	}
	return $pow2_val;
}


########################################################################
# PARAMS:
#   $x -- a scalar int containing a number which is a power of 2.
#
# RETURNS:
#   the log2 of $x.
#
########################################################################
sub log2_of_pow2($)
{
	my ($x) = @_;
	for (my $log2 = 31; $log2 >= 0; $log2--) {
		return $log2 if ($x & (1 << $log2));
	}
	return 0;
}


########################################################################
# PARAMS:
#   $title -- must contain either 'inbound' or 'outbound'.
#   $ra -- array ref of ranges.  If 'outbound' is in the title,
#          this is an arrayref of arrayrefs.
#
# RETURNS:
#   undef
########################################################################
sub dump_ranges($$)
{
	my ($title, $ra, $b, $cond) = @_;
	die if $title !~ /(in|out)bound/i;
	my @b = @$ra;
	my @a;

	print "[DEBUG: $title]\n";
	@b = map { @$_ } @b
		if ($title =~ /outbound/i);

	@a = map { Math::BigInt->new($_) } @b;

	for (my $i = 0; $i < @a; $i += 7) {
		my $size = ($a[$i+5] << 32) + $a[$i+6];
		my $pcie_beg = ($a[$i+1] << 32) + $a[$i+2];
		my $pcie_end = $pcie_beg + $size - 1;
		my $cpu_beg = ($a[$i+3] << 32) + $a[$i+4];
		my $cpu_end = $cpu_beg + $size - 1;
		my $info_str = sprintf " (%4d MB", ($size >> 20);
		$info_str .= ', 64'
		    if (($a[$i+0] & 0x03000000) == 0x03000000);
		$info_str .= ', pref'
		    if (($a[$i+0] & 0x40000000) == 0x40000000);
		$info_str .= ')';

		printf "  [%1x.%08x...%1x.%08x] " .
		    "=> [%1x.%08x...%1x.%08x]%s\n",
		    BcmUtils::bigint_to_two_int($cpu_beg),
		    BcmUtils::bigint_to_two_int($cpu_end),
		    BcmUtils::bigint_to_two_int($pcie_beg),
		    BcmUtils::bigint_to_two_int($pcie_end),
		    $info_str,
		    ;
	}
	print "\n" if $title =~ /default/i;
}


########################################################################
# DESCRIPTION:
#   Tries to use the identity map for the system => pcie_space mapping.
#   (inbound transactions).  This is preferred if it is possible.
#
# PARAMS:
#   $p -- board memory structure.
#
# RETURNS:
#   $p -- the same board memory structure that was given but with
#         some crucial fields filled in.  If the algorithm was
#         successful,
#             o $p->{success} will be set to a non-null string.
#             o $p->{pci_bar2_size_mb} will be set to the RC_BAR2
#               viewport size.
#             o $p->{pci_bar2_val_mb} will be set to the pci_addr
#               of the viewport, which will always be 0.
########################################################################
sub identity_map_alg($)
{
	my ($p) = shift;
	my $scb_size_sum_mb = 0;
	$p->{success} = 0;
	my $last_pci_addr_mb = 0;

	for (my $i = 0; $i < @{$p->{ra_regions}}; $i++) {
		my $e = $p->{ra_regions}->[$i];

		# Fail if we have any memc extension.
		return $p if $p->{ext};

		if ($e->{addr_mb} == $last_pci_addr_mb) {
			$e->{pci_addr_mb} = $last_pci_addr_mb;
			$last_pci_addr_mb += $e->{scb_size_mb};

		} elsif ($e->{addr_mb} > $last_pci_addr_mb) {
			# We can possibly maintain the identity map
			# by increasing the previous memc's log2 SCB
			# size to larger than it needs to be.  However,
			# we only allow this because it is a legacy
			# trick, done for the 7366c0 and 7439b0. For
			# any other chips, let us move to the future
			# and not resort to faux sizes for the SCB.
			return $p if $i == 0;
			my $chip = ::get_family_name();
			return $p if ($chip !~ /^(7366c0|7439b0)$/);
			my $e_prev = $p->{ra_regions}->[$i-1];
			while ($e->{addr_mb} > $last_pci_addr_mb) {
				$last_pci_addr_mb += $e_prev->{scb_size_mb};
				$e_prev->{scb_size_mb} *= 2;
			}
			return $p if ($e->{addr_mb} != $last_pci_addr_mb);
			$e->{pci_addr_mb} = $last_pci_addr_mb;
			$p->{identity_map_faux_scb_size} = ($i-1);
		} else {
			return $p;
		}
		$scb_size_sum_mb += $e->{scb_size_mb};
	}

	$p->{pci_bar2_size_mb} = round_up_to_pow2($scb_size_sum_mb);
	$p->{pci_bar2_val_mb} = 0;
	$p->{success} = 'identity_map';

	return $p;
}


########################################################################
# DESCRIPTION:
#   Maps the system memory to the pci space (inbound transactions).
#   First, we ascertain the size of the RC_BAR2 viewport.  This must
#   be a power of two, and is based on the sum of the constituent
#   scb sizes.  Once we have this size, S, we will map system memory
#   to PCIe addresses [S...2S-1].  After this function is called,
#   we will have to calculate the dma_ranges needed to make this all
#   possible.
#
# PARAMS:
#   $p -- board memory structure.
#
# RETURNS:
#   $p -- the same board memory structure that was given but with
#         some crucial fields filled in.  If the algorithm was
#         successful,
#             o $p->{success} will be set to a non-null string.
#             o $p->{pci_bar2_size_mb} will be set to the RC_BAR2
#               viewport size.
#             o $p->{pci_bar2_val_mb} will be set to the pci_addr
#               of the viewport.
########################################################################
sub mapped_to_pcie_ext_region_alg($)
{
	my ($p) = shift;

	$p->{success} = 0;
	my $last_pci_addr_mb = 0;
	my $r = $p->{ra_regions};
	my $scb_size_sum_mb = 0;

	for (my $i = 0; $i < @{$p->{ra_regions}}; $i++) {
		my $e0 = $r->[$i];
		my $e1 = $r->[$i+1];
		$e0->{pci_addr_mb} = $last_pci_addr_mb;

		if ($e1 && $e1->{memc} == $e0->{memc}) {
			# We do have an extension.
			$e1->{pci_addr_mb} = $last_pci_addr_mb + $e0->{size_mb};
			$e0->{scb_size_mb} = $e1->{scb_size_mb} =
			    round_up_to_pow2($e0->{size_mb} + $e1->{size_mb});
			$last_pci_addr_mb += $e0->{scb_size_mb};
			$i++;
		} else {
			# we do not have an extension for this memc.
			$e0->{scb_size_mb} = round_up_to_pow2($e0->{size_mb});
			$last_pci_addr_mb += $e0->{scb_size_mb};
		}
		$scb_size_sum_mb += $e0->{scb_size_mb};
	}
	$p->{success} = 'mapped_to_pcie_ext_region';

	$p->{pci_bar2_size_mb} = round_up_to_pow2($scb_size_sum_mb);

	if ($p->{tot_mem_mb} <= 3*1024 &&
	    $p->{pci_bar2_size_mb} <= 4*1024)
	{
	    # Special case where we can keep dma-addrs < 4GB.
	    $p->{pci_bar2_val_mb} = 0;
	    $p->{msi_target_addr} = ($p->{pci_bar2_size_mb} == 4*1024)
		? 'f_ffff_fffc' : 'ffff_fffc';
	}
	else {   
	    $p->{pci_bar2_val_mb} = $p->{pci_bar2_size_mb};
	    if ($p->{pci_bar2_val_mb} >= 4*1024) {
		$p->{msi_target_addr} = 'ffff_fffc';
	    } else {
		# I doubt we will ever get here.
		$p->{msi_target_addr} = '0000_0000';
	    }		
	}

	return $p;
}


########################################################################
#
########################################################################
sub processed_pcie_outbound_ranges_board()
{
	# Now we compute the "ranges" vectors.	We used to
	# do this at PCIe autogeneration time in Devices.pm,
	# but certain boards' configurations require the
	# outbound PCIe location to be moved higher in
	# PCIe space.
	#
	# For chips with pre-v7 memory map, we start with four
	# segments, starting at 0xc0000000, each with size 128M.
	# Starting v7 memory map, two 128M size segments start
	# at 0x30000000.
	#
	# Each outbound window is described by a range,
	# currently a 7-tuple. The numbers in the range are:
	#
	# (<pci-info>,
	#  <pci-addr-hi>,  <pci-addr-lo>,
	#  <cpu-addr-hi>,  <cpu-addr-lo>,
	#  <size-hi>  <size-lo>)
	#
	my @all_board_ranges;
	for (my $i = 0; $i < $num_pcie; $i++) {
		my @r0 = qw/0x02000000
			    0x00000000 0xc0000000
			    0x00000000 0xc0000000
			    0x00000000 0x08000000/;
		my @r1 = qw/0x02000000
			    0x00000000 0xc8000000
			    0x00000000 0xc8000000
			    0x00000000 0x08000000/;
		my @r2 = qw/0x02000000
			    0x00000000 0xd0000000
			    0x00000000 0xd0000000
			    0x00000000 0x08000000/;
		my @r3 = qw/0x02000000
			    0x00000000 0xd8000000
			    0x00000000 0xd8000000
			    0x00000000 0x08000000/;
		my @v7r0 = qw/0x02000000
			    0x00000000 0xf0000000
			    0x00000000 0x30000000
			    0x00000000 0x04000000
			    0x42000000
			    0x00000000 0xf4000000
			    0x00000000 0x34000000
			    0x00000000 0x04000000/;
		my @v7r1 = qw/0x02000000
			    0x00000000 0xf8000000
			    0x00000000 0x38000000
			    0x00000000 0x04000000
			    0x42000000
			    0x00000000 0xfc000000
			    0x00000000 0x3c000000
			    0x00000000 0x03f00000/;

		my @ranges;
		my $chip = ::get_family_name();

		if ($chip =~ /^(7250|7260|7268|7271|7364|7366|74371|7439|7445)/
		    && $num_pcie == 1) {
			# The only device gets the full 512M.
			@ranges = (@r0, @r1, @r2, @r3);

		} elsif ($chip =~ /^(7445|7439|7366)/ && $num_pcie == 2) {
			# Each device gets 256M.
			@ranges = ($i == 0) ? (@r0, @r1) : (@r2, @r3);
		} elsif ($chip =~ /^(7278)/ && $num_pcie == 2) {
			# V7 memory map: each device gets 128M.
			@ranges = ($i == 0) ? @v7r0 : @v7r1;
		} else {
			die "$P: need to address PCIe ranges for $chip";
		}
		push @all_board_ranges, [@ranges];
	}

	return @all_board_ranges;
}


########################################################################
# PURPOSE:
#   This sub computes all of the PCIe DT properties for a board and
#   queues them so that they will be processed at runtime.
#
# PARAMS:
#   $b -- the board object.
#   $ra_outbound_ranges -- arrayref of the outbound range values already
#           computed.
#   $cond -- (optional) -- a condition applied to the generated DT
#           properties.  The anticipated choices for this are
#           'V7_64==0' and 'V7_64==1'.
#
# RETURNS:
#   undef
########################################################################
sub processed_pcie_inbound_ranges_board($$;$)
{
	my ($b, $ra_outbound_ranges, $cond) = @_;
	my $ddr = $b->{ddr};
	my $mmap = ($cond && $cond eq 'V7_64==1')
	    ? $b->{drammap64} : $b->{drammap};


	# Make a local copy of the ranges since we may change them.
	$ra_outbound_ranges = Storable::dclone($ra_outbound_ranges);

	# Put the mmap information into a new arrayref ($map) where all
	# mappings are collated by memc number; eg all entries
	# pertaining to memc0 are in the arrayref $map->[0].
	my $map = [];
	foreach my $mm (@$mmap) {
		my $i = $mm->{memc};
		$map->[$i] = []
		    if (!defined $map->[$i]);
		push @{$map->[$i]}, $mm;
	}
	my @a_regions;

	# Process map and ddr into a single array of structures ($map).
	# The memory units in the structures will all be MBs so we
	# neither have to worry about BigInt nor old Perls that use
	# 32-bit integers.
	for (my $memc = 0; $memc < @$ddr; $memc++) {
		my $d = $ddr->[$memc];
		my $m = $map->[$memc];
		my $d_size_mb = $d->{size_mb};
		next if $d_size_mb eq '-';

		my $ext0 = $m->[0];
		my $ext1 = $m->[1];
		my $ext0_size_mb = hexstr_to_mbytes($ext0->{size_bytes});
		
		if ($ext0_size_mb < $d_size_mb) {
			# We have extended memory for $memc.
			my $ext1_size_mb = $d_size_mb - $ext0_size_mb;
			my $scb_size_mb = round_up_to_pow2($ext0_size_mb
							   + $ext1_size_mb);

			my $e0 =
			{ memc => $memc,
			  ext => 0,
			  addr_mb => hexstr_to_mbytes($ext0->{to_bytes}),
			  size_mb => $ext0_size_mb,
			  pci_addr_mb => undef,
			  scb_size_mb => $scb_size_mb,
			};
			my $e1 =
			{ memc => $memc,
			  ext => 1,
			  addr_mb => hexstr_to_mbytes($ext1->{to_bytes}),
			  size_mb => $ext1_size_mb,
			  pci_addr_mb => undef,
			  scb_size_mb => $scb_size_mb,
			};
			push @a_regions, ($e0, $e1);
		} else {
			my $e =
			{ memc => $memc,
			  ext => 0,
			  addr_mb => hexstr_to_mbytes($ext0->{to_bytes}),
			  size_mb => $d_size_mb,
			  pci_addr_mb => undef,
			  scb_size_mb => round_up_to_pow2($d_size_mb),
			};
			push @a_regions, $e;
		}
	}

	my $tot_mem_mb = 0;
	foreach my $region (@a_regions) {
	    $tot_mem_mb += $region->{size_mb};
	}
	my $tot_mem_str = ($tot_mem_mb % 1024)
	    ? sprintf '%dMB', $tot_mem_mb
	    : sprintf '%dGB', ($tot_mem_mb >> 10);
	
	my $pkg = {
		ra_regions => \@a_regions,
		pci_bar2_size_mb => 4 * 1024,
		pci_bar2_val_mb => 0,
		success => 0,
		tot_mem_mb => $tot_mem_mb,
	};

	my ($ident, $dma);
	$ident = identity_map_alg( Storable::dclone($pkg) );
	$dma = mapped_to_pcie_ext_region_alg( Storable::dclone($pkg) );

	if (!$dma->{success}) {
	    	warn ::Dumper $pkg->{ra_regions};
		die "$P: Err: could not map pci ranges!\n ";
	}

	# @dma_ranges is a collection of mappings concatenated together.
	# Each "mapping" requires 7 integers:
	#	1 flags
	#	2 pcie_addr
	#	2 cpu_addr
	#	2 size
	# So if @dma_ranges has 21 elements, it contains three mappings:
	# @dma_ranges[0..6], @dma_ranges[7..13], @dma_ranges[14..20].
	my @dma_ranges = ();
	my $flags = '0x43000000'; # prefetchable, 64-bit

	if (1) {
		# We want to migrate towards always using dma-ranges
		# and phase out log2-scb-sizes.  So we always put
		# out our dma-ranges information and if the Linux
		# is modern enough it will use that instead of the
		# log2-scb-size info.
		my $pci_offset = Math::BigInt->new($dma->{pci_bar2_val_mb})
		    << 20;
		foreach my $r (@{$dma->{ra_regions}}) {
			die 'look at this to see what is going on'
			    if ($r->{size_mb} == 0);
			
			my @a = ($flags);
			push @a, BcmUtils::bigint_to_two_hexint($pci_offset +
				(Math::BigInt->new($r->{pci_addr_mb}) << 20));
			push @a, BcmUtils::bigint_to_two_hexint(
				Math::BigInt->new($r->{addr_mb}) << 20);
			push @a, BcmUtils::bigint_to_two_hexint(
				Math::BigInt->new($r->{size_mb}) << 20);
			push @dma_ranges, map { hex($_); } @a;
		}
	}

	# @scb_log2_sizes is an array of size #mem controllers.
	# Each entry is the log2 of the size of the SCB window
	# that is mapped to PCIe space.	 Note that we sometimes
	# make this window larger than there is memory in the
	# controller as we "cover" gaps between successive
	# memc's to preserve the identity map.
	my @scb_log2_sizes = ();
	if ($ident->{success}) {
		my $ra = $ident->{ra_regions};
		foreach my $r (@$ra) {
			next if $r->{ext};
			push @scb_log2_sizes, log2_of_pow2($r->{scb_size_mb})
			    + 20;
		}
	}

	# Set the per pcie properties.
	for (my $i = 0; $i < $num_pcie; $i++) {
		my $node = $pcie_nodes[$i];
		my $args;

		##################
		# Set property 'brcm,log2-scb-sizes'.
		##################
		if ($ident->{success}) {
			$args = ['-root' => '/', '-node' => $node,
			    '-prop' => 'brcm,log2-scb-sizes',
			    '-vec' => \@scb_log2_sizes, '-cond' => $cond];
			::subcmd_dt_ops('prop', $args, $b);
		}

		##################
		# Set property 'ranges'.
		##################
		if (@{$ra_outbound_ranges}) {
			$args = ['-root' => '/', '-node' => $node,
			    '-prop' => 'ranges', '-cond' => $cond,
			    '-vec' => $ra_outbound_ranges->[$i]];
			::subcmd_dt_ops('prop', $args, $b);

			my $title = sprintf "%s %s (%s)%s",
				"ranges     prop (outbound) for",
				$b->{board}, $b->{chip},
				$cond ? " $cond" : "";
		}

		if ($i == 0 && $DEBUG && $ident->{success})
		{
			my $t0 = $ident->{identity_map_faux_scb_size};
			my $t1 = defined $t0;
			my $fmt = "[DEBUG: %s (%s, %s)%s identity_map%s]\n";
			printf $fmt, $b->{board}, $b->{chip}, $tot_mem_str,
				$cond ? " $cond" : "",
				$t1 ? " (faux-scb${t0}-size)" : "",
		}

		##################
		# Set property 'dma-ranges'.
		##################
		if (@dma_ranges) {
			$args = ['-root' => '/', '-node' => $node,
			    '-prop' => 'dma-ranges',
			    '-vec' => \@dma_ranges, '-cond' => $cond];
			::subcmd_dt_ops('prop', $args, $b);

			my $title = sprintf "%s (%s, %s)%s %s",
				$b->{board}, $b->{chip}, $tot_mem_str,
				$cond ? " $cond" : "",
				"dma-ranges (inbound) msi\@$dma->{msi_target_addr}";
			dump_ranges($title, \@dma_ranges)
			    if ($i == 0 && $DEBUG);
		}
	}
}


########################################################################
# PURPOSE:
#   Calculate and process the ranges and dma-ranges for all PCIe nodes
#   for all boards.
########################################################################
sub processed_pcie_ranges()
{
	my $rh = ::get_bchp_info(::get_family_name());
	$num_pcie = ::get_dt_autogen_presence('pcie')
		? BcmUtils::get_num_pcie($rh->{rh_defines}) : 0;
	return if !$num_pcie;

	@pcie_nodes = ();
	for (my $i = 0; $i < $num_pcie; $i++) {
		my ($beg, $size);
		if ($num_pcie > 1) {
			($beg, $size) = BcmDt::Devices::get_reg_range_from_regexp(
			$rh, "^BCHP_PCIE_${i}");
		} else {
			($beg, $size) = BcmDt::Devices::get_reg_range_from_regexp(
			$rh, "^BCHP_PCIE_(?!DMA)");
		}
		die if !$beg;
		$pcie_nodes[$i] = sprintf("pcie\@%x", $beg);
	}

	my @ranges = processed_pcie_outbound_ranges_board();

	my $title = sprintf "ranges prop     (outbound, %s DEFAULT)",
		::get_family_name();
	dump_ranges($title, \@ranges)
	    if $DEBUG;

	for my $b (::get_valid_boards()) {
		my $m32 = $b->{drammap} && @{$b->{drammap}};
		my $m64 = $b->{drammap64} && @{$b->{drammap64}};

		if ($m64 && $m32) {
			processed_pcie_inbound_ranges_board($b,
			    \@ranges, 'V7_64==0');
			processed_pcie_inbound_ranges_board($b,
			    \@ranges, 'V7_64==1');
		} elsif ($m64) {
			processed_pcie_inbound_ranges_board($b,
			    \@ranges, 'V7_64==1');
		} elsif ($m32) {
			processed_pcie_inbound_ranges_board($b,
			    \@ranges, '');
		} else {
			die "$P: Err: no memory maps???";
		}
	}
}


1;
