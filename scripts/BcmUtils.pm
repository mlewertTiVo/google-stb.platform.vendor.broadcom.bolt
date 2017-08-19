################################################################################
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
# 
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
################################################################################
package BcmUtils;
use strict;
use warnings FATAL=>q(all);
use POSIX;
use Data::Dumper;
use File::Basename;

my $P = basename $::0;
our $Debug = 0;

my $year = (localtime())[5] + 1900;
my $cheader = "/***************************************************************************
 *     Copyright (c) 2012-$year, Broadcom
 *     All Rights Reserved
 *     Confidential Property of Broadcom
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
\n";

my $makeheader = "#
#	 Copyright (c) 2012-$year, Broadcom
#	 All Rights Reserved
#	 Confidential Property of Broadcom
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
#\n";

sub create_copyright_header($)
{
	my $type = shift;
	my $string = "";

	if ($type =~ /[h c]/) {
		return ($cheader);
	}
	elsif ($type =~ /mk/) {
		return ($makeheader);
	}
}

########################################################################
# FUNCTION:
#   create_pin_regexp()
#
# DESCRIPTION:
#   Takes in a pin that describes some pin(s), outputs a regexp.
#
# PARAMS:
#   $pin -- the string that describes the pin.  It can be of these
#           following forms:
#               'gpio_012'
#               'gpio_012:gpio015'
#               'gpio_012,gpio_013,gpio_014,gpio_015'
#   
#            Note that if you are trying to match the AON pins, you
#            must use 'aon_gpio_012'.
#
# RETURNS:
#   An array of three parameters:
#       o error string, null if no error
#       o the regexp
#       o the number of pin terms in the regexp
########################################################################
sub create_pin_regexp($)
{
	my $pin = shift;
	my $rval = 0;
	$pin =~ s/\s+//g;
	my @a = split /,/, $pin;
	foreach (@a) {
		next if (0>index($_, ':'));
		my ($t0, $t1) = split /:/, $_;
		my ($a0, $d0) = ($1, $2) if ($t0 =~ /^(.*?)(\d+)$/);
		my ($a1, $d1) = ($1, $2) if ($t1 =~ /^(.*?)(\d+)$/);

		return ("cannot understand pin sequence '$_'", undef, undef)
			if ($a0 ne $a1 || length($d0) != length($d1));

		my ($min,$max) = ($d0 > $d1) ? ($d1, $d0) : ($d0, $d1);
		my $re = '';
		my @b;
		my $n = length $min;
		for (my $i=$min; $i<=$max; $i++) {
			# must preserve leading zeroes.
			while (length($i) < $n) {
				$i = '0' . $i;
			}
			push @b, $a0 . $i; 
		}
		$_ = join('|', @b);
	}
	$pin = '^(?:' . join('|', @a) . ')';
	my @temp = split(/\|/, $pin);
	my $npins = scalar @temp;

	return ($rval, $pin, $npins);
}



########################################################################
# FUNCTION:
#   pmux_match($$$$)
#
# DESCRIPTION:
#   
#
# PARAMS:
#   $rh_pmux -- the hashref that describes all of the pinmux info.
#   $pin_in -- the name or partial name of a pin.  For example,
#       this can be just 'gpio' if you want config.pl to do the
#       matching, or it can be 'gpio_012', or 'gpio_012:gpio_014',
#       or 'gpio_012,gpio_013,gpio_014'.
#   $select -- the name or partial name of a pinmux selection
#       field.  An example: 'sd_card0'.
#   $pad_ctrl -- (optional) a string, one of 'PULL_NONE', 'PULL_UP',
#       or 'PULL_DOWN'.
#
# RETURNS:
#   An array of two arrayrefs; the first one is for pin mux matches
#   and the second arrayref is for pad cntl matches.  Each arrayref
#   has elements that are hashrefs and look like this:
#          {
#            'pin' => 'gpio_076',
#            'type' => 'st_pm',
#            'pin_re' => '^(?:gpio)',
#            'select' => 'SD_CARD0_CMD'
#          },
#
#   
########################################################################
sub pmux_match($$$$)
{
	my ($rh_pmux, 
	    $pin_in, 
	    $select_in,
	    $pad_ctrl,
	   ) = @_;

	my $type = ($pin_in =~ /^aon/i
		    ? 'ao'
		    : ($pin_in =~ /^bnm/i
		       ? 'cm'
		       : 'st'));

	my $rh_pm = $rh_pmux->{"${type}_pm"};
	my $rh_pp = $rh_pmux->{"${type}_pp"};

	# Tweak the inputs.
	my ($err, $pin_re) = BcmUtils::create_pin_regexp($pin_in);
	return $err if $err;
	my $select_re = '^' . uc $select_in;
	$pad_ctrl = uc $pad_ctrl if $pad_ctrl;

	my (@a, @b);

	foreach my $pin (sort keys %{$rh_pm}) {
		my $rh_pin = $rh_pm->{$pin};
		next if $pin !~ /$pin_re/;
		my $select;
		foreach (keys %{$rh_pin}) {
			if (/$select_re/) {
				::cfg_error("regexp '$select_in' matches "
					. "multiple selections ($_, $select "
					. "or more) for $pin")
					if ($select);
				$select = $_;
			}
		}
		if ($select) {
			push @a,
				{ type => $type . '_pm',
				  pin_re => $pin_re,
				  pin => $pin,
				  select => $select, };

			if ($pad_ctrl) {
				$pin .= '_pad_ctrl';
				if (exists $rh_pp->{$pin} &&
				    exists $rh_pp->{$pin}->{$pad_ctrl}) {
					push @b,
						{ type => $type . "_pp",
						  pin_re => $pin,
						  pin => $pin,
						  select => $pad_ctrl,
						};
				}
			}
		} 
	}

	# This is a special case for dedicated GPIOs.  These pins do
	# not have a corresponding GPIO pin per se, but they might
	# have a pad ctrl.  In this special case, the select value
	# must be '__NONE__'.
	if (!@a && !@b && $select_in eq '__NONE__') {
		my $pin = $pin_in . '_pad_ctrl';
		if (exists $rh_pp->{$pin}
			&& exists $rh_pp->{$pin}->{$pad_ctrl})
		{
			push @b,
				{ type => $type . "_pp",
				  pin_re => $pin,
				  pin => $pin,
				  select => $pad_ctrl,
				};
		}
	}

	return (\@a, \@b);
}



########################################################################
# FUNCTION:
#   grok_bchp_for_pin_mux()
#
# DESCRIPTION:
#   Generates a database for all of the pinmux registers.
#
# PARAMS:
#   $ra_defines -- an arrayref of all of the BCHP definitions.
#   
# RETURNS:
#   A single hashref that contains four hashrefs:
#     {
#         st_pm => { ... } # standard pin control info
#         st_pp => { ... } # standard pad control info
#         ao_pm => { ... } # aon pin control info
#         ao_pp => { ... } # aon pad control info
#         cm_pm => { ... } # cm pin control info
#         cm_pp => { ... } # cm pad control info
#     }
#   
#   Each of the hashrefs above contain as keys all of the names
#   of the pin/pad fields they appear in the RDB.  Here is an example
#   of one such key, and the hashref it points to:
#
#      'aon_gpio_07' => {
#          'MASK' => 0xf0000000,    # bitfield mask
#          'SHIFT' => '28',         # shift amount for field
#          'reg' => '0',            # the aon_gpio register number
#          'reg_addr' => 0x410500,  # the address of the containing register
#
#          'AON_GPIO_07' => '0',    # field name => value
#          'LED_LS_3' => '1',       # field name => value
#          'UART_RTS_0' => '2',     # field name => value
#          'LED_LD_15' => '3',      # field name => value
#          'DEFAULT' => 0,          # field name => value
#      }
#   
#   
########################################################################
sub grok_bchp_for_pin_mux($)
{
	my $ra_defines = shift;
	my @a;
	my %h = (st_pm => {}, 
		st_pp => {},
		ao_pm => {},
		ao_pp => {},
		cm_pm => {},
		cm_pp => {},
		st_pm_reg => [], 
		st_pp_reg => [],
		ao_pm_reg => [],
		ao_pp_reg => [],
		cm_pm_reg => [],
		cm_pp_reg => [],
		);

	foreach my $x (@$ra_defines) {
		my $key = $x->[0];
		my $val = $x->[1];

		$h{st_pm_reg}->[$1] = $val
			if ($key =~ /^BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_(\d+)$/);
		$h{st_pp_reg}->[$1] = $val
			if ($key =~ /^BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_(\d+)$/);
		$h{ao_pm_reg}->[$1] = $val
			if ($key =~ /^BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_(\d+)$/);
		$h{ao_pp_reg}->[$1] = $val
			if ($key =~ /^BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_(\d+)$/);
		$h{cm_pm_reg}->[$1] = $val
			if ($key =~ /^BCHP_CM_TOP_CTRL_PIN_MUX_CTRL_(\d+)$/);
		$h{cm_pp_reg}->[$1] = $val
			if ($key =~ /^BCHP_CM_TOP_CTRL_PIN_MUX_PAD_CTRL_(\d+)$/);

		next if ($key !~ /^BCHP_
				(SUN_TOP|AON_PIN|CM_TOP)_
				CTRL_PIN_MUX_
				(PAD_)?
				CTRL_
				(\d+)_
				([a-z0-9_]+)_
				([A-Z0-9_]+)$/x);
		my $type;
		if ($2) {
			$type = { SUN_TOP => 'st_pp', 
				  AON_PIN => 'ao_pp', 
				  CM_TOP => 'cm_pp' }->{$1};
		} else {
			$type = { SUN_TOP => 'st_pm', 
				  AON_PIN => 'ao_pm', 
				  CM_TOP => 'cm_pm' }->{$1};
		}
		my $rh = $h{$type};
		my $reg = $3;
		my $pname = $4;
		my $select = $5;
		$rh->{$pname} ||= {};
		$rh->{$pname}->{reg} = $reg;
		$rh->{$pname}->{reg_addr} = $h{"${type}_reg"}->[$reg];
		die "$P: internal error"
			if (!$rh->{$pname}->{reg_addr});
		$rh->{$pname}->{$select} = $val;
	}
	delete $h{st_pm_reg};
	delete $h{st_pp_reg};
	delete $h{ao_pm_reg};
	delete $h{ao_pp_reg};
	delete $h{cm_pm_reg};
	delete $h{cm_pp_reg};
	return \%h;
}



########################################################################
# FUNCTION:
#   grok_defines_from_c_incl_files
#
# DESCRIPTION:
#   Takes a list of bchp include files, processes them, and records
#   all of their definitions.  
#
# PARAMS:
#   $f0 ... filename pattern (can be filename but gets globbed)
#   $f1 ... filename pattern (opt)
#   ...
#   $fn ... filename pattern (opt)
# 
# RETURNS:
#   $rh .. hashref of defines
#   $ra .. arrayref of defines, each element is [string, val]
########################################################################
sub grok_defines_from_c_incl_files
{
	my $rh = {};
	my $ra = [];
	my @file_patterns = @_;
	my @files;
	my $f;

	foreach $f (@file_patterns) {
		my @a = glob($f);
		push @files, @a;
		warn "$P: wrn: no inc files match '$f'\n"
			if ($Debug && !@a);
	}	

	foreach $f (@files) {
		my $fh;
		if (!open($fh, '<', $f)) {
			warn "$P: wrn: could not open '$f'.\n"
				if ($Debug);
			next;
		}

		my $mode = '';
		while (<$fh>) {
			if ($mode eq 'multi-line-comment') {
				next if (! s/^.*\*\///);
				$mode = '';
			}
			s/\/\*.*?\*\///g;
			$mode = 'multi-line-comment'
				if (s/\/\*.*$//);
			
			if (/^\#define\s+(\S+)\s+(\S+)/) {
				my ($key, $val) = ($1, $2);
				warn "$P: wrn: redefining '$key'.\n"
					if (exists $rh->{$key});
				$val = hex($val) if ($val =~ /^0x[0-9A-F]{1,8}$/i);
				$rh->{$key} = $val;
				push @{$ra}, [$key, $val];
			}
		}
		close($fh);
	}
	return ($rh, $ra);
}

sub grok_memc_client_names($$)
{
	my @memc_client_defines;
	my ($fp, $chip_arch) = @_;
	my @file_patterns = @{$fp};
	my @files;
	my $f;

	foreach $f (@file_patterns) {
		my @a = glob($f);
		push @files, @a;
		warn "$P: wrn: no inc files match '$f'\n"
			if ($Debug && !@a);
	}

	foreach $f (@files) {
		my $fh;
		if (!open($fh, '<', $f)) {
			warn "$P: wrn: could not open '$f'.\n"
				if ($Debug);
			next;
		}

		while (<$fh>) {
			# Each client may be present on any MEMC, we organize
			# the data as per-MEMC array of clients, with the following
			# name, comments, instance name
			my @line;
			my $num_memc = 0;
			s/\r|\n//g;
			if(/^#/) {
				@line = (undef,$_);
			} elsif(/^\s+$/) {
				@line = (undef);
			} elsif(/([a-zA-Z][^\t]+)\t( |\d*)\t( |\d*)\t(\d+)(\t|$)(.*)/) {
				#print "3: $_\n";
				@line = ([$1,$2,$3,$4],$6);
				$num_memc = 3;
			} elsif(/([a-zA-Z][^\t]+)\t( |\d*)\t(\d+)(\t|$)(.*)/) {
				#print "2: $_\n";
				@line = ([$1,$2,$3],$5);
				$num_memc = 2;
			} elsif(/([a-zA-Z][^\t]+)\t(\d+)(\t|$)(.*)/) {
				#print "1: $_\n";
				@line = ([$1,$2],$4);
				$num_memc = 1;
			} else {
				@line = (undef,$_);
			}

			# Organize data as per-MEMC arrays of client names
			for (my $memc = 0; $memc < $num_memc; $memc++) {
				$memc_client_defines[$memc][$2] = ($1);
			}
		}

		print Dumper(\@memc_client_defines) if ($Debug);

		close ($fh);
	}

	die "Missing BCHP client names" if scalar(@memc_client_defines) == 0 &&
		$chip_arch ne "MIPS";

	return \@memc_client_defines;
}

########################################################################
# FUNCTION:
#   bigint_to_two_hexint
#
# DESCRIPTION:
#   Takes a Math::Bigint object number and split it in two 32-bits
#   pairs (hi and lo)
#
# PARAMS:
#   $bigint ... the large number to split
#
# RETURNS:
#   $hi, $lo ... a two entry array containing the high 32-bits part of the
#   number and the low 32-bits part of the number
########################################################################
sub bigint_to_two_hexint($) {
	my $bigint = Math::BigInt->new(shift);
	my $lo = $bigint & 0xffffffff;
	$lo = $lo->as_hex();
	my $hi = $bigint->brsft(32)->as_hex();
	return ($hi, $lo);
}


########################################################################
# FUNCTION:
#   bigint_to_two_int
#
# DESCRIPTION:
#   Takes a Math::Bigint object number and split it in two 32-bits
#   pairs (hi and lo).
#
# PARAMS:
#   $bigint ... the large number to split
#
# RETURNS:
#   $hi, $lo ... two scalars.
########################################################################
sub bigint_to_two_int($) {
	my $bigint = Math::BigInt->new(shift);
	my $lo = $bigint & 0xffffffff;
	my $hi = $bigint->brsft(32);
	return ($hi, $lo);
}


########################################################################
# FUNCTION:
#   bigint_is_power_of_two
#
# DESCRIPTION:
#   Takes a Math::Bigint object number and return if it is a power of
#   two
#
# PARAMS:
#   $x ... the large number to evaluate for power of two
#
# RETURNS:
#   true if the number is a power of two, false otherwise
########################################################################
sub bigint_is_power_of_two($)
{
	my $x = Math::BigInt->new(shift);
	return (($x != 0) && (($x & (~$x + 1)) == $x));
}

########################################################################
# FUNCTION:
#   addtab
#
########################################################################
sub addtab($$)
{
	my($pfx, $tcol) = @_;

	my $ret = $pfx;
	my $col = length($pfx);

	while ($col < $tcol) {
		$ret .= "\t";
		$col += 8 - ($col % 8);
	}
	return $ret;
}

# Returns the number of bits set in the provided integer argument.
sub count_set_bits($)
{
	my $x = shift;
	unpack("%32b*", pack('L', $x));
}

sub get_chip_family_id($)
{
	my $bchp_defines = shift;
	my $fam = $bchp_defines->{BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID_chip_family_id_DEFAULT};
	return $fam & 0xf0000000 ? $fam >> 16 : $fam >> 8
}

sub get_chip_family_rev($)
{
	my $bchp_defines = shift;
	my $rev = $bchp_defines->{BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID_chip_family_id_DEFAULT};

	return $rev & 0xff;
}

sub get_num_serial($)
{
	my ($bchp_defines,
	   ) = @_;

	my @a = sort grep { /^BCHP_UART[A-Z]_REG_START$/ }
		keys %${bchp_defines};
	return scalar(@a);
}

sub get_num_bsc($)
{
	my $bchp_defines = shift;
	my @a = sort grep { /^BCHP_BSC[A-Z]_REG_START$/ }
		keys %${bchp_defines};
	return scalar(@a);
}

sub get_num_usb($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_USB%s_CAPS_REG_START', $count ? $count : '');
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}


sub get_num_genet($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_GENET_%d_SYS_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}


sub get_num_sata($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_SATA%d_TOP_CTRL_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count if ($count != 0);
	return $bchp_defines->{BCHP_SATA_TOP_CTRL_REG_START} ? 1 : 0;
}


sub get_num_sata_phy($$)
{
	my ($bchp_defines, $sata) = @_;
	my $count = 0;
	my ($tmp0, $tmp1, $tmp2);
	do {
		$tmp0 = sprintf("${sata}_PORT%d_PCB_REG_START", $count);
		$count++ if $bchp_defines->{$tmp0};
		$tmp1 = sprintf("BCHP_PORT%d_SATA3_PCB_REG_START", $count);
		$count++ if $bchp_defines->{$tmp1};
	} while ($bchp_defines->{$tmp0} or $bchp_defines->{$tmp1});

	return $count if ($count);

	# Legacy platforms did not have PCB registers available in RDB, but
	# they were there anyway, let's just use the PORTi_CTRL
	do {
		$tmp0 = sprintf("BCHP_SATA_PORT%d_CTRL_REG_START", $count);
		$count++ if $bchp_defines->{$tmp0};
	} while ($bchp_defines->{$tmp0});

	return $count;
}

sub get_num_sata_phy_ctl($)
{
	my $bchp_defines = shift;
	my $c = 0;
	my $tmp;
	do {
		#Starting offset for these registers is 1, not 0.
		$tmp = sprintf('BCHP_SATA_TOP_CTRL_PHY_CTRL_%d', $c + 1);
		$c++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $c;
}

sub get_num_pcie($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_PCIE_%d_RC_CFG_TYPE1_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	# 40nm chips only had on PCIE RC
	$count++ if defined($bchp_defines->{BCHP_PCIE_RC_CFG_TYPE1_REG_START});

	return $count;
}

sub get_num_memc($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $template;
	my $tmp;

	if ($bchp_defines->{'BCHP_MC_GLB_0_REG_START'}) {
		$template = 'BCHP_MC_GLB_%d_REG_START';
	} else {
		$template = 'BCHP_MEMC_GEN_%d_REG_START';
	}

	do {
		$tmp = sprintf($template, $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}

sub get_num_moca($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_MOCA_HOSTMISC_REG_START}) ? 1 : 0;
}

my $get_num_l1_intr_regs;
sub get_num_l1_intr_regs($)
{
	return $get_num_l1_intr_regs
		if defined $get_num_l1_intr_regs;
	my $bchp_defines = shift;
	my $fmt = 'BCHP_HIF_CPU_INTR1_INTR_W%d_STATUS';
	my $i = 0;
	for ($i=0; $i<100; $i++) {
		my $r = sprintf($fmt, $i);
		last if !defined $bchp_defines->{$r};
	}
	return ($get_num_l1_intr_regs = $i);
}

sub get_num_sun_l2($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_SUN_L2_REG_START}) ? 1 : 0;
}

sub get_num_hif_spi_l2($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_HIF_SPI_INTR2_REG_START}) ? 1 : 0;
}


sub get_num_gisb_arb($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_SUN_GISB_ARB_REG_START}) ? 1 : 0;
}

sub get_num_hif_l2($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_HIF_L2_REG_START}) ? 1 : 0;
}

sub get_num_aon_pm_l2($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_AON_PM_L2_REG_START}) ? 1 : 0;
}

sub get_num_avs_host_l2($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_AVS_HOST_L2_REG_START}) ? 1 : 0;
}

sub get_num_avs_cpu($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_AVS_CPU_DATA_MEM_REG_START}) ? 1 : 0;
}

sub get_l2_intc_mapping($)
{
	my $l2_intc = shift;
	my %l2_irq_subst = (
		"sun_l2" => "SYS",
		"hif_intr2" => "HIF",
		"hif_spi_intr2" => "HIF_SPI",
		"aon_pm_l2" => "SYS_PM",
		"avs_host_l2" => "AVS",
		"memc_l2_0_0" => "MEMC0",
		"memc_l2_0_1" => "MEMC0",
		"memc_l2_0_2" => "MEMC0",
		"memc_l2_1_0" => "MEMC1",
		"memc_l2_1_1" => "MEMC1",
		"memc_l2_1_2" => "MEMC1",
		"memc_l2_2_0" => "MEMC2",
		"memc_l2_2_1" => "MEMC2",
		"memc_l2_2_2" => "MEMC2",
		"upg_main_irq" => "UPG_MAIN",
		"upg_main_aon_irq" => "UPG_MAIN_AON",
		"upg_bsc_irq" => "UPG_BSC",
		"upg_bsc_aon_irq" => "UPG_BSC_AON",
		"upg_spi_aon_irq" => "UPG_SPI",
	);

	return $l2_irq_subst{$l2_intc};
}

sub get_l2_irq0_intc_mapping($)
{
	my $intc = shift;
	# intc to Level-1 interrupt
	my %irq_array = (
		# Do not include the UPG_UART[012] interrupts as those are shared interrupts
		# that receive special treatment
		"irq0" => [ "UPG_MAIN", "UPG_BSC" ],
		"irq0_aon" => [ "UPG_MAIN_AON", "UPG_BSC_AON", "UPG_SPI" ],
	);

	return $irq_array{$intc};
}

# internal intc mapping
# These contents do not vary from chip to chip.
my %irq_int_mapping = (
	"irq0" => {
		"uarta" => "forward",
		"uartb" => "forward",
		"uartc" => "forward",
		"all" => "UPG_MAIN"
	},
	"irq0_aon" => {
		"spi" => "UPG_SPI",
		"all" => "UPG_MAIN_AON",
	},
	);

# This should be called once after the bchp_defines are collected
# There are chip differences for bsc channel assigments this
# autogenrates the right channel assignment  so that the right
# irq0 and irq0_aon masks will get generated
sub gen_irq0_int_mapping($)
{
	my $bchp_defs = shift;

	# Just brute force, since some chips may have unpopulated BSC
	# controllers that create a hole (e.g: 7250 has no BSCB)
	my @a = split //, substr("abcdefghij", 0, 10);
	foreach (@a) {
		my $iic = 'BCHP_IRQ0_IRQEN_iic' . $_ . '_irqen_MASK';
		if ($bchp_defs->{$iic}) {
			$irq_int_mapping{irq0}->{"iic${_}"} = 'UPG_BSC';
		}

		$iic = 'BCHP_IRQ0_AON_IRQEN_iic' . $_ . '_irqen_MASK';
		if ($bchp_defs->{$iic}) {
			$irq_int_mapping{irq0_aon}->{"iic${_}"} = 'UPG_BSC_AON';
		}
	}
}

# This is called by the irq0 and irq0_aon controller node generation code
sub get_l2_irq0_intc_internal($$)
{
	my ($intc, $l2_irq) = @_;

	my $val = $irq_int_mapping{"$intc"}->{"$l2_irq"};

	if (!defined($val)) {
		$val = $irq_int_mapping{$intc}->{"all"};
	}

	return $val;
}

sub l2_intc_can_wake($)
{
	my $l2_intc = shift;
	my %wake_intcs = (
		"aon_pm_l2" => 1,
		"irq0_aon" => 1,
		"upg_main_aon_irq" => 1,
		"upg_bsc_aon_irq" => 1,
		"upg_spi_aon_irq" => 1,
	);

	return exists($wake_intcs{$l2_intc});
}

sub get_rdb_fields($$)
{
	my $regex = shift @_;
	my ($bchp_defs,) = @_;
	my @fields = ();
	my @a = sort grep {/$regex/} keys %${bchp_defs};
	push @fields, @a;
	return \@fields;
}

sub get_num_waketimer($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_WKTMR_REG_START}) ? 1 : 0;
}

sub get_num_avs_tmon($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_AVS_TMON_REG_START}) ? 1 : 0;
}

sub get_num_ddr_phy($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_DDR34_PHY_CONTROL_REGS_%d_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}

sub get_num_ddr_shimphy($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_SHIMPHY_ADDR_CNTL_%d_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}

sub get_num_spi($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_HIF_MSPI_REG_START}) &&
		defined($bchp_defines->{BCHP_BSPI_REG_START}) &&
		defined($bchp_defines->{BCHP_BSPI_RAF_REG_START}) ? 1 : 0;
}

sub get_num_mspi($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_MSPI_REG_START}) ? 1 : 0;
}

sub get_num_nand($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_NAND_REG_START}) ? 1 : 0;
}

sub get_num_systemport($)
{
	my $bchp_defines = shift;
	my $count = 0;

	$count++ if defined($bchp_defines->{BCHP_SYSTEMPORT_TOPCTRL_REG_START});

	my $tmp;
	do {
		$tmp = sprintf('BCHP_SYSTEMPORTLITE_%d_TOPCTRL_REG_START', $count);
				$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});
	return $count;
}

sub get_num_rf4ce($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_RF4CE_CPU_PROG0_MEM_REG_START}) ? 1 : 0;
}

sub get_num_systemport_queues($)
{
	my $bchp_defines = shift;
	my $n_sysport = get_num_systemport($bchp_defines);
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf('BCHP_SYSTEMPORT_TDMA_DESC_RING_%02d_COUNT', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	return $count if $count ne 0;

	do {
		$tmp = sprintf('BCHP_SYSTEMPORTLITE_%d_TDMA_DESC_RING_%02d_COUNT',
				$n_sysport -1, $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	return $count;
}

sub get_num_sf2_switch($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_SWITCH_CORE_REG_START}) ? 1 : 0;
}

sub get_sf2_top_rev($)
{
	my $bchp_defines = shift;
	my $top_rev = $bchp_defines->{BCHP_SWITCH_REG_SWITCH_REVISION_switch_top_rev_DEFAULT};
	if (defined($top_rev) && ($top_rev == 0)) {
		# FIXME: BCHP update is required
		$top_rev = 0x200;
	}
	return $top_rev;
}

sub get_sf2_rev($)
{
	my $bchp_defines = shift;
	my $sf2_rev = $bchp_defines->{BCHP_SWITCH_REG_SWITCH_REVISION_SF2_rev_DEFAULT};
	if (defined($sf2_rev) && ($sf2_rev == 0)) {
		# FIXME: BCHP update is required
		$sf2_rev = 0x400;
	}
	return $sf2_rev;
}

sub get_sf2_model_id($)
{
	my $bchp_defines = shift;
	my $model_id = $bchp_defines->{BCHP_SWITCH_CORE_MODEL_ID_MODELID_DEFAULT};
	return defined($model_id) ? $model_id : 0x53010;
}

sub get_num_systemport_tier_arb($$)
{
	my ($bchp_defines, $level) = @_;
	my $n_sysport = get_num_systemport($bchp_defines);
	my $count = 0;
	my $tmp;

	do {
		$tmp = sprintf('BCHP_SYSTEMPORT_TDMA_TIER_%d_ARBITER_%d_CTRL', $level, $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	return $count if $count ne 0;

	do {
		$tmp = sprintf('BCHP_SYSTEMPORTLITE_%d_TDMA_TIER_%d_ARBITER_%d_CTRL',
		$n_sysport - 1, $level, $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	# Try the other naming if we did not find anything
	if ($count == 0) {
		$tmp = sprintf('BCHP_SYSTEMPORT_TDMA_TIER_%d_ARBITER_CTRL', $level);
		$count++ if $bchp_defines->{$tmp};
		$tmp = sprintf('BCHP_SYSTEMPORTLITE_%d_TDMA_TIER_%d_ARBITER_CTRL',
				$n_sysport -1, $level);
		$count++ if $bchp_defines->{$tmp};
	}

	return $count;
}

sub get_num_sf2_rgmii_ports($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;

	do {
		$tmp = sprintf("BCHP_SWITCH_REG_RGMII_%d_CNTRL", $count + 1);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	return $count;
}

sub get_num_sf2_phys($)
{
	my $bchp_defines = shift;
	my $count;

	return defined($bchp_defines->{"BCHP_SWITCH_REG_QPHY_CNTRL"}) ? 4 : 1;
}

sub get_num_sf2_acb_queues($)
{
	my $bchp_defines = shift;
	my $count = 0;
	my $tmp;
	do {
		$tmp = sprintf("BCHP_SWITCH_ACB_ACB_QUEUE_%d_CONFIGURATION", $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	return $count;
}

sub get_sf2_has_pause_override($)
{
	my $bchp_defines = shift;

	return defined($bchp_defines->{"BCHP_SWITCH_FCB_FCB_PAUSE_OVERRIDE_CONTROL"}) ? 1 : 0;
}

sub get_sf2_has_packets_inflight($)
{
	my $bchp_defines = shift;

	return defined($bchp_defines->{"BCHP_SWITCH_ACB_ACB_QUEUE_0_PACKETS_IN_FLIGHT"}) ? 1 : 0;
}

sub get_sf2_num_switch_phy($)
{
	my $bchp_defines = shift;
	# Use a register which has 1-bit per-port of the switch
	my $val;
	my $i;

	$val = $bchp_defines->{"BCHP_SWITCH_REG_SPHY_STATUS_energy_det_masked_MASK"};
	if (!defined($val)) {
		$val = $bchp_defines->{"BCHP_SWITCH_REG_QPHY_STATUS_energy_det_masked_MASK"};
	}

	return 0 if !defined($val);

	for ($i = 31; $i >= 0; $i--) {
		if ($val & (1 << $i)) {
			return $i + 1;
		}
	}

	return 0;
}

sub get_sf2_num_switch_ports($)
{
	my $bchp_defines = shift;
	# Use a register which has 1-bit per-port of the switch
	my $val = $bchp_defines->{"BCHP_SWITCH_CORE_LNKSTS_LNK_STS_MASK"};
	my $i;

	return 0 if !defined($val);

	for ($i = 31; $i >= 0; $i--) {
		if ($val & (1 << $i)) {
			return $i + 1;
		}
	}

	return 0;
}

sub get_num_irq_l2($$)
{
	my ($bchp_defines, $name) = @_;

	return defined($bchp_defines->{"BCHP_" . uc($name) . "_REG_START"}) ? 1 : 0;
}

sub get_num_pwm($)
{
	my ($bchp_defines) = @_;
	my $count = 0;

	$count++ if (defined($bchp_defines->{"BCHP_PWM_REG_START"}));
	$count++ if (defined($bchp_defines->{"BCHP_PWMB_REG_START"}));

	warn "Found no PWM controllers\n" if !$count;

	warn "Found more than two PWM controllers\n"
		if (defined($bchp_defines->{"BCHP_PWMC_REG_START"}));

	return $count;
}

sub get_num_dtu_config($)
{
	my ($bchp_defines) = @_;
	my $count = 0;

	while ($bchp_defines->{"BCHP_MEMC_DTU_CONFIG_${count}_REG_START"}) {
		$count++;
	}

	return $count;
}

sub get_num_dtu_map($)
{
	my ($bchp_defines) = @_;
	my $count = 0;


	while ($bchp_defines->{"BCHP_MEMC_DTU_MAP_STATE_${count}_REG_START"}) {
		$count++;
	}

	return $count;
}

sub get_num_dpfe($)
{
	my ($bchp_defines) = @_;
	my $count = 0;

	while ($bchp_defines->{"BCHP_DPFE_CPU_${count}_REG_START"}) {
		$count++;
	}

	return $count;
}

sub get_num_wlan($)
{
	my ($bchp_defines) = @_;
	my $count = 0;

	$count++ if (defined($bchp_defines->{"BCHP_WLAN_INTF_RGR_BRIDGE_REG_START"}));

	return $count;
}

sub get_num_v3d_mmu($)
{
	my $bchp_defines = shift;
	my $count = 0;

	$count++ if (defined($bchp_defines->{"BCHP_V3D_MMUC_REG_START"}));

	return $count;
}

#SWBOLT-1715
sub mcp_wr_pairing_allowed($)
{
	my $family_name = shift;
	my %mcp_wp_allowed_tbl = (
	       "7250b0" => "1",
	       "7260a0" => "1",
	       "7268b0" => "1",
	       "7271b0" => "1",
	       "7278a0" => "1",
	       "7364b0" => "1",
	       "7364c0" => "1",
	       "74371a0" => "1",
	       "7439b0" => "1",
	       "7445e0" => "1",
        );

	return defined($mcp_wp_allowed_tbl{$family_name}) ? 1 : 0;
}

sub validate_gic_interrupt_cells($)
{
	my @gic_cells = @_;
	my ($type, $flags_cpu_mask) = @gic_cells;
	my ($flags, $cpu_mask);

	$cpu_mask = ($flags_cpu_mask >> 8);
	$flags = ($flags_cpu_mask & 0x0f);

	# Cannot have the interrupt type be neither SPI or PPI
	die "Invalid GIC interrupt type: $type\n" if ($type != 0 && $type != 1);
	# Can't have more than 4 CPUs for now
	die "Invalid CPU mask: $cpu_mask\n" if ($cpu_mask > 0xf);
	# Cannot have conflicting edge/level flags
	die "Invalid Trigger type: $flags\n" if ($flags % 2);
}

sub format_gic_interrupt_cells($$)
{
	my ($flags, $irq) = @_;
	my @cells;

	# Following the binding documentation in the Linux kernel sources
	# located in Documentation/devicetree/bindings/arm/gic.txt
	push @cells, @$flags[0];
	push @cells, $irq;
	push @cells, @$flags[1];

	return @cells;
}

sub get_moca_hwrev($)
{
	my $bchp_defines = shift;
	my $chip_id = sprintf("%x", get_chip_family_id($bchp_defines));
	my $chip_rev = sprintf("%x", get_chip_family_rev($bchp_defines));
	my %chip_tbl = (
		"7425" => 0x2001,
		"7429" => 0x2002,
		"7435" => 0x2002,
	);

	return 0x2003
		if (!defined($chip_tbl{$chip_id}));

	return $chip_tbl{$chip_id};
}

sub validate_mips_interrupt_cells($)
{
}

sub format_mips_interrupt_cells($$)
{
	my ($flags, $irq) = @_;
	my @cells;

	push @cells, $irq;

	return @cells;
}

sub get_arch($)
{
	my $bchp_defines = shift;
	return defined($bchp_defines->{BCHP_BOOTSRAM_TM_REG_START}) ? "ARM" : "MIPS";
}

1;
