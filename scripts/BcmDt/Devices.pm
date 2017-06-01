################################################################################
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
# 
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
################################################################################
package BcmDt::Devices;
use strict;
use warnings FATAL=>q(all);
no if $] >= 5.017011, warnings => 'experimental::smartmatch';
use Data::Dumper;
use File::Basename;
use BcmUtils;
use Carp;
our $Debug = 0;

my $P = basename $::0;
my %syscons = ();

# With DT structures, using the Dumper utility will print out
# infinite output because each node has a 'parent' field.  The
# assignment below skips the printing/evaluation of the 'parent'.
$Data::Dumper::Sortkeys = sub
{
	my $x = shift;
	my @a = grep { ! /^parent$/ } keys %$x;
	return \@a;
};

# Used by board DTS generation code
my %labels = ();

# Finds the specified Level 1 interrupt name, but does not
# produce a fatal error if not found. Use this helper only if
# the interrupt to lookup has multiple matches
sub __find_l1_interrupt($$)
{
	my ($bchp_defines, $name) = @_;
	my ($i, $key, $fmt);

	my $N = BcmUtils::get_num_l1_intr_regs($bchp_defines);

	$fmt = 'BCHP_HIF_CPU_INTR1_INTR_W%d'
		. '_MASK_STATUS_%s_CPU_INTR_SHIFT';

	for ($i=0; $i<$N; $i++) {
		$key = sprintf($fmt, $i, $name);
		last if (exists $bchp_defines->{$key});
	}
	return undef if ($i >= $N);
	return $i*32 + $bchp_defines->{$key}
}

# Finds the specified Level 1 interrupt name, and does produce
# a fatal error if it could not be found. To be used when the
# peripheral has stable interrupt names (single match).
sub find_l1_interrupt($$)
{
	my ($bchp_defines, $name) = @_;
	my $intr = __find_l1_interrupt($bchp_defines, $name);

	die "could not find $name" if !defined($intr);
	return $intr;
}

# Finds the specified Level 2 interrupt name in the designated parent interrupt
# controller. Use this helper only if the interrupt to lookup has multiple
# matches.
sub __find_l2_interrupt($$$)
{
	my ($bchp_defines, $parent, $name) = @_;
	my ($key, $fmt);

	$fmt = 'BCHP_%s_CPU_STATUS_%s_INTR_SHIFT';
	$key = sprintf($fmt, $parent, $name);
	if (!exists($bchp_defines->{$key})) {
		$fmt = 'BCHP_%s_CPU_STATUS_%s_SHIFT';
		$key = sprintf($fmt, $parent, $name);
	}
	if (!exists($bchp_defines->{$key})) {
		# Some blocks (e.g., AVS_HOST_L2) have 3 sets of interrupts,
		# where 0=CPU, 1=PCI, and 2=WEBCPU
		$fmt = 'BCHP_%s_STATUS0_%s_SHIFT';
		$key = sprintf($fmt, $parent, $name);
	}

	return $bchp_defines->{$key};
}

sub find_l2_interrupt($$$)
{
	my ($bchp_defines, $parent, $name) = @_;
	my $intr = __find_l2_interrupt($bchp_defines, $parent, $name);
	die "could not find $name" if !defined($intr);
	return $intr;
}

sub __find_l2_irq0_interrupt($$$)
{
	my ($bchp_defines, $parent, $name) = @_;
	my ($key, $fmt);

	$fmt = 'BCHP_%s_IRQEN_%s_irqen_SHIFT';
	$key = sprintf($fmt, uc($parent), $name);

	return $bchp_defines->{$key};
}

sub find_l2_irq0_interrupt($$$)
{
	my ($bchp_defines, $parent, $name) = @_;
	my $intr = __find_l2_irq0_interrupt($bchp_defines, $parent, $name);
	die "could not find $name" if !defined($intr);
	return $intr;
}

sub bphysaddr($$)
{
	my ($rh, $offset) = @_;
	return $rh->{phys_offset} + $offset;
}

sub get_reg_range($$)
{
	my ($rh, $prefix) = @_;

	my $bchp_defines = $rh->{rh_defines};
	my $reg_start = $bchp_defines->{"${prefix}_REG_START"};
	my $reg_end = $bchp_defines->{"${prefix}_REG_END"};
	return undef if (!defined $reg_start || !defined $reg_end);
	my $reg_bank_size = $reg_end - $reg_start + 4;
	return (bphysaddr($rh, $reg_start), $reg_bank_size);
}

sub get_offset_from_base($$$)
{
	my ($rh, $prefix, $reg_name) = @_;

	my $bchp_defines = $rh->{rh_defines};
	my $reg_start = $bchp_defines->{"${prefix}_REG_START"};
	my $reg_end = $bchp_defines->{"${prefix}_REG_END"};
	return undef if (!defined $reg_start || !defined $reg_end);

	my $reg_offset = $bchp_defines->{$reg_name} - $reg_start;
	return $reg_offset;
}


# These two variables are For optimization over several calls
# to get_reg_names_from_regexp().  On the first invocation
# we reduce the search space for all subsequent invocations.
my @pre_filtered_names;
my $get_reg_names_from_regexp_first_invocation = 1;

sub get_reg_names_from_regexp($$)
{
	my ($rh, $re) = @_;
	my $bchp_defines = $rh->{rh_defines};

	if ($get_reg_names_from_regexp_first_invocation) {
		$get_reg_names_from_regexp_first_invocation = 0;
		@pre_filtered_names = 
			grep { !/(?:SHIFT|DEFAULT|MASK|ASSERT|DEASSERT|ENABLE|DISABLE)$/ }
			keys %${bchp_defines};
	}

	my @a = sort { $bchp_defines->{$a} <=> $bchp_defines->{$b} }
		grep { /$re/ } @pre_filtered_names;

	die ("Failed to find in RDB: \"" . $re . "\"") if !@a;
	#print Dumper \@a;
	#printf "a[0] is %s => %x\n", $a[0], $bchp_defines->{$a[0]};
	return @a;
}

sub get_reg_range_from_regexp($$)
{
	my ($rh, $re) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my @a = get_reg_names_from_regexp($rh, $re);
	my $start  = $bchp_defines->{$a[0]};
	my $size = $bchp_defines->{$a[-1]} - $start + 4;
	return (bphysaddr($rh, $start), $size);
}

sub override_default($$)
{
	my ($h0, $h1) = @_;

	confess if !$h0 || !$h1;

	my @keys = keys %$h0;
	foreach (@keys) {
		if (defined $h1->{"-$_"}) {
			my $x = $h1->{"-$_"};
			if ("ARRAY" eq ref $x) {
				# preserve the type of the key, only override the value
				$h0->{$_}->[1] = $x->[0];
			} else {
				$h0->{$_} = $x;
			}
		}
	}
}

sub output_default($)
{
	my $default = shift;
	my $t = '';
	my %formatters = (
		# each formatter must be an array of three.
		"hex" => [ '<', '0x%x', '>' ],
		"dec" => [ '<', '%d', '>' ],
		"phandle" => [ '<', '&%s', '>' ],
		"string" => [ '"', '%s', '"' ],
		"bool" => [ '', '', '',],
	);

	my @keys = sort keys %$default;
	foreach (@keys) {
		my $x = $default->{"$_"};
		my $fmt;
		my $simple_string = "ARRAY" ne ref $x;
		# Booleans are a special case, handle them here.
		if (!$simple_string && $x->[0] eq 'bool') {
		    $t .= "$_;\n" if $x->[1];
		    next;
		}
		$t .= sprintf("%s = ", $_);
		if ($simple_string) {
			$fmt = join('', @{$formatters{"string"}});
			$t .= sprintf($fmt, $x);
		} else {
			$t .= $formatters{"$x->[0]"}[0];
			if ("ARRAY" eq ref $x->[1]) {
				my ($join, $i);
				if ($x->[0] eq "string") {
					$join = "\", \"";
					$t .= sprintf($formatters{"$x->[0]"}[1],
						join($join, @{$x->[1]}));
				} else {
					for ($i = 0; $i < scalar @{$x->[1]}; $i++) {
						$t .= sprintf($formatters{"$x->[0]"}[1] . " ", $x->[1][$i]);
					}
				}
			} else {
				$x->[1] = hex($x->[1])
					if (($x->[0] eq "hex") and ($x->[1] =~ m/^\s*0x/));

				$t .= sprintf($formatters{"$x->[0]"}[1], $x->[1]);
			}
			$t .= $formatters{"$x->[0]"}[2];
		}
		$t .= ";\n";
	}

	return $t;
}

sub compat_with($$$)
{
	my ($compatible, $key, $info) = @_;

	return ('string', [$compatible])
		if (!defined $info->{"-compat_with"});

	my @compat = @{$info->{"-compat_with"}};
	my @clist = ();

	while (@compat) {
		my @entry = split("=", shift @compat);
		push @clist, $entry[1]
			if ($key =~ /$entry[0]/);
	}
	return ('string', [$compatible, @clist])
}

sub get_reg_valid_mask($$)
{
	my ($bchp_defines, $regname) = @_;
	my $mask = 0xffffffff;
	my $i;
	my $fmt = "${regname}_reserved%d_MASK";

	for ($i=0; $i<32; $i++) {
		my $key = sprintf($fmt, $i);
		if (exists $bchp_defines->{$key}) {
			$mask &= ~($bchp_defines->{$key});
		}
	}

	return $mask;
}

sub get_gisb_valid_mask($)
{
	return get_reg_valid_mask($_[0], 'BCHP_SUN_GISB_ARB_ERR_CAP_MASTER');
}

sub get_gisb_master_names($)
{
	my $bchp_defines = shift;
	my $regex = '(?=^((?!reserved).)*$)BCHP_SUN_GISB_ARB_ERR_CAP_MASTER_[A-Za-z_0-9]*_SHIFT';
	my $masters = BcmUtils::get_rdb_fields($regex, $bchp_defines);
	my $t = '';

	# Sort by _SHIFT value since this is part of the DT binding
	@{$masters} = sort { $bchp_defines->{$a} <=> $bchp_defines->{$b} } @{$masters};

	foreach my $name (@{$masters}) {
		$name =~ s/BCHP_SUN_GISB_ARB_ERR_CAP_MASTER_//;
		$name =~ s/_SHIFT//;
		my $last = ${$masters}[-1];
		$t .= sprintf("\"%s\"%s", $name, $name eq $last ? "" : ", ");
	}

	return $t;
}

sub get_memc_client_masks($)
{
	my $memc_client_info = shift;
	my $last = scalar(@$memc_client_info);
	my $t = '';
	my $idx = 0;
	my $bit_idx = 0;
	my $curr_mask = 0;

	foreach my $name (@$memc_client_info) {
		$idx++;
		if (defined($name)) {
			$curr_mask |= 1 << $bit_idx;
		}
		$bit_idx++;

		# Just finished a bitmask, output it and restart
		if (($idx % 32) == 0) {
			$t .= sprintf("<0x%08x>%s", $curr_mask, $idx == $last ? "" : ", ");
			$curr_mask = 0;
			$bit_idx = 0;
		}
	}

	return $t;
}

sub get_memc_client_names($)
{
	my $memc_client_info = shift;
	my $last = scalar(@$memc_client_info);
	my $t = '';
	my $idx = 0;

	foreach my $name (@$memc_client_info) {
		$idx++;
		next if !defined($name);
		$t .= sprintf("\"%s\"%s", lc($name), $idx == $last ? "" : ", ");
	}

	return $t;
}

sub add_reference_alias($$$)
{
	my ($dt, $alias_name, $label) = @_;
	add_reference_rootnode($dt, $alias_name, $label, "aliases");
}

sub append_rdb_syscon_ref_phandle($$)
{
	my ($node, $label) = @_;
	$syscons{ $node } .= ", " if defined $syscons{ $node };
	$syscons{ $node } .= "<&" . $label . ">";
}

sub final_write_all_rdb_syscon_references($)
{
	my ($dt) = @_;
	my $t = "";
	for my $k ( sort keys %syscons) {

		$t = sprintf("%s {\n", $k);
		$t .= sprintf("\t%s = %s;\n", "syscon-refs", $syscons{ $k });
		$t .= "};\n";

		my $root = $dt->get_root();
		$root->add_node(DevTree::node->new($t));
	}
}

sub add_reference_rootnode($$$$)
{
	my ($dt, $name, $label, $node) = @_;
	my $t;
	my $root;

	$t = sprintf("%s {\n", $node);
	$t .= sprintf("%s = &%s;\n", $name, $label);
	$t .= "};\n";

	# The '$name' nodes should be put into the root of the tree
	$root = $dt->get_root();
	$root->add_node(DevTree::node->new($t));
}

sub add_fullpath_alias($$$)
{
	my ($dt, $alias_name, $full_path) = @_;
	my $t;

	$t = "aliases {\n";
	$t .= sprintf("%s = \"%s\";\n", $alias_name, $full_path);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub insert_label($$)
{
	my ($label, $unit) = @_;

	$labels{$label} = $unit;
}

sub get_label_by_path($)
{
	my $path = shift;
	my %reverse_labels = reverse %labels;

	return "NULL" if !defined($path);
	return $reverse_labels{$path};
}

sub get_path_by_label($)
{
	my $label = shift;

	return $labels{ $label };
}

# Add a generic interrupt property, looked up using the specified
# name and parent interrupt controller, the caller is responsible for
# filling in information about the interrupt: name, IRQ line #, parent_intc
# and additional flags if present
#
# Note: caller should ensure that L1 interrupts are listed *first* in the
# array, otherwise the logic would become too complicated (sorting is
# non-trivial)
sub output_interrupt_prop($$)
{
	my ($rh, $int) = @_;
	my $t = '';
	my $int_t = '';
	my $ext_t = '';
	my $name_t = '';
	my $elems = 0;
	my @intspec;
	my $n_intrs;
	my $int_parent = undef;
	my $last_parent = undef;
	my $int_name_valid = undef;
	my $mix_int_types = 0;
	my $last_non_mixed_index = 0;

	$int_t = "interrupts = ";
	$name_t = "interrupt-names = ";
	$ext_t = "interrupts-extended = ";

	if (ref $int eq "ARRAY") {
		@intspec = @{$int};
	} else {
		push @intspec, $int;
	}

	$n_intrs = scalar(@intspec);

	# First pass to determine whether we will see a mix and match of interrupts:
	# more than one interrupt at least comes from a different kind of interrupt
	# controller parent
	foreach my $i (0..$n_intrs-1) {
		my $ent = $intspec[$i];
		my $parent_intc = $ent->{"parent_intc"};

		if (!defined($parent_intc)) {
			$parent_intc = $rh->{l1_intc_props}->{name};
		}

		# We start with last_parent undefined, define them just to get
		# pass the first test
		if (!defined($last_parent)) {
			$last_parent = $parent_intc;
		}

		if ($last_parent ne $parent_intc) {
			$last_non_mixed_index = $i - 1;
			$mix_int_types = 1;
			last;
		}

		$last_parent = $parent_intc;
		$last_non_mixed_index = $i;
	}

	# Now let's get this done
	foreach my $i (0..$n_intrs-1) {
		my $ent = $intspec[$i];
		my $name = $ent->{"name"};
		my $parent_intc = $ent->{"parent_intc"};
		my $irq = $ent->{"irq"};
		my @flags;
		my $n_flags;
		# Default to a single cell: most common type
		my $n_cells = 1;
		my @cells;
		my $sep = $i == ($n_intrs - 1) ? ";\n" : ",\n";
		my $dbg_str;
		my $v_func = $ent->{"chk_func"};
		my $f_func = $ent->{"fmt_func"};

		die "Needs an IRQ# at least" if !defined($irq);

		# No parent interrupt means we are implicitly using the L1
		# interrupt controller, assign the properties we need to do our
		# job
		if (!defined($parent_intc)) {
			$parent_intc = $rh->{l1_intc_props}->{name};
			$n_cells = $rh->{l1_intc_props}->{num_cells};
			$v_func = $rh->{l1_intc_props}->{chk_func};
			$f_func = $rh->{l1_intc_props}->{fmt_func};
		}

		die "Unsupported number of cells: $n_cells"
			if $n_cells != 3 and $n_cells != 1;

		$parent_intc = lc($parent_intc);

		# Define the array of flags, or provide a default one with the
		# specified number of cells - 1, 0 is the default for everything
		# right now, later on, we could provide defaults from l1_intc_props
		# Ignore flags on MIPS
		if (defined($ent->{"flags"}) && ($rh->{chip_arch} ne "MIPS") ) {
			@flags = @{$ent->{"flags"}};
		} else {
			for (my $j = 0; $j < $n_cells - 1; $j++) {
				push @flags, 0;
			}
		}

		# The parent interrupt controller is different from the L1, flag that
		if ((defined($parent_intc) and $parent_intc) ne $rh->{l1_intc_props}->{name}) {
			$int_parent = $parent_intc;
		}

		# Verify number of flags against cell count - 1 (interrupt is always
		# present)
		$n_flags = scalar(@flags);
		die "Invalid number of flags: $n_flags for $n_cells"
			if $n_flags != ($n_cells - 1);

		# Invoke the validation function if it exists
		if (defined($v_func)) {
			$v_func->(@flags);
		}

		# When using 3 interrupt cells, we currently assume that this
		# is the layout of an ARM GIC interrupt controller as defined
		# in Documentation/devicetree/bindings/arm/gic.txt in the Linux
		# kernel sources
		#
		# For a single cell, we just encode the interrupt number there,
		# which is the most common scheme
		if (defined($f_func)) {
			@cells = $f_func->(\@flags, $irq);
		} else {
			push @cells, $irq;
		}

		# Now proceed to format the interrupt cells based on how we
		# did format them before
		my $int_t_fmt = "<";
		my $ext_t_fmt = "<&$parent_intc ";
		for (my $j = 0; $j < $n_cells; $j++) {
			my $end = $j == $n_cells - 1 ? ">" : " ";
			$int_t_fmt .= sprintf("0x%x%s", $cells[$j], $end);
			$ext_t_fmt .= sprintf("0x%x%s", $cells[$j], $end);
		}

		# Conditionally format the 'interrupts' property as long as we
		# find interrupts from the same controller, if not, stop there
		#
		# Always generate the 'interrupts-extended' property since we
		# can always keep adding mixed interrupt sources
		if ($mix_int_types == 0 ||
		    $i <= $last_non_mixed_index && $mix_int_types == 1) {
				$int_t .= $int_t_fmt;
		}
		$ext_t .= $ext_t_fmt;

		# Keep adding the separator, or terminate earlier for mixed
		# interrupts
		if ($mix_int_types == 0 ||
		    $i < $last_non_mixed_index && $mix_int_types == 1) {
			$int_t .= $sep;
		} elsif ($i == $last_non_mixed_index && $mix_int_types == 1) {
			$int_t .= ";\n";
		}

		# Add the name of interrupt(s)
		if (defined($name)) {
			$int_name_valid = $name;
			$name_t .= sprintf("\"%s\"", lc($name)) . $sep;
		}
		$ext_t .= $sep;

		$dbg_str .= "[$i] Interrupt properties:\n";
		if (defined($name)) {
			$dbg_str .= "\tName: $name\n";
		}
		$dbg_str .= "\tParent: $parent_intc\n" .
			    "\tRequired cells: $n_cells\n" .
			    "\tIRQ: $irq\n";
		if ($n_flags != 0) {
			$dbg_str .= "\tFlags: $flags[0] - $flags[1]\n";
		}

		print $dbg_str if $Debug != 0;
	}

	$t .= $int_t;

	# Parent interrupt specified is different from the "L1" aka main
	# interrupt controller, indicate that with an 'interrupt-parent'
	# property
	if ($last_parent ne $rh->{l1_intc_props}->{"name"}
	    or $mix_int_types == 1) {
		$t .= "interrupt-parent = <&$last_parent>;\n";
	}

	# Interrupts are not coming from the same controllers, e.g:
	# multiple L1s and one L2 (for wake-up), which means we need an
	# interrupt-extended property as well
	if ($mix_int_types == 1) {
		$t .= $ext_t;
	}

	# Interrupt name is valid, let's output it as well
	if (defined($int_name_valid)) {
		$t .= $name_t;
	}

	return $t;
}

# This variable will be set/modified by start_uart_body()
# and then later on it maybe used to insert the
# "clock-frequency" property into the nodes by func
# insert_clk_frequency_into_uarts().
my @serial_names;

sub start_uart_body($$$$$$)
{
	my ($rh, $info, $label, $addr, $regs_ref, $intr_ref) = @_;

	my @regs = @$regs_ref;
	my @intrs = @$intr_ref;
	my %default = ("compatible" => "ns16550a",
		       "reg-shift" => [ "hex", 2 ],
		       "reg-io-width" => [ "hex", 4 ],
		      );
	override_default(\%default, $info);

	my $reg_count = scalar(@regs);

	# If there's more than 1 register defined, it means this UART
	# has the additional DMA hardware.
	if ($reg_count > 1) {
	    $default{compatible} =
		sprintf("brcm,brcm7271-uart\", \"%s", $default{compatible});
	}
	my $t = sprintf("%s: serial@%x {\n", $label, $addr);
	insert_label($label, sprintf("serial@%x", $addr));
	push @serial_names, sprintf("serial@%x", $addr);
	$t .= output_default(\%default);
	my $reg_addrs;
	my $reg_names;
	my $i;
	for ($i=0; $i < $reg_count; $i++) {
	    if ($i != 0) {
		$reg_addrs .= " ";
		$reg_names .= "\", \"";
	    }
	    $reg_addrs .= sprintf("0x%x 0x%x", $regs[$i][0], $regs[$i][1]);
	    $reg_names .= $regs[$i][2];
	}
	$t .= sprintf("reg = <%s>;\n", $reg_addrs);
	$t .= sprintf("reg-names = \"%s\";\n", $reg_names);
	$t .= output_interrupt_prop($rh, \@intrs);
	$t .= "fifo-size = <0x20>;\n";
	$t .= "auto-flow-control;\n";
	return $t;
}


sub add_serial($$$$$)
{
	my ($dt, $rh, $n, $info, $default_baud) = @_;
	my @reg_names = (
	    ["uart", ""],
	    ["dma_arb", "_DMA_ARB"],
	    ["dma_rx", "_DMA_RX"],
	    ["dma_tx", "_DMA_TX"],
	    ["dma_intr2", "_DMA_INTR2"]);
	my @stdout = @{$info->{'-stdout'}} if (defined($info->{'-stdout'}));
	die "Error: incorrect number of args for '-stdout'\n\tin $info->{__cfg_where__}\n"
		if (scalar(@stdout) > 1);
	my ($i, $j, $k);
	my $bchp_defines = $rh->{rh_defines};
	my @enable_dma = @{$info->{'-dma'}} if $info->{'-dma'};

	for ($i=0; $i<$n; $i++) {
		my @regs;
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my $letter = chr(65 + $i);
		my $lc_letter = lc($letter);
		my $reg_count = ($i ~~ @enable_dma) ? scalar(@reg_names) : 1;

		for ($k = 0; $k < $reg_count; $k++) {
		    my ($rstart, $rsize)
			= get_reg_range($rh,
					"BCHP_UART${letter}" . $reg_names[$k][1]);
		    if (!$rstart) {
			last;
		    }
		    ($regs[$k][0], $regs[$k][1], $regs[$k][2]) =
			($rstart, $rsize, $reg_names[$k][0]);
		}
		my @intr_prop;
		my $intr_name = "UART";
		my $intr = find_l1_interrupt($bchp_defines,
					     sprintf("UPG_UART%d", $i));
		push @intr_prop, { "irq" => $intr, "name" => $intr_name,
				   "flags" => [ 0, 4 ]  };
		# If additional DMA registers were specified, add DMA interrupt
		if (scalar(@regs) > 1) {
		    $intr_name = "DMA";
		    $intr = find_l1_interrupt($bchp_defines,
					      sprintf("UPG_UART_DMA%d", $i));
		    push @intr_prop, { "irq" => $intr, "name" => $intr_name,
				       "flags" => [ 0, 4 ] };
		}
		my $uart_x = sprintf("uart%s", $lc_letter);
		my $is_stdout = (@stdout && lc($stdout[0]) eq $lc_letter);

		my $t = start_uart_body($rh, $info, $uart_x, $regs[0][0],
			\@regs, \@intr_prop);
		# Default stdout console
		if ($is_stdout) {
			$t .= sprintf("current-speed = <%d>;\n", $default_baud);
		}
		$t .= "};\n";

		my $uart_node = DevTree::node->new($t);
		$dt->add_node($uart_node);

		# Default stdout console
		if ($is_stdout) {
			# ePAPR method to specify boot console
			$t = "chosen {\n";
			# Can't use path reference DT syntax (e.g., &uarta)
			# because we need to concatenate ":<UART_OPTS>"
			$t .= sprintf("stdout-path = \"%s:%d\";\n",
				      $uart_node->get_path(), $default_baud);
			$t .= "};\n";
			$dt->get_root()->add_node(DevTree::node->new($t));
		}

		add_reference_alias($dt, sprintf("serial%d", $i), $uart_x);
	}
}

sub add_named_serial($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my $name = $info->{'-name'}[0];
	my $alias = $info->{'-alias'}[0];
	my $intr_name = $info->{'-l1intr'}[0];
	my @regs;

	($regs[0][0], $regs[0][1]) = get_reg_range($rh, "BCHP_" . $name);
	$regs[0][2] = "uart";

	my $intr = find_l1_interrupt($bchp_defines, $intr_name);
	die "$P: cannot find  " . $intr_name . "interrupt"
		if !defined $intr;

	my @intr_prop;
	push @intr_prop, { "irq" => $intr, "name" => $intr_name,
			   "flags" => [ 0, 4 ]  };


	my $t = start_uart_body($rh, $info, lc($name), $regs[0][0] || 0,
			\@regs, \@intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
	add_reference_alias($dt, $alias, lc($name));
}


sub add_bsc($$$$)
{
	my ($dt, $rh, $n, $info) = @_;

	my $bchp_defines = $rh->{rh_defines};

	my %default = ("compatible" => "brcm,brcmstb-i2c",
		        '#interrupt-cells' => [ "dec", 1 ],
			"#address-cells" => [ "dec", 1 ],
			"#size-cells" => [ "dec", 0 ],
		      );

	my @clk_frequency = (390000) x $n;
	@clk_frequency = @{$info->{"-clock-frequency"}}
		if ($info->{"-clock-frequency"});
	my $num_chosen = $n;
	if ($info->{'-choose'}) {
		$num_chosen = 0;
		foreach (@{$info->{'-choose'}}) {
			$num_chosen++ if $_;
		}
	}
	die "Error: incorrect number of args for '-clock-frequency'\n\tin $info->{__cfg_where__}\n"
		if ($num_chosen != @clk_frequency && $info->{'-clock-frequency'});


	my $i;
	my $num_found = 0;
	for ($i=0; $i<26; $i++) { #  26 as the number of alphabets
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my $letter = chr(65 + $i);
		my $lc_letter = lc($letter);
		my $parent_phandle = "irq0_intc";
		my ($reg_first, $bank_size)
			= get_reg_range($rh, "BCHP_BSC${letter}");
		my $intr_name = sprintf('UPG_BSC%s', $letter);
		my $intr = __find_l2_irq0_interrupt($bchp_defines, "IRQ0", "iic" . lc(${letter}));
		if (!defined($intr)) {
		  $intr = __find_l2_irq0_interrupt($bchp_defines, "IRQ0_AON", "iic" . lc(${letter}));
		  $intr_name = sprintf("UPG_BSC%s_AON", $letter);
		  $parent_phandle = "irq0_aon_intc";
		}
		if (!defined($intr)) {
		  $intr = __find_l2_interrupt($bchp_defines, "UPG_BSC_IRQ", "iic" . lc(${letter}));
		  $intr_name = sprintf("UPG_BSC%s", $letter);
		  $parent_phandle = "upg_bsc_irq_intc";
		}
		if (!defined($intr)) {
		  $intr = __find_l2_interrupt($bchp_defines, "UPG_BSC_AON_IRQ", "iic" . lc(${letter}));
		  $intr_name = sprintf("UPG_BSC%s_AON", $letter);
		  $parent_phandle = "upg_bsc_aon_irq_intc";
		}
		next if (!defined $intr);
		$num_found++;

		my $label = sprintf("bsc%s", $lc_letter);
		my $unit = sprintf("i2c@%x", $reg_first || 0);
		my $t = sprintf("%s: %s {\n", $label, $unit);
		insert_label($label, $unit);
		my %intr_prop = (
			"irq" => $intr,
			"name" => $intr_name,
			"parent_intc" => $parent_phandle,
		);
		$t .= output_default(\%default);
		$t .= sprintf("clock-frequency = <%s>;\n", shift(@clk_frequency));
		$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg_first, $bank_size);
		$t .= output_interrupt_prop($rh, \%intr_prop);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));

		add_reference_alias($dt, sprintf("i2c-%d", $i), sprintf("bsc%s", $lc_letter));

		last if ($num_found >= $n);
	}
	die "$P: found $num_found BSC level-2 interrupts while $n is expected"
		if (!$info->{'-choose'} && $num_found != $n);
}


sub add_usb_v2($$$$$)
{
	my ($dt, $rh, $n, $info, $l2_intr) = @_;
	my @usb_info;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);

	my %default = (compatible => "brcm,usb-phy",
		       "#address-cells" => [ "dec", 1 ],
		       "#size-cells" => [ "dec", 1 ],
		       ipp => [ "dec", 1 ],
		       ioc => [ "dec", 1 ],
		      );

	override_default(\%default, $info);

	my ($i, $j);

	for ($i=0; $i<$n; $i++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my $pre = sprintf("BCHP_USB%s", $i ? $i : "");
		my ($usb_start, $usb_size) 
			= get_reg_range($rh, "${pre}_CTRL");
		my ($xhci_ec_start, $xhci_ec_size) 
			= get_reg_range($rh, "${pre}_XHCI_EC");

		my $t = '';
		my $usb_label = sprintf("usbphy_%d", $i);
		my $usb_unit = sprintf("usb-phy\@%x", $usb_start);
		$t .= sprintf("%s: %s {\n", $usb_label, $usb_unit);
		insert_label($usb_label, $usb_unit);
		$t .= sprintf("%s = <0x%x 0x%x>", 'reg', $usb_start, $usb_size);
		$t .= defined($xhci_ec_start)
			? sprintf(", <0x%x 0x%x>;\n", $xhci_ec_start, $xhci_ec_size)
			: ";\n";
		$t .= output_default(\%default);
		$t .= "#phy-cells = <1>;\n";
		$t .= sprintf("%s;\n", 'ranges');
		my ($xhci_start, $xhci_size) 
			= get_reg_range($rh, "${pre}_XHCI");
		if ($xhci_start) {
		    my ($ehci_start, $ehci_size)
			= get_reg_range($rh, "${pre}_EHCI");
		    if ($ehci_start) {
			$t .= "has_xhci;\n";
		    } else {
			$t .= "has_xhci_only;\n";
		    }
		}
		$t .= "};\n";
		if ($dt) {
			$dt->add_node(DevTree::node->new($t));
			add_reference_alias($dt, sprintf("usbphy%d", $i), $usb_label);
		}

		my $usb_info = { top => sprintf('%x', $usb_start), xhci => [], ohci => [],
				 ehci => [], bdc => [] };
		foreach my $type ('ehci', 'ohci', 'xhci', 'bdc') {
			for ($j=0; 1; $j++) {
				$t = '';
				my $mid = sprintf("%s%s", uc($type), $j ? $j : "");
				my ($start, $size) 
					= get_reg_range($rh, "${pre}_${mid}");
				last if (!defined $start);
				my $intr_name;
				if ($type eq 'bdc') {
					$intr_name = sprintf("USB%d_USBD", $i);
				} else {
					$intr_name = sprintf("USB%d_%s_%d", $i, uc($type), $j);
				}
				my $intr = __find_l1_interrupt($bchp_defines, $intr_name);
				die "cannot find usb$j $type interrupt"
					if !defined $intr;
				push @{$usb_info->{$type}}, sprintf('%x', $start);
				my $compatible;
				if ($type eq 'bdc') {
					$compatible = "brcm,bdc-udc-v2";
				} elsif ($rh->{chip_arch} eq "MIPS") {
					$compatible = sprintf("brcm,bcm%x-%s", $chipid, $type);
				} else {
					$compatible = "brcm,$type-brcm-v2\", \"generic-$type";
				}

				my %intr_prop = (
					"name" => $intr_name,
					"irq" => $intr,
				);
				my $node_label = sprintf("%s_%d_%d", $type, $i, $j);
				$t .= sprintf("%s: $type\_v2\@%x {\n", $node_label, $start);
				$t .= sprintf("status = \"disabled\";\n")
					if $type eq 'bdc';
				$t .= sprintf("%s = \"%s\";\n", 'compatible', $compatible);
				$t .= sprintf("%s = <0x%x 0x%x>;\n", 'reg', $start, $size);
				$t .= output_interrupt_prop($rh, \%intr_prop);
				$t .= sprintf("phys = <&usbphy_%d 0x%d>;\n", $i, $type eq 'xhci' ? 1 : 0);
				$t .= "phy-names = \"usbphy\";";
				$t .= "};\n";
				if ($dt) {
					$dt->add_node(DevTree::node->new($t));
					add_reference_alias($dt, sprintf("%s%d\_%d", $type, $i, $j), $node_label);
				}
			}
		}
		push @usb_info, $usb_info;
	}
	return \@usb_info;
}

sub add_usb($$$$$)
{
	my ($dt, $rh, $n, $info, $l2_intr) = @_;
	my @usb_info;
	my $bchp_defines = $rh->{rh_defines};

	my %default = (compatible => "brcm,usb-instance",
		       "#address-cells" => [ "dec", 1 ],
		       "#size-cells" => [ "dec", 1 ],
		       ipp => [ "dec", 1 ],
		       ioc => [ "dec", 1 ],
		      );

	override_default(\%default, $info);

	my ($i, $j);

	for ($i=0; $i<$n; $i++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my $pre = sprintf("BCHP_USB%s", $i ? $i : "");
		my ($usb_start, $usb_size) 
			= get_reg_range($rh, "${pre}_CTRL");
		my ($xhci_ec_start, $xhci_ec_size) 
			= get_reg_range($rh, "${pre}_XHCI_EC");

		my $t = '';
		my $usb_label = sprintf("usb_%d", $i);
		my $usb_unit = sprintf("usb\@%x", $usb_start);
		$t .= sprintf("%s: %s {\n", $usb_label, $usb_unit);
		insert_label($usb_label, $usb_unit);
		$t .= sprintf("%s = <0x%x 0x%x>", 'reg', $usb_start, $usb_size);
		$t .= defined($xhci_ec_start)
			? sprintf(", <0x%x 0x%x>;\n", $xhci_ec_start, $xhci_ec_size)
			: ";\n";
		$t .= output_default(\%default);
		$t .= sprintf("%s;\n", 'ranges');
		my $intr = __find_l2_interrupt($bchp_defines, uc($l2_intr), "USB" . $i);
		if (defined($intr)) {
			$t .= sprintf("interrupts-extended = <&%s 0x%x>;\n",
				$l2_intr . "_intc", $intr);
		}
				
		my $usb_info = { top => sprintf('%x', $usb_start), xhci => [], ohci => [],
				 ehci => [], bdc => [] };

		foreach my $type ('ehci', 'ohci', 'xhci', 'bdc') {
			for ($j=0; 1; $j++) {
				my $mid = sprintf("%s%s", uc($type), $j ? $j : "");
				my ($start, $size) 
					= get_reg_range($rh, "${pre}_${mid}");
				last if (!defined $start);
				my $intr_name;
				if ($type eq 'bdc') {
					$intr_name = sprintf("USB%d_USBD", $i);
				} else {
					$intr_name = sprintf("USB%d_%s_%d", $i, uc($type), $j);
				}
				my $intr = __find_l1_interrupt($bchp_defines, $intr_name);
				die "cannot find usb$j $type interrupt"
					if !defined $intr;
				push @{$usb_info->{$type}}, sprintf('%x', $start);
				my $compatible;
				if ($type eq 'bdc') {
					$compatible = "brcm,bdc-udc-v0.16\", \"brcm,bdc-udc";
				} elsif ($type eq 'xhci') {
					$compatible = 'xhci-platform';
				} else {
					$compatible = "brcm,$type-brcm";
				}

				my %intr_prop = (
					"name" => $intr_name,
					"irq" => $intr,
				);

				$t .= sprintf("$type\@%x {\n", $start);
				$t .= sprintf("status = \"disabled\";\n")
					if $type eq 'bdc';
				$t .= sprintf("%s = \"%s\";\n", 'compatible', $compatible);
				$t .= sprintf("%s = <0x%x 0x%x>;\n", 'reg', $start, $size);
				$t .= output_interrupt_prop($rh, \%intr_prop);
				$t .= "};\n";
			}
		}
		$t .= "};\n";

		if ($dt) {
			$dt->add_node(DevTree::node->new($t));
			add_reference_alias($dt, sprintf("usb%d", $i), $usb_label);
		}
		push @usb_info, $usb_info;
	}
	return \@usb_info;
}

sub add_mdio($$$)
{
	my ($rh, $i, $compatible) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $unimac_start = $bchp_defines->{"BCHP_GENET_${i}_UMAC_REG_START"} -
		$bchp_defines->{"BCHP_GENET_${i}_SYS_REG_START"};
	my $mdio_start = $unimac_start + 0x614;
	my $mdio_end = $mdio_start + 8;
	my $mdio_size = $mdio_end - $mdio_start;
	$compatible =~ s/genet/genet-mdio/;
	my %mdio_default =
		("compatible" => $compatible,
		 "#address-cells" => [ "dec", 1 ],
		 "#size-cells" => [ "dec", 0 ],
	       );
	my $t = '';

	my $label = sprintf("mdio%d", $i);
	my $unit = sprintf("mdio\@%x", $mdio_start);
	$t .= sprintf("%s: %s {\n", $label, $unit);
	insert_label($label, $unit);
	$t .= output_default(\%mdio_default);
	$t .= sprintf("%s = <0x%x 0x%x>;\n", 'reg', $mdio_start, $mdio_size);
	$t .= "};\n";
}

sub add_genet($$$$$)
{
	my ($dt, $rh, $n, $info, $l2_intr) = @_;

	my ($i, $j, $str, $intr);
	my $bchp_defines = $rh->{rh_defines};

	my %default = ("compatible" => "brcm,genet-v4",
		       "#address-cells" => [ "dec", 1 ],
		       "#size-cells" => [ "dec", 1 ],
		       "device_type" => "network",
		      );


	override_default(\%default, $info);
	for ($i=0; $i<$n; $i++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my ($beg, $size) 
			= get_reg_range_from_regexp($rh, "^BCHP_GENET_${i}_");
		my @intr_prop;
		foreach $j (qw/A B C D E F G/) {
			my $intr_name = sprintf('GENET_%d_%s', $i, $j);
			$intr = __find_l1_interrupt($bchp_defines, $intr_name);
			last if !defined $intr;
			push @intr_prop, { "irq" => $intr, "name" => lc($intr_name) };
		}

		my $nintrs = scalar @intr_prop;
		die "could not find GENET interrupts" if !$nintrs;

		my $t = '';
		my $ext_t = '';
		my $int_t = '';
		my $intr_name = '';
		my $label = sprintf("eth%d", $i);
		my $unit = sprintf("ethernet\@%x", $beg);
		$t .= sprintf("%s: %s {\n", $label, $unit);
		insert_label($label, $unit);
		$t .= output_default(\%default);
		$t .= sprintf("%s = <0x%x 0x%x>;\n", 'reg', $beg, $size);
		if (@intr_prop) {
			if ($rh->{chip_arch} eq "MIPS") {
				$intr_name = "WOL_ENET" if ($i == 0);
				$intr_name = "WOL_MOCA" if ($i == 1);
			} else {
				$intr_name = "WOL_GENET" . $i;
			}
			$intr = find_l2_interrupt($bchp_defines, uc($l2_intr), $intr_name);
			push @intr_prop, { "irq" => $intr,
					   "name" => "wol_genet",
					   "parent_intc" => $l2_intr . "_intc" };
		}
		$t .= output_interrupt_prop($rh, \@intr_prop);
		$t .= add_mdio($rh, $i, $default{"compatible"});

		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));

		add_reference_alias($dt, $label, $label);
	}
}


sub add_bolt($$)
{
	my ($dt, $rh) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = ();

	my $t = "bolt {};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub sata_get_phy_reg_range($$$)
{
	my ($rh, $sata_inst, $n_phy_ports) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my ($start, $end);

	# We could have (2) SATA controllers, each with (1) port. As a result,
	# the RDB naming convention for some SATA registers was changed.
	#    New: BCHP_SATAn_PORT0_PCB_REG_START, n = {0, 1}
	# Others: BCHP_PORTn_SATA3_PCB_REG_START, n = {0, 1}
	my @a = grep { /^BCHP_(PORT\d_SATA3|SATA${sata_inst}_PORT0)_PCB_REG_START/ }
		keys %${bchp_defines};
	my @b = grep { /^BCHP_(PORT\d_SATA3|SATA${sata_inst}_PORT0)_PCB_REG_END/ }
		keys %${bchp_defines};
	if (!@a or !@b) {
		# 40nm platforms did not have PCB registers exposed, but it should
		# be at 0x100 from the start of the block and its size is fffh
		$start = $bchp_defines->{"BCHP_SATA_GRB_REG_START"} + 0x100;
		$end = 0xFFF;
		return ($start, $end);
	}
	die if !@a or !@b;

	@a = sort @a;
	@b = sort @b;

	$start = bphysaddr($rh, $bchp_defines->{$a[0]});
	$end = $bchp_defines->{$b[$n_phy_ports - 1]} -
		$bchp_defines->{$a[0]};

	return ($start, $end);
}

sub add_sata($$$$)
{
	my ($dt, $rh, $n_sata, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my ($i, $j);

	my %default = (compatible => [ 'string', [
		       $rh->{chip_arch} eq "MIPS" ? 'brcm,bcm7425-ahci' : 'brcm,bcm7445-ahci',
		       'brcm,sata3-ahci']],
		       "#address-cells" => [ 'dec', 1 ],
		       "#size-cells" => [ 'dec', 0 ],

		       # deprecated, kept for legacy driver
		       'phy-enable-ssc-mask' => [ "hex", 0 ],

		       # deprecated, kept for legacy driver
		       'phy-generation' => [ "hex", 0x2800 ],

		       'reg-names' => [ 'string', [ "ahci", "top-ctrl" ]],
		      );
	override_default(\%default, $info);

	# pull out SSC mask, so we can apply it to each port node
	# TODO: rework the -phy-enable-ssc-mask config option eventually, to
	# make this work nicer?
	my $ssc_mask;
	if ($info->{"-phy-enable-ssc-mask"}) {
		$ssc_mask = int(hex($info->{"-phy-enable-ssc-mask"}->[0]));
	} else {
		$ssc_mask = 0;
	}

	my %default_phy = (compatible => [ 'string', [
			   $rh->{chip_arch} eq "MIPS" ? "brcm,bcm7425-sata-phy" : "brcm,bcm7445-sata-phy",
			   "brcm,phy-sata3"]],
			   "#address-cells" => [ 'dec', 1 ],
			   "#size-cells" => [ 'dec', 0 ],
			  );

	for ($i=0; $i<$n_sata; $i++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my $sata = sprintf("BCHP_SATA%s", ($n_sata>1) ? $i : "");
		my @a = get_reg_range_from_regexp($rh, "^${sata}_");
		my @b = get_reg_range($rh, "${sata}_AHCI_GHC");
		my @c = get_reg_range($rh, "${sata}_TOP_CTRL");
		my $n_phy_ports = BcmUtils::get_num_sata_phy($bchp_defines, $sata);
		my ($phy_reg_start, $phy_reg_size) =
			sata_get_phy_reg_range($rh, $i, $n_phy_ports) if $n_phy_ports;
		my $reg_start = $b[0];
		my $reg_size = $a[1] - ($b[0] - $a[0]);
		my $top_ctrl_reg_start = $c[0];
		my $top_ctrl_reg_size = $c[1];
		my ($intr, $intr_name);
		my @intr_matches = ( "SATA${i}_AHCI", "SATA_${i}_AHCI" );
		unshift @intr_matches, "SATA_AHCI" if $i == 0;
		my $n_intr_matches = scalar @intr_matches;
		foreach (0..$n_intr_matches - 1) {
			$intr_name = $intr_matches[$_];
			$intr = __find_l1_interrupt($bchp_defines, $intr_name);
			last if defined $intr;
		}
		die "could not find sata$i interrupt" if (!defined($intr));

		# add SATA node
		my $sata_label = sprintf("sata_%d", $i);
		my $t = '';
		my %intr_prop = (
			"name" => $intr_name,
			"irq" => $intr,
		);
		my $sata_unit = sprintf("sata\@%x", $reg_start);
		$t .= sprintf("%s: %s {\n", $sata_label, $sata_unit);
		insert_label($sata_label, $sata_unit);
		$t .= output_default(\%default);
		$t .= sprintf("reg = <0x%x 0x%x 0x%x 0x%x>;\n",
			$reg_start, $reg_size, $top_ctrl_reg_start,
			$top_ctrl_reg_size);
		$t .= output_interrupt_prop($rh, \%intr_prop);

		# deprecated, kept for legacy driver
		$t .= sprintf("phy-base-addr = <0x%x>;\n", $phy_reg_start);

		# deprecated, kept for legacy driver
		$t .= sprintf("top-ctrl-base-addr = <0x%x>;\n",
			$top_ctrl_reg_start);
		$t .= "};\n";
		my $sata_node = $dt->add_node(DevTree::node->new($t));

		# add SATA PHY node
		my $sata_phy_name = sprintf("sata_phy%d", $i);
		my $sata_phy_unit = sprintf("sata_phy@%x", $phy_reg_start);
		$t = '';
		$t .= sprintf("%s: %s {\n", $sata_phy_name, $sata_phy_unit);
		insert_label($sata_phy_name, $sata_phy_unit);
		$t .= output_default(\%default_phy);
		$t .= sprintf("reg = <0x%x 0x%x>;\n",
			$phy_reg_start, $phy_reg_size);
		$t .= "reg-names = \"phy\";\n";
		$t .= "};\n";
		my $phy_node = $dt->add_node(DevTree::node->new($t));

		for ($j = 0; $j < $n_phy_ports; $j++) {
			# add port sub-node(s) to PHY
			my $phy_port_label = sprintf("sata_port%d_%d", $i, $j);
			my $phy_port_unit = sprintf("sata-phy@%d", $j);
			$t = '';
			$t .= sprintf("%s: %s {\n", $phy_port_label, $phy_port_unit);
			insert_label($phy_port_label, $phy_port_unit);
			$t .= sprintf("reg = <%d>;\n", $j);
			$t .= "#phy-cells = <0>;\n";
			$t .= "brcm,enable-ssc;\n" if ($ssc_mask & (1 << $j));
			$t .= "};\n";
			$phy_node->add_node(DevTree::node->new($t));

			# add port sub-node(s) to SATA
			$t = '';
			$t .= sprintf("sata-port@%d {\n", $j);
			$t .= sprintf("reg = <%d>;\n", $j);
			$t .= sprintf("phys = <&%s>;\n", $phy_port_label);
			$t .= "};\n";
			$sata_node->add_node(DevTree::node->new($t));
		}

		add_reference_alias($dt, $sata_phy_name, $sata_phy_name);
		add_reference_alias($dt, sprintf("sata%d", $i), $sata_label);
	}
}


sub add_sdio($$$$)
{
	my ($dt, $rh, $n_sdio, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my ($i, $j);



	my %default = (compatible => "brcm,bcm7425-sdhci\", \"brcm,sdhci-brcmstb");
	override_default(\%default, $info);

	for ($i=0; $i<$n_sdio; $i++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my ($host_start, $host_size) = get_reg_range($rh, "BCHP_SDIO_${i}_HOST");
		my ($cfg_start, $cfg_size) = get_reg_range($rh, "BCHP_SDIO_${i}_CFG");
		my $intr = find_l1_interrupt($bchp_defines, "SDIO${i}_0");

		my $t = '';
		my $sdhci_label = sprintf("sdhci_%d", $i);
		my %intr_prop = (
			"irq" => $intr,
			"name" => "SDIO${i}_0",
		);

		my $sdhci_unit = sprintf("sdhci\@%x", $cfg_start);
		$t .= sprintf("%s: %s {\n", $sdhci_label, $sdhci_unit);
		insert_label($sdhci_label, $sdhci_unit);
		$t .= sprintf("compatible = \"%s\";\n", $default{compatible});
		$t .= sprintf("reg = <0x%x 0x%x 0x%x 0x%x>;\n", $host_start,
			$host_size, $cfg_start, $cfg_size);
		$t .= sprintf("reg-names = \"host\", \"cfg\";\n");
		$t .= output_interrupt_prop($rh, \%intr_prop);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));
		add_reference_alias($dt, sprintf("sdhci%d", $i), $sdhci_label);
	}
}


sub add_memory($$$$)
{
	my ($dt, $rh, $n, $info) = @_;

	return if (!$info || scalar(keys %$info) == 0);
	my %memory_default = ('#address-cells' => 1,
		'#size-cells' => 1, );
	my %cma_default = ('#address-cells' => 0,
		'#size-cells' => 0, );

	my $t = "memory {\n";
	$t .= sprintf("#address-cells = <%d>;\n", $memory_default{"#address-cells"});
	$t .= sprintf("#size-cells = <%d>;\n", $memory_default{"#size-cells"});
	$t .= "device_type = \"memory\";\n";
	$t .= "reg = <" . join(" ", @{$info->{"-reg"}}) . ">;\n";
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

# subdivide_pcie_range(...) -- split a range into equal parts.
# $which -- can be 0..$parts-1.
# $parts -- number of equal parts of the range.
sub subdivide_pcie_range
{
	my ($which, $parts, @range) = @_;
	die if @range % 7 || $which >= $parts;
	my $size = sprintf('0x%08x', int(hex($range[-1]) / $parts));
	my $offset = sprintf('0x%08x', hex($range[2]) + $which * hex($size));
	return (@range[0..1], $offset, $range[3], $offset, $range[5], $size);
}


sub add_pcie($$$$$)
{
	my ($dt, $rh, $n_pcie, $info, $chip) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);

	my %default = ("compatible" => [ 'string', [ sprintf("brcm,bcm%x-pcie", $chipid),
					'brcm,pci-plat-dev' ] ],
			"#address-cells" => [ 'dec', 3 ],
			"#size-cells"  => [ 'dec', 2 ],
			"tot-num-pcie" => [ 'dec', $n_pcie ],
			"#interrupt-cells" => [ 'dec', 1 ],
			"interrupt-map-mask" => [ 'hex', [ 0xf800, 0, 0, 7] ],
			);
	override_default(\%default, $info);

	for (my $i=0; $i<$n_pcie; $i++) {
		my $disabled = ($info->{'-choose'} && !$info->{'-choose'}->[$i]);
		my ($beg, $size);
		if ($n_pcie > 1) {
			($beg, $size) = get_reg_range_from_regexp($rh, "^BCHP_PCIE_${i}");
		} else {
			($beg, $size) = get_reg_range_from_regexp($rh, "^BCHP_PCIE_(?!DMA)");
		}
		return if (!defined($beg));

		# MIPS: adjust by physical offset since we are out of the RDB node
		if ($rh->{phys_offset} eq 0) {
			$beg += $bchp_defines->{BCHP_PHYSICAL_OFFSET};
		}

		my (@intrs, @intr_names);
		my $inst;
		my $intr_name;
		foreach my $inst (qw/A B C D/) {
			my $intr;
			my @intr_matches =
				( "PCIE_INT${inst}", "PCIE${i}_INT${inst}", "PCIE_${i}_INT${inst}");
			my $n_intr_matches = scalar @intr_matches;
			foreach (0..$n_intr_matches - 1) {
				$intr_name = $intr_matches[$_];
				$intr = __find_l1_interrupt($bchp_defines, $intr_name);
				last if defined $intr;
			}
			die "could not find PCIE_INT${inst} interrupt"
				if (!defined($intr));
			push @intrs, $intr;
			push @intr_names, $intr_name if defined($intr);
		}

		my $t = '';
		my $pcie_intr;
		my @intr_prop;
		my @intr_matches =
			( "PCIE_INTR", "PCIE${i}_INTR", "PCIE_${i}_INTR");
		foreach (@intr_matches) {
			$pcie_intr = __find_l1_interrupt($bchp_defines, $_);
			last if defined $pcie_intr;
		}
		die "could not find PCIE_${i}_INTR interrupt\n "
				if (!defined($pcie_intr));

		my $msi_intr_name = "PCIE_${i}_MSI_INTR";
		my $msi_intr = __find_l1_interrupt($bchp_defines, $msi_intr_name);

		if (defined($msi_intr)) {
			@intr_prop = (
			{ "irq" => $rh->{chip_arch} eq "MIPS" ? $pcie_intr : 0,
			  "flags" => [0, 4],
			  "name" => $rh->{chip_arch} eq "MIPS" ? "pcie" : "bogus"
			},
			# Below line will replace the above line in the future.
			#{ irq => $pcie_intr, name => "PCIE_${i}_INTR" },
			{ irq => $msi_intr, name => "msi" },
			);
		} else {
			# legacy; msi and pcie interupt are on in the same.
			@intr_prop = (
			{ "irq" => $rh->{chip_arch} eq "MIPS" ? $pcie_intr : 0,
			  "flags" => [0, 4],
			  "name" => $rh->{chip_arch} eq "MIPS" ? "pcie" : "bogus"
			},
			# Below line will replace the above line in the future.
			#{ irq => $pcie_intr, name => "PCIE_${i}_INTR" },
			{ irq => $pcie_intr, name => "msi" }
			);
		}
		my $pcie_label = sprintf("pcie_%d", $i);
		my $pcie_unit = sprintf("pcie\@%x", $beg);
		if ($n_pcie > 1) {
			$intr_name = "PCIE_${i}_INTR";
		} else {
			$intr_name = "PCIE_INTR";
		}

		$t .= sprintf("%s: %s {\n", $pcie_label, $pcie_unit);
		insert_label($pcie_label, $pcie_unit);
		$t .= "status = \"disabled\";\n"
			if ($disabled);

		# pcie driver uses the following line to get intr offset.
		$t .= sprintf("reg = <0x0 0x%x 0x0 0x%x>;\n", $beg, $size);
		$t .= output_interrupt_prop($rh, \@intr_prop);
		$t .= output_default(\%default);
		$t .= "interrupt-map = <";
		for (my $j=0; $j<4; $j++) {
			$t .= sprintf("%d %d %d %d \&intc %d %d ", 0,0,0,$j+1,$intrs[$j],3);
		}
		$t .= ">;\n";
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));
		add_reference_alias($dt, sprintf("pcie%d", $i), $pcie_label);
	}
}


sub add_moca($$$$$)
{
	my ($dt, $rh, $n_moca, $info, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chip_family_id =
		$bchp_defines->{BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID_chip_family_id_DEFAULT};
	my %default = (compatible => 'brcm,bmoca-instance',
		'hw-rev' => [ "hex", BcmUtils::get_moca_hwrev($bchp_defines) ],
		'chip-id' => [ "hex", $chip_family_id + 0xa0, ],
		);
	override_default(\%default, $info);

	return if (!$n_moca);

	my $beg = bphysaddr($rh, $bchp_defines->{BCHP_DATA_MEM_REG_START});
	my ($t0, $t1) = get_reg_range_from_regexp($rh, "^BCHP_MOCA_HOSTMISC");
	my $size = ($t0 + $t1) - $beg;
	my $intr = find_l1_interrupt($bchp_defines, "MOCA");

	my $t = sprintf("bmoca: bmoca\@%x {\n", $beg);
	insert_label("bmoca", sprintf("bmoca\@%x", $beg));
	my @intr_prop;
	push @intr_prop, { "irq" => $intr, "name" => "MOCA", };
	$t .= output_default(\%default);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $beg, $size);
	my $wol_intr = find_l2_interrupt($bchp_defines, uc($l2_intr), "MOCA");
	push @intr_prop, { "irq" => $wol_intr,
			   "name" => "moca_wol",
			   "parent_intc" => uc($l2_intr)."_intc" };
	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));

	add_reference_alias($dt, "bmoca", "bmoca");
}

sub add_periph_intc($$)
{
	my ($dt, $rh) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %defaults = (compatible => "brcm,bcm7038-l1-intc",
			'interrupt-parent' => [ "phandle", 'cpu_intc'],
			'#interrupt-cells' => [ "dec", 1],
		);

	my ($reg0, $size0) = get_reg_range($rh, "BCHP_HIF_CPU_INTR1");
	my $num_tp_intr = 1;
	my $int_prop = "<2>";

	my $t = sprintf("intc: periph_intc\@%x {\n", $reg0);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>", $reg0, $size0);
	# Not all chips have BCHP_HID_CPU_TP1_INTR1 but some have up
	# to TP3
	for (my $i = 1; $i < 4; $i++) {
		my $reg_name = "BCHP_HIF_CPU_TP${i}_INTR1";
		next if (!defined($bchp_defines->{$reg_name . "_REG_START"}));
		my ($reg, $size) = get_reg_range($rh, $reg_name);
		$t .= sprintf(", <0x%x 0x%x>", $reg, $size);

		# Add the TP interrupt
		if ($num_tp_intr & 1) {
			$int_prop .= ", <3>";
		} else {
			$int_prop .= ", <2>";
		}
		$num_tp_intr++;
	}
	$t .= sprintf(";\n");
	$t .= "interrupt-controller;\n";
	$t .= "interrupts = $int_prop;\n";
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_l2_interrupt($$$$)
{
	my ($dt, $rh, $info, $reg_name) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $intr_name = BcmUtils::get_l2_intc_mapping($reg_name);
	my %defaults = (compatible => "brcm,l2-intc",
			'interrupt-parent' => [ "phandle", 'intc' ],
			'#interrupt-cells' => [ "dec", 1 ],
		);

	override_default(\%defaults, $info);

	my $wake = BcmUtils::l2_intc_can_wake($reg_name);

	my ($reg, $size) = get_reg_range($rh, "BCHP_" . uc($reg_name));
	my $intr = find_l1_interrupt($bchp_defines, uc($intr_name));

	my $label = sprintf("%s_intc", $reg_name);
	my $unit = sprintf("interrupt-controller\@%x", $reg);
	my $t = sprintf("%s: %s {\n", $label, $unit);
	insert_label($label, $unit);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= sprintf("interrupt-controller;\n");
	my %intr_prop = (
		"name" => $intr_name,
		"irq" => $intr
	);
	$t .= output_interrupt_prop($rh, \%intr_prop);
	$t .= sprintf("brcm,irq-can-wake;\n") if $wake;
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

# Find the mask of Level-2 IRQ0_* bits which are serviced by a specified
# Level-1 interrupt
sub find_l2_irq0_int_mask($$$)
{
	my ($bchp_defines, $reg_name, $l1_irq_name) = @_;
	my $fmt;
	my $i;
	my $t = '';
	my $regex = '(?=^((?!reserved).)*$)BCHP_' . uc($reg_name) . '_IRQEN_[A-Za-z_0-9]*_irqen_SHIFT';
	my $l2_names = BcmUtils::get_rdb_fields($regex, $bchp_defines);
	my ($l2_map, $l2_map_irq);
	my $int_mask = 0;

	foreach my $name (@{$l2_names}) {
		my $patt = "BCHP_" . uc($reg_name) . "_IRQEN_";
		$name =~ s/$patt//;
		$name =~ s/_irqen_SHIFT//;
		$l2_map = BcmUtils::get_l2_irq0_intc_internal($reg_name, $name);
		$l2_map_irq = find_l2_irq0_interrupt($bchp_defines, $reg_name, $name);
		# Matches the provided Level-1 interrupt, add it to the mask
		if ($l2_map eq $l1_irq_name) {
			$int_mask |= (1 << $l2_map_irq);
		}
	}

	return $int_mask;
}

sub add_l2_irq0_interrupt($$$$)
{
	my ($dt, $rh, $info, $reg_name) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %defaults = ('compatible' => "brcm,bcm7120-l2-intc",
			'interrupt-parent' => [ "phandle", "intc" ],
			'#interrupt-cells' => [ "dec", 1 ],
			);
	override_default(\%defaults, $info);
	my ($reg, $size) = get_reg_range($rh, "BCHP_" . uc($reg_name));
	my $wake = BcmUtils::l2_intc_can_wake($reg_name);

	my $intr_names = BcmUtils::get_l2_irq0_intc_mapping($reg_name);
	my $n_intrs = scalar @$intr_names;
	my $i;

	my $t = sprintf("%s_intc: interrupt-controller@\%x {\n", $reg_name, $reg);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= "interrupt-controller;\n";

	my @intr_prop;
	my $int_mask_t = 'brcm,int-map-mask = ';
	foreach (0..$n_intrs - 1) {
		my $int_name = @$intr_names[$_];
		my $intr = find_l1_interrupt($bchp_defines, $int_name);
		my $int_mask = find_l2_irq0_int_mask($bchp_defines, $reg_name, $int_name);
		last if !defined $intr;
		push @intr_prop, { "irq" => $intr, "name" => $int_name };
		my $sep = ($_ == $n_intrs - 1) ? ";\n" : ", \n";
		$int_mask_t .= sprintf("<0x%x>", $int_mask) . $sep;
	}
	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= $int_mask_t;
	$t .= sprintf("brcm,int-fwd-mask = <0x%x>;\n",
		find_l2_irq0_int_mask($bchp_defines, $reg_name, "forward"));
	$t .= sprintf("brcm,irq-can-wake;\n") if $wake;
	$t .= "};\n";
	$dt->add_node(DevTree::node->new($t));
}

sub add_clocks($$$$)
{
	my ($dt, $rh, $info, $freq) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my %cpupll_default = (compatible => 'fixed-clock',
		'#clock-cells' => [ 'dec', 0 ],
		'clock-frequency' => [ 'dec', $freq ],
		);
	override_default(\%cpupll_default, $info);

	my $t = sprintf("cpupll: cpupll\@0 {\n");
	insert_label("cpupll", "cpupll\@0");
	$t .= output_default(\%cpupll_default);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));

	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my $chip = sprintf("%x", BcmUtils::get_chip_family_id($bchp_defines));
	my $chip_rev = sprintf("%c",
		((BcmUtils::get_chip_family_rev($bchp_defines) >> 4) & 0xf) +
		ord('a'));
	my $clks_file = "config/clks-" . "$chip${chip_rev}0" . ".plx";

	my $cpu_clk_div_clocks = [ 'cpupll' ];
	# We want phandles for these clocks inside brcm,brcmstb-cpu-clk-div.
	# But only if there's a clocks file for our chip.
	if (-f $clks_file) {
		push(@$cpu_clk_div_clocks, ('cpu_mdiv_ch0', 'cpu_ndiv_int',
						'sw_scb'));
	}
	my %cpuclk_default = (compatible => 'brcm,brcmstb-cpu-clk-div',
		'clock-names' => [ 'string', $cpu_clk_div_clocks ],
		'clocks' => [ 'phandle', $cpu_clk_div_clocks ],
		'#clock-cells' => [ 'dec', 0 ],
		);

	override_default(\%cpuclk_default, $info);

	my $beg = bphysaddr($rh,
		$bchp_defines->{BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG});

	$t = sprintf("cpuclkdiv : cpu_clk_div\@0 {\n");
	insert_label("cpuclkdiv", "cpu_clk_div\@0");
	$t .= output_default(\%cpuclk_default);
	$t .= sprintf("reg = <0x%x 0x4>;\n", $beg);
	if (exists $info->{'-cpu-div-table'}) {
		my @a = @{$info->{"-cpu-div-table"}};
		$t .= sprintf("div-table = <");
		while (@a) {
			$t .= sprintf("%s ", shift(@a));
		}
		$t .= sprintf(">;\n");
	}

	my $clk_ratio_shift = $bchp_defines->{BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CLK_RATIO_SHIFT};
	my $tmp = '';
	vec($tmp, 0, 32) = $bchp_defines->{BCHP_HIF_CPUBIUCTRL_CPU_CLOCK_CONFIG_REG_CLK_RATIO_MASK};
	my $clk_ratio_width = (unpack("B*", $tmp) =~ tr/1//) + 1;

	$t .= sprintf("div-shift-width = <%d %d>;\n", $clk_ratio_shift,
		$clk_ratio_width);

	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
	add_reference_alias($dt, "cpuclkdiv0", "cpuclkdiv");
}

sub add_gisb_arb($$$$)
{
	my ($dt, $rh, $info, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %defaults = (compatible => 'brcm,gisb-arb' );
	override_default(\%defaults, $info);
	my ($reg, $size)
		= get_reg_range($rh, "BCHP_SUN_GISB_ARB");
	my @intr;
	$intr[0] = find_l2_interrupt($bchp_defines, "SUN_L2", "GISB_TIMEOUT");
	$intr[1] = __find_l2_interrupt($bchp_defines, "SUN_L2", "GISB_BUS_ERROR");
	if (!defined($intr[1])) {
		$intr[1] = find_l2_interrupt($bchp_defines, "SUN_L2", "GISB_TEA");
	}
	my @intr_prop = (
		{"name" => "GISB_TIMEOUT", "irq" => $intr[0], "parent_intc" => $l2_intr . "_intc"},
		{"name" => "GISB_TEA", "irq" => $intr[1], "parent_intc" => $l2_intr . "_intc" },
	);

	my $t = sprintf("gisb-arb\@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= sprintf("brcm,gisb-arb-master-mask = <0x%x>;\n", get_gisb_valid_mask($bchp_defines));
	$t .= sprintf("brcm,gisb-arb-master-names = %s;\n", get_gisb_master_names($bchp_defines));
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_waketimer($$$$)
{
	my ($dt, $rh, $info, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %defaults = (compatible => 'brcm,brcmstb-waketimer');
	override_default(\%defaults, $info);
	my ($reg, $size) = get_reg_range($rh, "BCHP_WKTMR");
	my $intr = find_l2_interrupt($bchp_defines, "AON_PM_L2", "TIMER");

	my $t = sprintf("waketimer\@%x {\n", $reg);
	my %intr_prop = (
		"name" => "timer",
		"parent_intc" => $l2_intr . "_intc",
		"irq" => $intr,
	);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= output_interrupt_prop($rh, \%intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub gen_reg_node($$$$$$)
{
	my ($dt, $rh, $name, $bchp_name, $intr, $size) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = sprintf("%x", BcmUtils::get_chip_family_id($bchp_defines));
	my %defaults = (compatible => [ 'string', [ "brcm,bcm$chipid-$name",
						"brcm,$name" ] ]);
	my ($reg, $rs) = get_reg_range($rh, $bchp_name);
	$size = $rs if ($size == 0);
	my $t = sprintf("$name\@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= output_interrupt_prop($rh, $intr) if (defined($intr));
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_avs_cpu($$)
{
	my ($dt, $rh) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $intr = $bchp_defines->{BCHP_AVS_HOST_L2_STATUS0_SW_INTR_SHIFT};
	my %host_intr = (
		'name' => 'sw_intr',
		'irq' => $intr,
		'parent_intc' => 'avs_host_l2_intc');
	gen_reg_node($dt, $rh, "avs-cpu-data-mem", "BCHP_AVS_CPU_DATA_MEM",
		\%host_intr, 0x60);
	gen_reg_node($dt, $rh, "avs-cpu-l2-intr", "BCHP_AVS_CPU_L2",
		undef, 0x10);
}

sub add_avs_tmon($$$$)
{
	my ($dt, $rh, $info, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %defaults = (compatible => [ "string", [ "brcm,avs-tmon-bcm7445",
						    "brcm,avs-tmon" ] ],
			'#thermal-sensor-cells' => [ 'dec', 0 ],
		);
	override_default(\%defaults, $info);
	my ($reg, $size) = get_reg_range($rh, "BCHP_AVS_TMON");
	my $intr = find_l2_interrupt($bchp_defines, "AVS_HOST_L2", "AVS_TMON_INTR");

	my $unit = sprintf("thermal\@%x", $reg);
	my $t = sprintf("avs_tmon: %s {\n", $unit);
	insert_label("avs_tmon", $unit);
	my %intr_prop = (
		"name" => "tmon",
		"irq" => $intr,
		"parent_intc" => $l2_intr . "_intc",
	);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= output_interrupt_prop($rh, \%intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_thermal_zones($$$)
{
	my ($dt, $rh, $info) = @_;
	my $zones = DevTree::node->new("thermal-zones { };");
	$dt->add_node($zones);

	my %defaults = ('polling-delay-passive' => [ 'dec', 0 ],
			'polling-delay' => [ 'dec', 0 ],
			'thermal-sensors' => [ 'phandle', '' ], # provided in cfg
		);
	override_default(\%defaults, $info);

	my $t = "cpu-thermal {\n";
	$t .= output_default(\%defaults);
	$t .= "};\n";
	my $zone = DevTree::node->new($t);
	$zones->add_node($zone);

	if (exists $info->{'-trips'}) {
		my @trips = @{$info->{"-trips"}};
		die "option '-trips' requres even number of elements"
			if (scalar(@trips) % 2 != 0);

		my $trips_node = DevTree::node->new("trips { };");
		$zone->add_node($trips_node);

		my $ntrips = scalar(@trips) / 2;
		for (my $i = 0; $i < $ntrips; $i++) {
			$t = sprintf("cpu-alert%d {\n", $i);
			$t .= sprintf("temperature = <%d>;\n", shift(@trips));
			$t .= sprintf("hysteresis = <%d>;\n", shift(@trips));
			$t .= "type = \"passive\";\n";
			$t .= "};\n";
			$trips_node->add_node(DevTree::node->new($t));
		}
	}
}

sub add_sram($$$$)
{
	my ($dt, $reg, $len, $info) = @_;
	my %defaults = (compatible => [ 'string', [ 'brcm,boot-sram', 'mmio-sram'] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("sram\@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $len);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_aon_ctrl($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $ctrl_reg = bphysaddr($rh, $bchp_defines->{BCHP_AON_CTRL_REG_START});
	my $sram_reg = bphysaddr($rh, $bchp_defines->{BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE});
	my $sram_len = ($bchp_defines->{BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_END} + 1) * 4;

	my %defaults = (compatible => 'brcm,brcmstb-aon-ctrl',
			'reg-names' => [ 'string', [ 'aon-ctrl', 'aon-sram' ] ],
			'reg' => [ 'hex', [ $ctrl_reg, $sram_reg - $ctrl_reg,
					    $sram_reg, $sram_len ] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("aon-ctrl\@%x {\n", $ctrl_reg);
	$t .= output_default(\%defaults);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_ddr_phy($$$$)
{
	my ($dt, $rh, $phy_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $compatible;
	my $reg;
	my $size;

	if (defined($bchp_defines->{BCHP_DDR34_PHY_CONTROL_REGS_0_PRIMARY_REVISION})) {
		# 7260A0
		$compatible = sprintf("brcm,brcmstb-ddr-phy-v%d.%d",
		$bchp_defines->{BCHP_DDR34_PHY_CONTROL_REGS_0_PRIMARY_REVISION_MAJOR_DEFAULT},
		$bchp_defines->{BCHP_DDR34_PHY_CONTROL_REGS_0_PRIMARY_REVISION_MINOR_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_DDR34_PHY_CONTROL_REGS_${phy_idx}");
	} elsif (defined($bchp_defines->{BCHP_DDR34_PHY_COMMON_REGS_0_PRIMARY_REVISION})) {
		# 7271A0
		$compatible = sprintf("brcm,brcmstb-ddr-phy-v%d.%d",
		$bchp_defines->{BCHP_DDR34_PHY_COMMON_REGS_0_PRIMARY_REVISION_MAJOR_DEFAULT},
		$bchp_defines->{BCHP_DDR34_PHY_COMMON_REGS_0_PRIMARY_REVISION_MINOR_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_DDR34_PHY_COMMON_REGS_${phy_idx}");
	} elsif (defined($bchp_defines->{BCHP_DDR40_PHY_CONTROL_REGS_0_REG_START})) {
		# 40nm platforms
		$compatible = sprintf("brcm,brcmstb-ddr-phy-v%d.%d",
		$bchp_defines->{BCHP_DDR40_PHY_CONTROL_REGS_0_REVISION_MAJOR_DEFAULT},
		$bchp_defines->{BCHP_DDR40_PHY_CONTROL_REGS_0_REVISION_MINOR_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_DDR40_PHY_CONTROL_REGS_${phy_idx}");
	} else {
		# everything else
		$compatible = sprintf("brcm,brcmstb-ddr-phy-v%d.%d",
		$bchp_defines->{BCHP_DDR34_PHY_CONTROL_REGS_0_REVISION_MAJOR_DEFAULT},
		$bchp_defines->{BCHP_DDR34_PHY_CONTROL_REGS_0_REVISION_MINOR_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_DDR34_PHY_CONTROL_REGS_${phy_idx}");
	}

	my %defaults = (compatible => [compat_with($compatible, "ddr-phy", $info)],
		'reg' => [ 'hex', [ $reg, $size ] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("ddr-phy@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_ddr_shim_phy($$$$)
{
	my ($dt, $rh, $phy_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $compatible;
	my ($reg, $size);

	return unless (defined($bchp_defines->{BCHP_SHIMPHY_ADDR_CNTL_0_REG_START}) or
		      defined($bchp_defines->{BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_REG_START}));

	if (defined($bchp_defines->{"BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_REG_START"})) {
		$compatible = sprintf("brcm,brcmstb-ddr-shimphy-v%d.%d",
		$bchp_defines->{BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_DDR23_SHIM_REV_ID_MAJOR_ID_DEFAULT},
		$bchp_defines->{BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_DDR23_SHIM_REV_ID_MINOR_ID_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_${phy_idx}");
	} else {
		$compatible = sprintf("brcm,brcmstb-ddr-shimphy-v%d.%d",
		$bchp_defines->{BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MAJOR_ID_DEFAULT},
		$bchp_defines->{BCHP_SHIMPHY_ADDR_CNTL_0_SHIMPHY_REV_ID_MINOR_ID_DEFAULT});
		($reg, $size) = get_reg_range($rh, "BCHP_SHIMPHY_ADDR_CNTL_${phy_idx}");
	}
	my %defaults = (compatible => $compatible,
		'reg' => [ 'hex', [ $reg, $size ] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("shimphy@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_memc_ddr($$$$)
{
	my ($dt, $rh, $memc_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $compat_base = 'brcm,brcmstb-memc-ddr';
	my @compat_strings = ($compat_base);
	my $rev_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_ARCH_REV_ID_DEFAULT"};
	my $cfg_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_CFG_REV_ID_DEFAULT"};
	my $sub_cfg_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_SUB_CFG_REV_ID_DEFAULT"};

	return unless defined($bchp_defines->{BCHP_MEMC_DDR_0_REG_START});

	if (defined($rev_id)) {
		my $revision = sprintf("%x.%x.%x", $rev_id, $cfg_id, $sub_cfg_id);
		if ($rev_id == 0xb && $cfg_id == 0x1) {
			my $extra_rev = sprintf("%x.%x.x", $rev_id, $cfg_id);
			# For memc rev b.1, we want a b.1.x compatible string.
			unshift(@compat_strings, "$compat_base-rev-$extra_rev");
		}
		# The most specific compatible string goes before the more
		# generic ones.
		unshift(@compat_strings, "$compat_base-rev-$revision");
	}

	my ($reg, $size) = get_reg_range($rh, "BCHP_MEMC_DDR_${memc_idx}");
	my %defaults = (compatible => [ 'string', \@compat_strings ],
		'reg' => [ 'hex', [ $reg, $size ] ],
		);
	override_default(\%defaults, $info);

	my $unit = sprintf("memc-ddr@%x", $reg);
	my $label = sprintf("memc_ddr%d", $memc_idx);
	my $t = sprintf("%s: %s {\n", $label, $unit);
	insert_label($label, $unit);
	$t .= output_default(\%defaults);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_memc_gen($$$$)
{
	my ($dt, $rh, $memc_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $compat_base = 'brcm,brcmstb-memc-gen';
	my @compat_strings = ($compat_base);
	my $rev_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_ARCH_REV_ID_DEFAULT"};
	my $cfg_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_CFG_REV_ID_DEFAULT"};
	my $sub_cfg_id = $bchp_defines->{"BCHP_MEMC_GEN_${memc_idx}_CORE_REV_ID_SUB_CFG_REV_ID_DEFAULT"};

	return unless defined($bchp_defines->{BCHP_MEMC_GEN_0_REG_START});

	if (defined($rev_id)) {
		my $revision = sprintf("%x.%x.%x", $rev_id, $cfg_id, $sub_cfg_id);
		# The specific compatible string goes before the generic one.
		unshift(@compat_strings, "$compat_base-rev-$revision");
	}

	my ($reg, $size) = get_reg_range($rh, "BCHP_MEMC_GEN_${memc_idx}");
	my %defaults = (
		'compatible' => [ 'string', \@compat_strings ],
		'reg' => [ 'hex', [ $reg, $size ] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("memc-gen@%x {\n", $reg);
	$t .= output_default(\%defaults);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_memc_arb($$$$)
{
	my ($dt, $rh, $memc_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $memc_client_defines = $rh->{memc_client_defines};

	return unless defined($bchp_defines->{BCHP_MEMC_ARB_0_REG_START});

	my ($reg, $size) = get_reg_range($rh, "BCHP_MEMC_ARB_${memc_idx}");
	my %defaults = (compatible => "brcm,brcmstb-memc-arb",
		'reg' => [ 'hex', [ $reg, $size ] ],
		);
	override_default(\%defaults, $info);

	my $t = sprintf("memc-arb@%x {\n", $reg);
	$t .= output_default(\%defaults);

	if (defined($memc_client_defines->[$memc_idx])) {
		$t .= sprintf("brcm,memc-client-masks = %s\;\n",
		      get_memc_client_masks($memc_client_defines->[$memc_idx]));
		$t .= sprintf("brcm,memc-client-names = %s\;\n",
		      get_memc_client_names($memc_client_defines->[$memc_idx]));
		$t .= sprintf("brcm,memc-max-clients = <%d>;\n",
			      scalar(@{$memc_client_defines->[$memc_idx]}));
	}
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_hif_spi_intc($$)
{
	my ($rh, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};

	my @hif_spi_irq;
	$hif_spi_irq[6] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "MSPI_HALTED");
	$hif_spi_irq[5] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "MSPI_DONE");
	$hif_spi_irq[4] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "SPI_LR_OVERREAD");
	$hif_spi_irq[3] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "SPI_LR_SESSION_DONE");
	$hif_spi_irq[2] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "SPI_LR_IMPATIENT");
	$hif_spi_irq[1] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "SPI_LR_SESSION_ABORTED");
	$hif_spi_irq[0] = find_l2_interrupt($bchp_defines, "HIF_SPI_INTR2", "SPI_LR_FULLNESS_REACHED");
	my @intr_prop = (
		{ "parent_intc" => $l2_intr . "_intc", "name" => "mspi_halted", "irq" => $hif_spi_irq[6]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "mspi_done", "irq" => $hif_spi_irq[5]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "spi_lr_overread", "irq" => $hif_spi_irq[4]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "spi_lr_session_done", "irq" => $hif_spi_irq[3]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "spi_lr_impatient", "irq" => $hif_spi_irq[2]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "spi_lr_session_aborted", "irq" => $hif_spi_irq[1]},
		{ "parent_intc" => $l2_intr . "_intc", "name" => "spi_lr_fullness_reached", "irq" => $hif_spi_irq[0]},
	);

	return output_interrupt_prop($rh, \@intr_prop);
}

sub add_qspi($$$$)
{
	my ($dt, $rh, $l2_intr, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string', [ "brcm,spi-bcm-qspi", "brcm,spi-brcmstb-qspi" ] ],
		"#size-cells" => [ 'dec', 0 ],
		"#address-cells" => [ 'dec', 1 ],
		"status" => "disabled",
	);
	my @reg_names = ("HIF_MSPI", "BSPI");
	my @regs;
	my $i;
	my $t = '';
	my $reg_t = '';
	my $ebi_cs_spi_reg = bphysaddr($rh, $bchp_defines->{BCHP_EBI_CS_SPI_SELECT});

	override_default(\%default, $info);

	for ($i = 0; $i < scalar(@reg_names); $i++) {
		($regs[$i][0], $regs[$i][1]) = get_reg_range($rh, "BCHP_" . $reg_names[$i]);
	}

	$t .= sprintf("qspi\@%x {\n", $regs[0][0]);
	$t .= output_default(\%default);
	$t .= "reg = <";
	$t .= sprintf("0x%x 0x4 ", $ebi_cs_spi_reg);
	$reg_t = "reg-names = ";
	$reg_t .= sprintf("\"%s\",", "cs_reg");
	for ($i = 0; $i < scalar(@reg_names); $i++) {
		$t .= sprintf("0x%x 0x%x%s", $regs[$i][0], $regs[$i][1],
			$i ne scalar(@reg_names) -1 ? " " : "");
		$reg_t .= sprintf("\"%s\"", lc($reg_names[$i]));
		$reg_t .= $i ne scalar(@reg_names) -1 ? ",\n" : ";\n";
	}
	$t .= ">;\n";
	$t .= $reg_t;
	$t .= add_hif_spi_intc($rh, $l2_intr);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_spi($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string',
				  [ "brcm,spi-brcmstb", "brcm,spi-bcm-qspi" ] ],
		"#size-cells" => [ 'dec', 0 ],
		"#address-cells" => [ 'dec', 1 ],
		"status" => "disabled",
	);
	my @reg_names = ("HIF_MSPI", "BSPI", "BSPI_RAF", "HIF_SPI_INTR2");
	my @regs;
	my $i;
	my $t = '';
	my $reg_t = '';
	my $ebi_cs_spi_reg = bphysaddr($rh, $bchp_defines->{BCHP_EBI_CS_SPI_SELECT});

	override_default(\%default, $info);

	for ($i = 0; $i < scalar(@reg_names); $i++) {
		($regs[$i][0], $regs[$i][1]) = get_reg_range($rh, "BCHP_" . $reg_names[$i]);
	}
	my $intr = find_l1_interrupt($bchp_defines, "HIF_SPI");
	my %intr_prop = ("name" => "hif_spi",
			 "irq" => $intr
	);

	$t .= sprintf("spi\@%x {\n", $regs[0][0]);
	$t .= output_default(\%default);
	$t .= "reg = <";
	$reg_t = "reg-names = ";
	for ($i = 0; $i < scalar(@reg_names); $i++) {
		$t .= sprintf("0x%x 0x%x%s", $regs[$i][0], $regs[$i][1],
			$i ne scalar(@reg_names) -1 ? " " : "");
		$reg_t .= sprintf("\"%s\"", lc($reg_names[$i]));
		$reg_t .= $i ne scalar(@reg_names) -1 ? ",\n" : ",\"cs_reg\";\n";
	}
	$t .= sprintf(" 0x%x 0x4 ", $ebi_cs_spi_reg);
	$t .= ">;\n";
	$t .= $reg_t;
	$t .= output_interrupt_prop($rh, \%intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_mspi($$$$$)
{
	my ($dt, $rh, $info, $l2_intr, $clks) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string', [ "brcm,spi-bcm-qspi", "brcm,spi-brcmstb-mspi" ] ],
		"#size-cells" => [ 'dec', 0 ],
		"#address-cells" => [ 'dec', 1 ],
	);
	my @reg_names = ("MSPI");
	my @regs;
	my $i;
	my $t = '';
	my $reg_t = '';

	if ($clks) {
		$default{"clocks"} = [ 'phandle', 'upg_fixed' ];
	} else {
		$default{"clock-frequency"} = [ 'dec', 27000000 ];
	}

	override_default(\%default, $info);

	for ($i = 0; $i < scalar(@reg_names); $i++) {
		($regs[$i][0], $regs[$i][1]) = get_reg_range($rh, "BCHP_" . $reg_names[$i]);
	}

	my $intr = __find_l2_irq0_interrupt($bchp_defines, uc($l2_intr), "spi");
	if (!defined($intr)) {
		$l2_intr = "upg_spi_aon_irq";
		$intr = __find_l2_interrupt($bchp_defines, uc($l2_intr), "spi");
	}
	die "$P: cannot find spi level-2 interrupt"
		if !defined $intr;

	my %intr_prop = ("name" => "mspi_done",
			 "irq" => $intr,
			 "parent_intc" => $l2_intr . "_intc",
	);

	$t .= sprintf("spi\@%x {\n", $regs[0][0]);
	$t .= output_default(\%default);
	$t .= "reg = <";
	$reg_t = "reg-names = ";
	for ($i = 0; $i < scalar(@reg_names); $i++) {
		$t .= sprintf("0x%x 0x%x%s", $regs[$i][0], $regs[$i][1],
			$i ne scalar(@reg_names) -1 ? " " : "");
		$reg_t .= sprintf("\"%s\"", lc($reg_names[$i]));
		$reg_t .= $i ne scalar(@reg_names) -1 ? ",\n" : ";\n";
	}
	$t .= ">;\n";
	$t .= $reg_t;
	$t .= output_interrupt_prop($rh, \%intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_nand_intc($$$)
{
	my ($rh, $l2_intr, $dma) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $t = "";
	my $int_name_t = "";
	my @intr_prop;

	my $ctlrdy_irq = find_l2_interrupt($bchp_defines, "HIF_INTR2", "NAND_CTLRDY");
	push @intr_prop, { "parent_intc" => $l2_intr . "_intc",
			   "name" => "nand_ctlrdy",
			   "irq" => $ctlrdy_irq };
	my $dma_irq;
	$int_name_t .= "interrupt-names = \"nand_ctlrdy\"";

	if ($dma) {
		$dma_irq = find_l2_interrupt($bchp_defines, "HIF_INTR2", "FLASH_DMA_DONE");
		push @intr_prop, { "parent_intc" => $l2_intr . "_intc",
				   "name" => "flash_dma_done",
				   "irq" => $dma_irq };
	}

	$t .= output_interrupt_prop($rh, \@intr_prop);

	return $t;
}

sub add_nand($$$$$)
{
	my ($dt, $rh, $flash_dma, $l2_intr, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = ('compatible' => [ 'string',
			[ sprintf("brcm,brcmnand-v%d.%d",
				$bchp_defines->{BCHP_NAND_REVISION_MAJOR_DEFAULT},
				$bchp_defines->{BCHP_NAND_REVISION_MINOR_DEFAULT}),
			"brcm,brcmnand" ] ],
			'#address-cells' => [ 'dec', 1 ],
			'#size-cells' => [ 'dec', 0 ],
			'status' => 'disabled',
			'reg-names' => [ 'string', $flash_dma ?
				[ "nand", "flash-dma" ] : "nand" ],
		);
	override_default(\%default, $info);
	my ($reg, $size)
		= get_reg_range($rh, "BCHP_NAND");
	my ($dma_reg, $dma_size)
		= get_reg_range($rh, "BCHP_FLASH_DMA");
	my $intr = find_l1_interrupt($bchp_defines, "HIF");

	my $t = sprintf("nand\@%x {\n", $reg);
	$t .= output_default(\%default);
	$t .= sprintf("reg = <0x%x 0x%x", $reg, $size);
	if ($flash_dma) {
		$t .= sprintf(" 0x%x 0x%x", $dma_reg, $dma_size);
	}
	$t .= sprintf(">;\n");
	$t .= add_nand_intc($rh, $l2_intr, $flash_dma);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_gic($$$)
{
	my ($dt, $rh, $n_gic) = @_;
	my $bchp_defines = $rh->{rh_defines};

	return if (!$n_gic);

	my $t = sprintf("intc: interrupt-controller");
	if ($bchp_defines->{"BCHP_PHYSICAL_OFFSET"} == 0x08000000) {
		$t .= sprintf("\@00200000 {\n");
	} else {
		$t .= sprintf("\@ffd00000 {\n");
	}
	$t .= sprintf("compatible = \"arm,cortex-a15-gic\";\n");
	$t .= sprintf("interrupt-controller;\n");
	$t .= sprintf("#interrupt-cells = <0x3>;\n");
	if ($bchp_defines->{"BCHP_PHYSICAL_OFFSET"} == 0x08000000) {
		$t .= sprintf("reg = <0x00 0x00201000 0x00 0x1000>,\n");
		$t .= sprintf("<0x00 0x00202000 0x00 0x2000>;\n");
	} else {
		$t .= sprintf("reg = <0x00 0xffd01000 0x00 0x1000>,\n");
		$t .= sprintf("<0x00 0xffd02000 0x00 0x2000>;\n");
	}
	$t .= "reg-names = \"dist\", \"cpu\";\n";
	$t .= "};\n";
	$dt->add_node(DevTree::node->new($t));

	# Sets the "interrupt-parent" property in the root node
	$dt->add_prop(DevTree::prop->new("interrupt-parent = <&intc>;"));
}

sub add_sun_top_ctrl($$)
{
	my ($dt, $rh) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my $compatible = '';

	my ($reg, $size)
		= get_reg_range($rh, "BCHP_SUN_TOP_CTRL");

	if ($rh->{chip_arch} eq "MIPS") {
		$compatible = sprintf("compatible = \"brcm,bcm%x-sun-top-ctrl\",
				\"syscon\";\n",$chipid)
	} else {
		$compatible = sprintf("compatible = \"brcm,brcmstb-sun-top-ctrl\",
				\"syscon\";\n");

	}
	my $t = sprintf("sun_top_ctrl: syscon@%x {\n", $reg);
	$t .=  $compatible;
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= "};\n";

	my $sun_top_ctrl_node = DevTree::node->new($t);

	$dt->add_node($sun_top_ctrl_node);
}


sub us2dash($)
{	my $a = shift;
	$a =~ s/_/-/g;
	return $a;
}

sub add_rdb_syscon_regset_byname($$$$$$)
{
	my ($dt, $rh, $info, $inre, $dom, $name) = @_;
	return if( !keys %$info );

	my $bchp_defines = $rh->{rh_defines};
	my $label = $dom . "_" . $name;
	my $of_name = us2dash($label);
	my $refnode = $info->{"-ref"}[0];

	my ($reg, $size) = get_reg_range_from_regexp($rh, $inre);
	return if (!defined($reg));

	my $unit = sprintf("syscon@%x", $reg);
	my $t = sprintf("%s: %s {\n", $label, $unit);
	insert_label($label, $unit);
	$t .= sprintf("compatible = \"brcm,brcmstb-%s\", \"%s\";\n",
		$of_name, "syscon");
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= "};\n";

	my $rdb_node = DevTree::node->new($t);
	$dt->add_node($rdb_node);

	if (defined $refnode) {
		append_rdb_syscon_ref_phandle($refnode, $label);
	}
}

sub add_sun_top_pin_mux_ctrl($$$)
{
	my ($dt, $rh, $info) = @_;
	my $regex = "^BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_[0-9]+\\b";
	add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		"sun_top_ctrl", "pin_mux_ctrl");
}

sub add_sun_top_pad_mux_ctrl($$$)
{
	my ($dt, $rh, $info) = @_;
	my $regex = "^BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_[0-9]+\\b";
	add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		"sun_top_ctrl",	"pin_mux_pad_ctrl");
}

sub add_aon_pin_mux_ctrl($$$)
{
	my ($dt, $rh, $info) = @_;
	my $regex = "^BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_[0-9]+\\b";
	add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		"aon_pin_ctrl", "pin_mux_ctrl");
}

sub add_aon_pad_mux_ctrl($$$)
{
	my ($dt, $rh, $info) = @_;
	my $regex = "^BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_[0-9]+\\b";
	add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		"aon_pin_ctrl", "pad_mux_ctrl");
}

sub add_memc_client_info($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $regex_base = "^BCHP_MEMC_ARB_%d_CLIENT_INFO_[0-9]+\\b";

	my $n = BcmUtils::get_num_memc($bchp_defines);

	for( my $i=0; $i < $n; $i++) {
		my $regex = sprintf($regex_base, $i);
		my $dom = sprintf("memc_arb_%d", $i);
		add_rdb_syscon_regset_byname($dt, $rh, $info,
			$regex, $dom, "client_info");
	}
}

sub add_sdio_syscon($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $regex;

	if (defined $bchp_defines->{"BCHP_SDIO_0_CFG_SD_PIN_SEL"}) {
	    $regex = "^BCHP_SDIO_0_CFG_SD_PIN_SEL+\\b";
	    add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
					 "sdio_0_cfg", "sd_pin_sel");
	}
	if (defined $bchp_defines->{"BCHP_SDIO_1_CFG_SD_PIN_SEL"}) {
	    $regex = "^BCHP_SDIO_1_CFG_SD_PIN_SEL+\\b";
	    add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
					 "sdio_1_cfg", "sd_pin_sel");
	}
	if (defined $bchp_defines->{"BCHP_SDIO_1_BOOT_MAIN_CTL"}) {
	    $regex = "^BCHP_SDIO_1_BOOT_MAIN_CTL+\\b";
	    add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
					 "sdio_1_boot", "main_ctl");
	}
}

sub add_sun_top_ctrl_general_ctrl($$$)
{
       my ($dt, $rh, $info) = @_;
       my $sel = $info->{"-sel"}[0];
       my $regex = "^BCHP_SUN_TOP_CTRL_GENERAL_CTRL_";

       if (defined($sel)) {
	       $regex .= $sel;
	       add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		       "sun_top_ctrl", "general_ctrl_". $sel);
       } else {
	       $regex .= "[0-9]+\\b";
	       add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		       "sun_top_ctrl", "general_ctrl");
       }
}

sub add_sun_top_ctrl_general_ctrl_no_scan($$$)
{
       my ($dt, $rh, $info) = @_;
       my $sel = $info->{"-sel"}[0];
       my $regex = "^BCHP_SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_";

       if (defined($sel)) {
	       $regex .= $sel . "+\\b";
	       add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		       "sun_top_ctrl", "general_ctrl_no_scan_". $sel);
       } else {
	       $regex .= "[0-9]+\\b";
	       add_rdb_syscon_regset_byname($dt, $rh, $info, $regex,
		       "sun_top_ctrl", "general_ctrl_no_scan");
       }
}

sub add_cpu_biu_ctrl($$$)
{
	my ($dt, $rh, $family_id) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $mcp_wr_pairing_allowed = BcmUtils::mcp_wr_pairing_allowed($family_id);

	my ($reg, $size)
		= get_reg_range($rh, "BCHP_HIF_CPUBIUCTRL");

	my $unit = sprintf("syscon@%x", $reg);
	my $t = sprintf("cpu_biu_ctrl: %s {\n", $unit);
	insert_label("cpu_biu_ctrl", $unit);
	$t .= sprintf("compatible = \"brcm,brcmstb-cpu-biu-ctrl\", \"syscon\";\n");
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	if ($mcp_wr_pairing_allowed) {
		$t .= sprintf("brcm,write-pairing;\n");
	}
	$t .= "};\n";

	my $cpu_biu_ctrl_node = DevTree::node->new($t);

	$dt->add_node($cpu_biu_ctrl_node);
}

sub add_hif_cont($$)
{
	my ($dt, $rh) = @_;

	my ($reg, $size)
		= get_reg_range($rh, "BCHP_HIF_CONTINUATION");

	my $unit = sprintf("syscon@%x", $reg);
	my $t = sprintf("hif_continuation: %s {\n", $unit);
	insert_label("hif_continuation", $unit);
	$t .= sprintf("compatible = \"brcm,brcmstb-hif-continuation\", \"syscon\";\n");
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg, $size);
	$t .= "};\n";

	my $hif_cont_node = DevTree::node->new($t);

	$dt->add_node($hif_cont_node);
}

sub add_reboot($$)
{
	my ($dt, $rh) = @_;

	my $rst_src_en_ofs = get_offset_from_base($rh, "BCHP_SUN_TOP_CTRL",
		"BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE");
	my $sw_mstr_rst_ofs = get_offset_from_base($rh, "BCHP_SUN_TOP_CTRL",
		"BCHP_SUN_TOP_CTRL_SW_MASTER_RESET");

	my $t = sprintf("reboot {\n");
	$t .= sprintf("compatible = \"brcm,brcmstb-reboot\";\n");
	$t .= sprintf("syscon = <&sun_top_ctrl 0x%x 0x%x>;\n", $rst_src_en_ofs,
		$sw_mstr_rst_ofs);
	$t .= "};\n";

	my $reboot_node = DevTree::node->new($t);
	$dt->add_node($reboot_node);
}

sub add_smpboot($$)
{
	my ($dt, $rh) = @_;

	my $cpu_pwr_zn_ofs = get_offset_from_base($rh, "BCHP_HIF_CPUBIUCTRL",
		"BCHP_HIF_CPUBIUCTRL_CPU0_PWR_ZONE_CNTRL_REG");
	my $cpu_rst_ofs = get_offset_from_base($rh, "BCHP_HIF_CPUBIUCTRL",
		"BCHP_HIF_CPUBIUCTRL_CPU_RESET_CONFIG_REG");

	my $t = sprintf("smpboot {\n");
	$t .= sprintf("compatible = \"brcm,brcmstb-smpboot\";\n");
	$t .= sprintf("syscon-cpu = <&cpu_biu_ctrl 0x%x 0x%x>;\n",
		$cpu_pwr_zn_ofs, $cpu_rst_ofs);
	$t .= sprintf("syscon-cont = <&hif_continuation>;\n");
	$t .= "};\n";

	my $smpboot_node = DevTree::node->new($t);
	$dt->add_node($smpboot_node);
}

sub add_systemport($$$$$)
{
	my ($dt, $rh, $n_systemport, $info, $l2_intr) = @_;
	my ($i, $num, $str, $intr);
	my $bchp_defines = $rh->{rh_defines};
	my $tmp = sprintf("BCHP_SYSTEMPORTLITE_%d_TOPCTRL_REG_START", $n_systemport - 1);
	my $is_lite = defined($bchp_defines->{$tmp}) ? 1 : 0;
	my ($prefix, $bchp_prefix);

	if ($is_lite eq 1 and $n_systemport > 1) {
		$prefix = "SYSTEMPORTLITE";
		$bchp_prefix = sprintf("%s_%d", $prefix, $n_systemport - 1);
	} else {
		$prefix = "SYSTEMPORT";
		$bchp_prefix = $prefix;
	}

	my %default = ("compatible" => [ 'string',
			[ sprintf("brcm,%s-v%d.%02d",
			lc($prefix),
			$bchp_defines->{"BCHP_".$bchp_prefix."_TOPCTRL_REV_CNTL_SYS_PORT_REV_DEFAULT"} >> 8,
			$bchp_defines->{"BCHP_".$bchp_prefix."_TOPCTRL_REV_CNTL_SYS_PORT_REV_DEFAULT"} & 0xff),
			"brcm,systemport" ] ],
		       "device_type" => "network",
		       "fixed-link" => [ 'dec', [ 0, 1, 1000, 0, 0 ] ],
		       "phy-mode" => [ 'string', 'gmii' ],
		       "systemport,num-txq" => [ 'dec', BcmUtils::get_num_systemport_queues($bchp_defines) ],
		       "systemport,num-tier1-arb" => [ 'dec',
			       BcmUtils::get_num_systemport_tier_arb($bchp_defines, 1) ],
		       "systemport,num-tier2-arb" => [ 'dec',
			       BcmUtils::get_num_systemport_tier_arb($bchp_defines, 2) ],
		       "systemport,num-rxq" => [ 'dec', 1 ],
		      );

	override_default(\%default, $info);

	for ($num = 0; $num < $n_systemport; $num++) {
		next if ($info->{'-choose'} && !$info->{'-choose'}->[$num]);
		my ($inst_name, $wol_name);
		my @intr_prop;

		if ($n_systemport > 1 and $is_lite eq 1) {
			$inst_name = sprintf("SYSTEMPORTLITE_%d", $num);
			$intr = find_l1_interrupt($bchp_defines, $inst_name);
			push @intr_prop, { "irq" => $intr, "name" => lc($prefix), };

			# Wake-on-LAN interrupt names do not include "LITE" in their
			# name
			$wol_name = sprintf("SYSTEMPORT_%d", $num);
		} else {
			$inst_name = $prefix;
			for ($i = 0; $i < 2; $i++) {
				my $intr_name = sprintf($inst_name.'_%s', $i);
				$intr = find_l1_interrupt($bchp_defines, $intr_name);
				last if !defined $intr;
				push @intr_prop, { "irq" => $intr, "name" => lc($intr_name), };
			}
			$wol_name = $prefix;
		}

		# GR_BRIDGE is not necessarily located within the SYSTEMPORLITE_*
		# register range, so just exclude it here
		my ($beg, $size)
			= get_reg_range_from_regexp($rh,
						    "^BCHP_".$inst_name."_(?!GR_BRIDGE)");

		my $nintrs = scalar @intr_prop;

		my $t = '';
		my $ext_t = '';
		my $int_t = '';
		my $unit = sprintf("ethernet\@%x", $beg);
		my $label = sprintf("enet_%d", $num);
		$t .= sprintf("%s: %s {\n", $label, $unit);
		insert_label($label, $unit);
		$t .= output_default(\%default);
		$t .= sprintf("%s = <0x%x 0x%x>;\n", 'reg', $beg, $size);
		if (@intr_prop) {
			$intr = find_l2_interrupt($bchp_defines, uc($l2_intr), "WOL_$wol_name");
			push @intr_prop, { "irq" => $intr,
					   "name" => "wol_".lc($wol_name),
					   "parent_intc" => $l2_intr . "_intc" };
		}
		$t .= output_interrupt_prop($rh, \@intr_prop);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));

		add_reference_alias($dt, sprintf("eth%d", $num), $label);
	}
}

sub add_pmu($$)
{
	my ($dt, $rh) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string', [ 'arm,cortex-a53-pmu', 'arm,armv8-pmuv3', 'arm,cortex-a15-pmu', 'arm,cortex-a9-pmu' ] ],
	);
	my @intr_prop;
	my $j;
	my $intr;
	my $pmu_cpu_mask;
	foreach $j (qw/0 1 2 3/) {
		my $intr_name = sprintf('CPU_PMU_IRQ_%d', $j);
		$intr = __find_l1_interrupt($bchp_defines, $intr_name);
		if (!$intr) {
			$intr_name = sprintf('CPU_PMUIRQ_%d', $j);
			$intr = __find_l1_interrupt($bchp_defines, $intr_name);
		}
		if (!$intr) {
			$intr_name = sprintf('EAG_PMUIRQ_%d', $j);
			$intr = __find_l1_interrupt($bchp_defines, $intr_name);
		}
		$pmu_cpu_mask |= (1 << $j);
		last if !defined $intr;
		# PMU interrupts are SGI and IRQ_TYPE_LEVEL_HIGH and wired
		# to all processor, hence we set the relevant CPU_MASK
		# bits all CPUs
		push @intr_prop, { "irq" => $intr,
				   "name" => lc($intr_name),
			   	 };
	}
	my $nintrs = scalar @intr_prop;
	# Append the final mask now
	foreach (0..$nintrs - 1) {
		$intr_prop[$_]->{"flags"} = [0, $pmu_cpu_mask << 8 | 0x04];
	}
	die "could not find PMU interrupts" if !$nintrs;

	my $t = sprintf("pmu {\n");
	my $int_t = '';
	$t .= output_default(\%default);
	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= "};\n";
	$dt->add_node(DevTree::node->new($t));
}

sub add_sf2_mdio($$$$$)
{
	my ($dt, $rh, $top_base, $sf2_rev, $chipid) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string', [ sprintf('brcm,bcm%d-mdio-v%x.%x',
					$chipid, ($sf2_rev >> 8) & 0xff, $sf2_rev & 0xff),
					'brcm,unimac-mdio' ] ],
		"reg-names" => [ 'string' , [ 'mdio', 'mdio_indir_rw' ] ],
		"#address-cells" => [ 'dec' , 1 ],
		"#size-cells" => [ 'dec', 0 ],
	);
	my $t = '';
	my ($mdio_base, $mdio_base_size) =
		get_reg_range($rh, "BCHP_SWITCH_MDIO");
	my ($mdio_indir_base, $mdio_indir_size) =
		get_reg_range($rh, "BCHP_SWITCH_INDIR_RW");
	$mdio_base -= $top_base;
	$mdio_indir_base -= $top_base;

	my $unit = sprintf("mdio\@%x", $mdio_base);
	$t .= sprintf("mdio: %s {\n", $unit);
	insert_label("mdio", $unit);
	$t .= sprintf("reg = <0x%x 0x%x 0x%x 0x%x>;\n",
		$mdio_base, $mdio_base_size, $mdio_indir_base, $mdio_indir_size);
	$t .= output_default(\%default);
	$t .= "};\n";

	return $t;
}

sub output_switch_port_layout($$$)
{
	my ($dt, $port_layout, $label) = @_;
	my $t = '';
	my %default = (
		"#address-cells" => [ 'dec', 1 ],
		"#size-cells" => [ 'dec', 0 ],
	);
	my @keys = keys %$port_layout;
	my $count = 0;

	$t .= "$label {\n";

	# Old-style DSA binding
	if ($label eq "switch\@0") {
		$default{reg} = [ 'dec', [ 0, 0 ] ],
	}

	$t .= output_default(\%default);
	foreach (sort @keys) {
		if (defined $port_layout->{$_}) {
			my $port_label;

			if ($label eq "switch\@0") {
				$port_label = sprintf("switch_port%d", $_);
			} else {
				$port_label = sprintf("sw_port%d", $_);
			}
			my $port_unit = sprintf("port\@%x", $_);
			$t .= sprintf("%s: %s {\n", $port_label, $port_unit);
			insert_label($port_label, $port_unit);
			$t .= sprintf("reg = <%d>;\n", $_);
			$t .= sprintf("label = \"%s\";\n", $port_layout->{"$_"});

			# ASP port needs Broadcom tags
			if ($port_layout->{"$_"} eq "asp") {
				$t .= "brcm,use-bcm-hdr;\n";
			}

			# With the new binding, the CPU phandle is associated
			# with the port directly, not at the top-level, the old
			# one just ignores the property, so it does not hurt
			# adding it
			if ($port_layout->{"$_"} eq "cpu") {
				$t .= "ethernet = <&enet_0>;\n";
			}
			$t .= "};\n";

			add_reference_alias($dt, $port_label, $port_label);
		}
	}
	$t .= "};\n";
}

sub add_sf2($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $top_rev = BcmUtils::get_sf2_top_rev($bchp_defines);
	my $sf2_rev = BcmUtils::get_sf2_rev($bchp_defines);
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my %default = (
		"compatible" => [ 'string',
			[ sprintf('brcm,bcm%x-switch-v%d.%d',
					$chipid, ($sf2_rev >> 8) & 0xff, $sf2_rev & 0xff),
			sprintf('brcm,bcm%x', BcmUtils::get_sf2_model_id($bchp_defines)) ] ],
		"#address-cells" => [ 'dec', 2 ],
		"#size-cells" => [ 'dec', 0 ],
		"dsa,mii-bus" => [ 'phandle', 'mdio' ],
		"dsa,ethernet" => [ 'phandle', 'enet_0' ],
		"brcm,num-rgmii-ports", [ 'dec', BcmUtils::get_num_sf2_rgmii_ports($bchp_defines) ],
		"brcm,num-gphy", [ 'dec', BcmUtils::get_num_sf2_phys($bchp_defines) ],
		"brcm,num-acb-queues", [ 'dec', BcmUtils::get_num_sf2_acb_queues($bchp_defines) ],
	);
	my %top_default = (
		"compatible" => [ 'string', [ sprintf('brcm,bcm%x-switch-top-v%x.%x',
					$chipid, ($top_rev >> 8) & 0xff, $top_rev & 0xff),
				'simple-bus' ] ],
		"#address-cells" => [ 'dec', 1 ],
		"#size-cells" => [ 'dec', 1 ],
	);
	my @regs = ("CORE", "REG", "INTRL2_0", "INTRL2_1", "FCB", "ACB");
	my @intrs = ("SWITCH_0", "SWITCH_1");
	my @intr_prop;
	my ($base, $end);
	my ($reg, $reg_names);
	my $t .= '';
	my $i;

	$base = bphysaddr($rh, $bchp_defines->{"BCHP_SWITCH_" . $regs[0] . "_REG_START"});
	$end = bphysaddr($rh, $bchp_defines->{"BCHP_SWITCH_" . $regs[scalar(@regs) - 1] . "_REG_END"} - $base);

	$t .= sprintf("switch_top\@%x {\n", $base);
	$t .= output_default(\%top_default);
	$t .= sprintf("ranges = <0 0x%x 0x%x>;\n", $base, $end);

	$t .= sprintf("ethernet_switch: ethernet_switch\@%x {\n", $base - $base);
	$t .= output_default(\%default);
	for ($i = 0; $i < scalar(@regs); $i++) {
		my ($r_base, $r_size) = get_reg_range($rh, "BCHP_SWITCH_" . $regs[$i]);
		$reg .= sprintf("0x%x 0x%x", $r_base - $base, $r_size);
		$reg .= $i ne scalar(@regs) - 1 ? " " : "";
		$reg_names .= "\"" . lc($regs[$i]) . "\"";
		$reg_names .= $i ne scalar(@regs) - 1 ? ", " : "";
	}
	$t .= sprintf("reg = <%s>;\n", $reg);
	$t .= sprintf("reg-names = %s;\n", $reg_names);

	$reg = '';
	$reg_names = '';
	for ($i = 0; $i < scalar(@intrs); $i++) {
		my $intr = find_l1_interrupt($bchp_defines, $intrs[$i]);
		push @intr_prop, { "irq" => $intr, "name" => lc($intrs[$i]) };

	}
	$t .= output_interrupt_prop($rh, \@intr_prop);

	if (BcmUtils::get_sf2_has_pause_override($bchp_defines)) {
		$t .= "brcm,fcb-pause-override;\n";
	}
	if (BcmUtils::get_sf2_has_packets_inflight($bchp_defines)) {
		$t .= "brcm,acb-packets-inflight;\n";
	}

	# Port layout
	my %switch_port_layout = (
		0 => "gphy",
		1 => "rgmii_1",
		2 => "rgmii_2",
		8 => "cpu",
	);

	$chipid = sprintf("%x", $chipid);

	if ($chipid eq "7445") {
		$switch_port_layout{7} = "moca";
	} else {
		$switch_port_layout{5} = "wifi";
		$switch_port_layout{7} = "asp";
	}

	# Do it once for the old binding
	$t .= output_switch_port_layout($dt, \%switch_port_layout, "switch\@0");
	# and another time with the new binding
	$t .= output_switch_port_layout($dt, \%switch_port_layout, "ports");

	$t .= "};\n";

	$t .= add_sf2_mdio($dt, $rh, $base, $sf2_rev, $chipid);

	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_memc($$$$)
{
	my ($dt, $rh, $memc_idx, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my %default = (
		"compatible" => [ 'string', [ 'brcm,brcmstb-memc', 'simple-bus' ] ],
		"#address-cells" => [ 'dec', 1 ],
		"#size-cells" => [ 'dec', 1 ],
	);
	my $t = '';

	override_default(\%default, $info);

	$t .= sprintf("memc@%d {\n", $memc_idx);
	$t .= output_default(\%default);
	$t .= "ranges;\n";
	$t .= "};\n";

	my $node = DevTree::node->new($t);
	$dt->add_node($node);

	add_memc_gen($node, $rh, $memc_idx, $info);
	add_memc_arb($node, $rh, $memc_idx, $info);
	add_ddr_phy($node, $rh, $memc_idx, $info);
	add_ddr_shim_phy($node, $rh, $memc_idx, $info);
	add_memc_ddr($node, $rh, $memc_idx, $info);
}

sub add_memcs($$$$)
{
	my ($dt, $rh, $n_memc, $info) = @_;
	my $i;
	my $t = '';

	$t .= "memory_controllers {\n";
	$t .= "ranges;\n";
	$t .= "compatible = \"simple-bus\";\n";
	$t .= "#address-cells = <1>;\n";
	$t .= "#size-cells = <1>;\n";
	$t .= "};\n";
	my $node = DevTree::node->new($t);
	$dt->add_node($node);

	# Each memc is a child
	for ($i = 0; $i < $n_memc; $i++) {
		add_memc($node, $rh, $i, $info);
	}
}



sub add_nexus_wakeups($$$)
{
	my ($dt, $rh, $l2_intr) = @_;

	my $parent = $l2_intr->{"-parent"}[0];
	my $uses = $l2_intr->{"-uses"}[0];

	return if (!defined($parent));

	die "$P: " . $l2_intr->{__cfg_where__} . ": '-uses' param is required.\n"
		if !defined($uses);

	my $bchp_defines = $rh->{rh_defines};
	my @bit = @{$l2_intr->{"-uses"}};

	my $t = "nexus-wakeups {\n";
	my @intr_prop;

	foreach my $b (@bit) {

		$b =~ s/^\s+|\s+$//g; # trim spaces

		my $intr = __find_l2_interrupt($bchp_defines, uc($parent), $b);

		if (defined($intr)) {
			push @intr_prop, { "name" => lc($b),
					   "irq" => $intr,
					   "parent_intc" => $parent . "_intc" };
		}
	}

	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub __add_nexus_irq0($$$)
{
	my ($dt, $rh, $parent) = @_;

	return if !defined($parent);
	my $bchp_defines = $rh->{rh_defines};

	my $t = "nexus-$parent {\n";
	my $regex = '(?=^((?!reserved).)*$)BCHP_' . uc($parent) . '_IRQEN_[A-Za-z_0-9]*_irqen_SHIFT';
	my $l2_names = BcmUtils::get_rdb_fields($regex, $bchp_defines);
	my ($l2_map, $l2_map_irq);
	my $num_l2 = scalar @$l2_names;
	my @intr_prop;
	my $i = 0;
	my $sep;

	foreach my $name (@{$l2_names}) {
		my $patt = "BCHP_" . uc($parent) . "_IRQEN_";
		$name =~ s/$patt//;
		$name =~ s/_irqen_SHIFT//;
		$l2_map = BcmUtils::get_l2_irq0_intc_internal($parent, $name);
		$l2_map_irq = find_l2_irq0_interrupt($bchp_defines, $parent, $name);
		push @intr_prop, { "irq" => $l2_map_irq,
				  "name" => $name,
				  "parent_intc" => $parent . "_intc" };
	}

	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_nexus_irq0($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq0($dt, $rh, "irq0");
}

sub add_nexus_irq0_aon($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq0($dt, $rh, "irq0_aon");
}

sub __add_nexus_irq($$$)
{
	my ($dt, $rh, $parent) = @_;

	return if !defined($parent);
	my $bchp_defines = $rh->{rh_defines};

	my $t = "nexus-$parent {\n";
	my $regex = '(?=^((?!reserved).)*$)BCHP_' . uc($parent) . '_CPU_STATUS_[A-Za-z_0-9]*_SHIFT';
	my $l2_names = BcmUtils::get_rdb_fields($regex, $bchp_defines);
	my $intr;
	my $num_l2 = scalar @$l2_names;
	my @intr_prop;

	foreach my $name (@{$l2_names}) {
		my $patt = "BCHP_" . uc($parent) . "_CPU_STATUS_";
		$name =~ s/$patt//;
		$name =~ s/_SHIFT//;
		$intr = find_l2_interrupt($bchp_defines, uc($parent), $name);
		push @intr_prop, { "irq" => $intr,
				   "name" => $name,
				   "parent_intc" => $parent . "_intc" };
	}

	$t .= output_interrupt_prop($rh, \@intr_prop);
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_nexus_upg_main_irq($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq($dt, $rh, "upg_main_irq");
}

sub add_nexus_upg_main_aon_irq($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq($dt, $rh, "upg_main_aon_irq");
}

sub add_nexus_upg_bsc_irq($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq($dt, $rh, "upg_bsc_irq");
}

sub add_nexus_upg_bsc_aon_irq($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq($dt, $rh, "upg_bsc_aon_irq");
}

sub add_nexus_upg_spi_aon_irq($$$)
{
	my ($dt, $rh, $info) = @_;

	__add_nexus_irq($dt, $rh, "upg_spi_aon_irq");
}

sub add_rf4ce($$$$$$)
{
	my ($dt, $rh, $info, $intr_name, $l2_intr, $l2_bit) = @_;
	my $t = "";
	my $macver = 0;

	my $bchp_defines = $rh->{rh_defines};

	$macver = $bchp_defines->{"BCHP_MAC_CORE_ID_CORE_ID_DEFAULT"}
		if defined $bchp_defines->{"BCHP_MAC_CORE_ID_CORE_ID_DEFAULT"};

	my %default = (
		"compatible" => [ 'string',
			[ "brcm,rf4ce-v$macver", "brcm,rf4ce"]],
		"#address-cells" => [ 'dec', 1 ],
		"#size-cells" => [ 'dec', 1 ],
	);

	my $base = bphysaddr($rh, $bchp_defines->{"BCHP_RF4CE_CPU_PROG0_MEM_REG_START"});
	my $size = bphysaddr($rh, $bchp_defines->{"BCHP_RF4CE_CPU_HOST_RG_L2_REG_END"} - $base);

	#  Check that the interrupt bits exist before we hard substitute
	# L1 with "rf4ce" and L2 with "rf4ce_aon"
	# in the interrupt-names property.
	#
	my $intr = find_l1_interrupt($bchp_defines, uc($intr_name));

	my $intr2 = find_l2_interrupt($bchp_defines, uc($l2_intr), uc($l2_bit));
	die "rf4ce: bad L2 interrupt reg or name: " . uc($l2_intr) . " " . uc($l2_bit)
		if !defined $intr2;

	my $unit = sprintf("rf4ce\@%x", $base);
	$t .= sprintf("rf4ce_controller: %s {\n", $unit);
	insert_label("rf4ce_controller", $unit);
	$t .= output_default(\%default);

	$t .= sprintf("reg = <0x%x 0x%x>;\n", $base, $size + 4);
	$t .= sprintf("interrupt-names = \"rf4ce\", \"rf4ce_aon\";\n");
	$t .= sprintf("interrupts-extended = <&intc 0x%x 0x%x 0x%x>,\n", 0, $intr, 0);
	$t .= sprintf("<&%s 0x%x>;\n", $l2_intr . "_intc", $intr2);

	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
}

sub add_gpio($$$)
{
	my ($dt, $rh, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);

	my %default = (
		"compatible" => [ 'string',
			[ sprintf('brcm,bcm%x-gpio', $chipid),
			  'brcm,brcmstb-gpio' ]],
		"#gpio-cells" => [ 'dec', 2 ],
		"#interrupt-cells" => [ 'dec', 2 ],
	);

	foreach my $gio (("gio", "gio_aon")) {
		my ($reg_first, $bank_size) =
			get_reg_range($rh, "BCHP_" . uc($gio));
		my $intr_name = 'upg_' . $gio;
		my ($intr, $parent_phandle, $ext_intr, $ext_parent_phandle);
		my $ext_t = '';
		my @intr_prop;

		if (index($intr_name, 'aon') != -1) {
			$intr = __find_l2_irq0_interrupt($bchp_defines,
							 "IRQ0_AON", "gio");
			$parent_phandle = "irq0_aon_intc";
			if (!defined($intr)) {
			  $intr = __find_l2_interrupt($bchp_defines,
						      "UPG_MAIN_AON_IRQ", "gio");
			  $parent_phandle = "upg_main_aon_irq_intc";
			}
			push @intr_prop, { "irq" => $intr,
					   "name" => "upg_gio_aon",
					   "parent_intc" => $parent_phandle };
			$ext_t .= "always-on;\n";
		} else {
			$intr = __find_l2_irq0_interrupt($bchp_defines,
							 "IRQ0", "gio");
			$parent_phandle = "irq0_intc";
			if (!defined($intr)) {
			  $intr = __find_l2_interrupt($bchp_defines,
						      "UPG_MAIN_IRQ", "gio");
			  $parent_phandle = "upg_main_irq_intc";
			}
			push @intr_prop, { "irq" => $intr,
					   "name" => "upg_gio",
					   "parent_intc" => $parent_phandle };
		}
		die "$P: cannot find " . $gio . " level-2 interrupt"
			if !defined $intr;
		$ext_intr = find_l2_interrupt($bchp_defines,
			"AON_PM_L2", "GPIO");
		die "$P: cannot find " . $gio . " level-2 interrupt"
			if !defined $ext_intr;
		push @intr_prop, { "irq" => $ext_intr,
				   "name" => "upg_gio_aon_wakeup",
				   "parent_intc" => "aon_pm_l2_intc" };
		$ext_t .= "wakeup-source;\n";

		# Put together list of GPIO bank widths in physical address
		# order
		my @reg_names = get_reg_names_from_regexp($rh, 'BCHP_' .
			uc($gio) . '_ODEN_[A-Z_]*[0-9]?$');
		@reg_names = sort {bphysaddr($rh, $bchp_defines->{$a}) <=>
			bphysaddr($rh, $bchp_defines->{$b})} @reg_names;
		my $bank_widths = join(' ',
			map { BcmUtils::count_set_bits(
			       get_reg_valid_mask($bchp_defines, $_)) } @reg_names);

		my $label = sprintf("upg_%s", $gio);
		my $unit = sprintf("gpio\@%x", $reg_first);
		my $t = sprintf("%s: %s {\n", $label, $unit);
		insert_label($label, $unit);
		$t .= output_default(\%default);
		$t .= "gpio-controller;\n";
		$t .= "interrupt-controller;\n";
		$t .= sprintf("reg = <0x%x 0x%x>;\n", $reg_first, $bank_size);
		$t .= output_interrupt_prop($rh, \@intr_prop);
		$t .= $ext_t;
		$t .= sprintf("brcm,gpio-bank-widths = <%s>;\n", $bank_widths);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));
		add_reference_alias($dt, $intr_name, $intr_name);
	}
}

sub add_watchdog($$$$)
{
	my ($dt, $rh, $info, $clks_file) = @_;
	my $bchp_defines = $rh->{rh_defines};

	foreach $b (@{$info->{"-type"}}) {
		my %default = (
			"compatible" => "brcm,bcm7038-wdt",
		);
		my ($base, $size, $wdtnum, $t);
		if ($b eq "upg") {
			$base = bphysaddr($rh,
				$bchp_defines->{"BCHP_TIMER_WDTIMEOUT"});
			$size = bphysaddr($rh,
				$bchp_defines->{"BCHP_TIMER_WDCTRL"} - $base);
			$wdtnum = 0;
		} elsif ($b eq "webhif") {
			$base = bphysaddr($rh,
				$bchp_defines->{"BCHP_WEBHIF_TIMER_WDTIMEOUT"});
			$size = bphysaddr($rh,
				$bchp_defines->{"BCHP_WEBHIF_TIMER_WDCTRL"} - $base);
			$wdtnum = 1;
		} elsif ($b eq "hyper") {
			$base = bphysaddr($rh,
				$bchp_defines->{"BCHP_WATCHDOG_WDTIMEOUT"});
			$size = bphysaddr($rh,
				$bchp_defines->{"BCHP_WATCHDOG_WDCTRL"} - $base);
			$wdtnum = 2;
		}

		my $label = sprintf("watchdog_timer%d", $wdtnum);
		my $unit = sprintf("watchdog\@%x", $base);
		$t = sprintf("%s: %s {\n", $label, $unit);
		insert_label($label, $unit);

		if (-e $clks_file) {
			$default{"clocks"} = [ 'phandle', 'upg_fixed'];
		} else {
			$default{"clock-frequency"} = [ 'dec', 27000000 ];
		}
		$t .= output_default(\%default);
		$t .= sprintf("reg = <0x%x 0x%x>;\n", $base, $size);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));
		add_reference_alias($dt, sprintf("watchdog%d", $wdtnum),
					sprintf("watchdog_timer%d", $wdtnum));
	}
}


sub add_bsp($$$)
{
	my ($dt, $rh, $info) = @_;
	my $t = "";

	my $bchp_defines = $rh->{rh_defines};

	my %default = (
		"compatible" => "brcm,bsp",
		"#address-cells" => [ 'dec', 1 ],
		"#size-cells" => [ 'dec', 1 ],
	);

	my $base = bphysaddr($rh, $bchp_defines->{"BCHP_BSP_CMDBUF_REG_START"});
	my $end;
	if (defined($bchp_defines->{"BCHP_BSP_OTP_SCRATCH_REG_END"})) {
		# 28nm style BSP
		$end = bphysaddr($rh, $bchp_defines->{"BCHP_BSP_OTP_SCRATCH_REG_END"});
	} elsif (defined($bchp_defines->{"BCHP_BSP_CONTROL_INTR2_REG_END"})) {
		# Late 40nm style BSP (e.g: 7435)
		$end = bphysaddr($rh, $bchp_defines->{"BCHP_BSP_CONTROL_INTR2_REG_END"});
	} else {
		# 40nm style BSP
		$end = bphysaddr($rh, $bchp_defines->{"BCHP_BSP_INST_PATCHRAM_REG_END"});
	}
	my $size = $end - $base;

	my $unit = sprintf("bsp\@%x", $base);
	$t .= sprintf("security_processor: %s {\n", $unit);
	insert_label("security_processor", $unit);
	$t .= output_default(\%default);
	$t .= sprintf("reg = <0x%x 0x%x>;\n", $base, $size + 4);

	# Parent and child address spaces are identical.
	# (devicetree.org/Device_Tree_Usage#Ranges_.28Address_Translation.29)
	$t .= "ranges;\n";
	$t .= "};\n";

	$dt->add_node(DevTree::node->new($t));
	add_reference_alias($dt, "bsp", "security_processor");
}

sub add_cpu_clock($$)
{
	my ($dt, $info) = @_;
	my $t = "";

	die "missing '-clocks' property" if !$info->{"-clocks"};

	my %default = (
		"clocks" => [ 'phandle', '' ],
		"clock-latency" => [ 'dec', 0 ],
	);

	override_default(\%default, $info);

	my $cpus = $dt->add_node(DevTree::node->new("cpus { };"));
	$t .= 'cpu@0 {';
	$t .= output_default(\%default);
	$t .= "};\n";

	$cpus->add_node(DevTree::node->new($t));
}

sub add_pwm($$$$$)
{
	my ($dt, $rh, $n, $info, $clks) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $i;

	my %defaults = (
		"compatible" => [ 'string', [ "brcm,bcm7038-pwm", "brcm,brcmstb-pwm" ]],
		"#pwm-cells" => [ 'dec', 2 ],
	);

	if ($clks) {
		$defaults{"clocks"} = [ 'phandle', 'upg_fixed' ];
	} else {
		$defaults{"clock-frequency"} = [ 'dec', 27000000 ];
	}

	for ($i = 0; $i < $n; $i++) {
		my $letter = $i == 0 ? "" : chr($i + ord('A'));
		my $t = "";

		my $base = bphysaddr($rh,
			$bchp_defines->{"BCHP_PWM".$letter."_REG_START"});
		my $size = bphysaddr($rh,
			$bchp_defines->{"BCHP_PWM".$letter."_REG_END"}) - $base + 4;

		my $label = sprintf("pwm%d", $i);
		my $unit = sprintf("pwm\@%x", $base);
		$t .= sprintf("%s: %s {\n", $label, $unit);
		$labels{$label} = $unit;
		insert_label($label, $unit);
		$t .= output_default(\%defaults);
		$t .= sprintf("reg = <0x%x 0x%x>;\n", $base, $size);
		$t .= "};\n";

		$dt->add_node(DevTree::node->new($t));
	}
}

sub add_wlan($$$$$)
{
	my ($dt, $rh, $n, $info, $l2_intr) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);

	my %defaults = (
		"compatible" => [ 'string', [ sprintf("brcm,bcm%x-wlan", $chipid) ]],
	);
	my ($base, $size) = get_reg_range_from_regexp($rh, "^BCHP_WLAN");
	my $intr_name = "WLAN_D2H";
	my @intr_prop = (
		# L1 interrupt
		{ "irq" => find_l1_interrupt($bchp_defines, $intr_name),
		  "name" => $intr_name,
		},
		# Wake-on-LAN/AON interrupt
		{
		  "irq" => find_l2_interrupt($bchp_defines, uc($l2_intr), "WLAN"),
		  "name" => "wlan_wol",
		  "parent_intc" => uc($l2_intr)."_intc",
		},
	);
	my $t = "";

	for (my $i = 0; $i < $n; $i++) {
		my $label = sprintf("wlan%d", $i);
		my $unit = sprintf("wlan\@%x", $base);
		$t .= sprintf("%s: %s {\n", $label, $unit);
		$t .= output_default(\%defaults);
		$t .= sprintf("reg = <0x%x 0x%x>;\n", $base, $size);
		$t .= output_interrupt_prop($rh, \@intr_prop);
		$t .= "};\n";

		add_reference_alias($dt, $label, $label);

		$dt->add_node(DevTree::node->new($t));
	}
}

sub add_v3d_mmu($$$$)
{
	my ($dt, $rh, $n, $info) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my %defaults = (
		"compatible" => [ 'string',
		[ sprintf("brcm,bcm%x-v3d", $chipid),
		sprintf("brcm,v3d-v%d.%d.%d.%d",
		$bchp_defines->{BCHP_V3D_HUB_CTL_IDENT1_TVER_DEFAULT},
		$bchp_defines->{BCHP_V3D_HUB_CTL_IDENT1_REV_DEFAULT},
		$bchp_defines->{BCHP_V3D_HUB_CTL_IDENT3_IPREV_DEFAULT},
		0) ]]
	);
	my $t = "";

	for (my $i = 0; $i < $n; $i++) {
		my ($base, $size) = get_reg_range($rh, "BCHP_V3D_MMUC");
		my $label = sprintf("gpu_mmu%d\n", $i);
		my $unit = sprintf("gpu-mmu\@%x\n", $base);
		$t .= sprintf("%s: %s {\n", $label, $unit);
		$t .= output_default(\%defaults);
		$t .= "};\n";

		add_reference_alias($dt, $label, $label);

		$dt->add_node(DevTree::node->new($t));
	}
}

sub add_dtu_map($$$)
{
	my ($dt, $rh, $num) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my %default = (
		"compatible" => [ 'string',
			[
				sprintf("brcm,bcm%x-memc-dtu-map", $chipid),
				sprintf("brcm,memc-dtu-map-v%d.%d.%d.%d",
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_MAJOR_REV_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_MINOR_REV_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_ALL_LAYER_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_METAL_LAYER_ID_DEFAULT}),
				'brcm,brcmstb-memc-dtu-map',
				'syscon' ]
			],
	);


	for (my $i = 0; $i < $num; $i++) {
		my ($reg, $size)
			= get_reg_range($rh, "BCHP_MEMC_DTU_MAP_STATE_$i");

		$default{'reg'} = [ 'hex' , [ $reg, $size ] ];

		my $t = sprintf("memc_dtu_map$i: syscon\@%x {\n", $reg);
		$t .= output_default(\%default);
		$t .= "};";

		$dt->add_node(DevTree::node->new($t));
	}
}

sub add_dtu_config($$$)
{
	my ($dt, $rh, $num) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);
	my %default = (
		"compatible" => [ 'string',
			[
				sprintf("brcm,bcm%x-memc-dtu-config", $chipid),
				sprintf("brcm,memc-dtu-config-v%d.%d.%d.%d",
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_MAJOR_REV_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_MINOR_REV_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_ALL_LAYER_ID_DEFAULT},
					$bchp_defines->{BCHP_MEMC_DTU_CONFIG_0_REV_ID_METAL_LAYER_ID_DEFAULT}),
				'brcm,brcmstb-memc-dtu-config',
				'syscon' ]
			],
	);


	for (my $i = 0; $i < $num; $i++) {
		my ($reg, $size)
			= get_reg_range($rh, "BCHP_MEMC_DTU_CONFIG_$i");

		$default{'reg'} = [ 'hex' , [ $reg, $size ] ];

		my $t = sprintf("memc_dtu_config$i: syscon\@%x {\n", $reg);
		$t .= output_default(\%default);
		$t .= "};";
		$dt->add_node(DevTree::node->new($t));
	}
}

sub add_dpfe($$$)
{
	my ($dt, $rh, $num) = @_;
	my $bchp_defines = $rh->{rh_defines};
	my $chipid = BcmUtils::get_chip_family_id($bchp_defines);

	for (my $i = 0; $i < $num; $i++) {
		my @regs;
		my @reg_names;

		foreach my $node (qw(cpu dmem imem)) {
			push(@regs, get_reg_range($rh,
				"BCHP_DPFE_".uc($node)."_$i"));
			push(@reg_names, "dpfe-$node");
		}
		my %default = (
			"compatible" => [ 'string',
				[
					sprintf("brcm,bcm%x-dpfe-cpu", $chipid),
						"brcm,dpfe-cpu" ]
				],
			"reg" => [ 'hex', \@regs ],
			"reg-names" => [ 'string', \@reg_names ],
		);

		# $regs[0] is the address of the DPFE CPU register space
		my $t = sprintf("dpfe_cpu$i: dpfe-cpu\@%x {\n", $regs[0]);
		$t .= output_default(\%default);
		$t .= "};";
		$dt->add_node(DevTree::node->new($t));
	}
}

###############################################################
# FUNCTION:
#   gen_clocks_prop
# PARAMS:
#   $rh_funcs -- hashref of functions; each value is an arrayref
#       of clock names.
#   $ra_funcs -- arrayref the function names of interest (eg ['NAND']).
#   $re_dev - the regular expression of the device.  This is
#       optional and only used for special hacks.
# DESCRIPTION:
#    For a given function, generates the 'clock' and 'clock-names'
#    properties to be inserted into devices.  This function is
#    only invoked by function insert_clocks_prop_into_devs().
# RETURNS:
#   hashref containing keys 'clk', 'clk_names'.  The values are the
#   property strings for properties 'clock' and 'clock-names'. An
#   example:
#       { clks => 'clocks = <&usb1_27_mdio_ck>, <&usb1_freerun_ck>',
#         clk_names => 'clock-names = "usb0", "usb1"';
#       }
sub gen_clocks_prop
{
	my ($rh_funcs, $ra_funcs, $re_dev) = @_;
	my @funcs = @$ra_funcs;
	my @f;
	foreach (@funcs) {
		push @f, @{$rh_funcs->{$_}};
	}
	return undef if !@f;
	my @a = map { "\<\&$_\>" } @f;
	my @b = map { "\"$_\"" } @f;
	my $str0 = "clocks = " . join(", ", @a) . ";";
	my $str1 = "clock-names = " . join(", ", @b) . ";";

	# change genet clock names to generic
	$str1 =~ s/\b(sw_genet(?:|mux|wol|eee))\d\b/$1/g;

	# change usb clock names to generic.  For the case of
	# usb-phy, do some ad hoc renaming.
	if ($re_dev && $re_dev eq qr/^usb-phy\@/) {
		$str1 =~ s/\b(sw_usb)2\d*\b/$1/g;
		$str1 =~ s/\b(sw_usb3)\d*\b/$1/g;
	} else {
		$str1 =~ s/\b(sw_usb)\d+\b/$1/g;
	}

	# change pcie clock names to generic
	$str1 =~ s/\b(sw_pcie)\d+\b/$1/g;
	# change sata30 => sata3
	$str1 =~ s/\b(sw_sata3)\d+\b/$1/g;

	# change sw_uart0 => sw_baud
	$str1 =~ s/\bsw_uart\d\b/sw_baud/g;
	
	# change sysport[01] => sysport
	$str1 =~ s/\b(sw_sysport)\d+\b/$1/g;
	$str1 =~ s/\b(sw_sysport)\d+(wol)\b/$1$2/g;

	my $clks = DevTree::prop->new($str0);
	my $clock_names = DevTree::prop->new($str1);
	return { clks => $clks, clock_names => $clock_names };
}

###############################################################
# FUNCTION:
#
###############################################################
sub insert_clk_frequency_into_uarts($)
{
    my ($dt) = @_;
    my $count = 0;

    my $text = "clock-frequency = <81000000>;";

    foreach my $uart (@serial_names) {
	my $re = qr/$uart/;
	my ($x) = $dt->find_node($re);
	next if !$x;
	$x->add_prop(DevTree::prop->new($text));
	$count++;
    }
    return $count;
}
    

###############################################################
# FUNCTION:
#  get_devs
# PARAMS:
#   $dt -- the device tree which we will be inserting things.
#   $re -- regular expression of the device.
#   $depth -- optional integer that indicates the depth
#       to search for $re1.  If this is not specified, the
#       depth is assumed unlimited.
# RETURNS:
#   list of matching devices.
###############################################################
sub get_devs($$$)
{
	my ($dt, $re, $depth) = @_;
	return $dt->find_node($re, $depth);
}
    

###############################################################
# FUNCTION:
#   insert_clocks_prop_into_devs
# PARAMS:
#   $dt -- the device tree which we will be inserting things.
#   $rh_funcs -- hashref of functions; each value is an arrayref
#       of clock names.
#   $mode -- describes how to distribute the functions when
#       the number of functions does not equal the devices.
#       Values are: 'none', 'grow', 'interleave', 'partition'.
#       See the code comments for the precise meanings.
#   $re0 -- the regexp that describes the functions of interest.
#       Can also be an array of regexps; each regexp will be
#       evaluated and the results will be collected.
#   $re1 -- the regexp that describes the device name of interest.
#   $re2 -- the regexp that furthe describes the device name of
#       interest.  This is only used by USB devices so far
#       and is optional.
#   $depth -- optional integer that indicates the depth
#       to search for $re1.  If this is not specified, the
#       depth is assumed unlimited.
# DESCRIPTION:
#   Generates the list of functions from $re0.  Generates the
#   list of devices from $re1 (and optionally $re2).  If thee
#   number of functions does not equal the number of devices,
#   we duplicate the number of functions with a method indicated
#   by $mode.  Afterwards, we go through each device and add
#   two properties to it: clock, clock-names.
# RETURNS:
#   undef
sub insert_clocks_prop_into_devs
{
	my ($dt, $rh_funcs, $mode, $re0_in, $re1, $re2, $depth) = @_;
	my $num_inserted = 0;

	die if $mode !~ /^(none|grow|interleave|partition)$/;

	my $re0_arr = "ARRAY" eq ref($re0_in) ? $re0_in : [ $re0_in ];

	# Find all of the functions that match the regexp array $re0_arr.
	my @funcs;
	foreach my $func (keys %$rh_funcs) {
		foreach my $re0 (@$re0_arr) {
			push @funcs, $func
				if ($func =~ $re0);
        }
	}
	@funcs = sort @funcs;

	# If there are no such functions, we are done (example:
	# looking for GENET in a chip that uses switch).
	goto OUT if !scalar(@funcs);

	# Find the devices in the clock tree that we want.
	my @tmp_devs = $dt->find_node($re1, $depth);
	my @devs;
	if ($re2) {
		foreach my $dev (@tmp_devs) {
			# So far, only usb hits this code.
			my @sub_devs = $dev->find_node($re2);
			push @devs, @sub_devs;
		}

	} else {
		@devs = @tmp_devs;
	}

	# We are done if there are no devices we seek
	goto OUT if !scalar(@devs);

	my $nf = scalar(@funcs);
	my $nd = scalar(@devs);
	my $n = ($nd > $nf) ? $nd/$nf : $nf/$nd;
	die sprintf("nf=$nf, nd=$nd, internal error int(%s) != %s", int($n), $n)
		if (int($n) != $n);
	my @props;

	if ($mode eq 'none') {
		# Simple one-to-one mapping of funcs and devices.
		die 'internal error' if ($n != 1);
		@props = map { gen_clocks_prop($rh_funcs, [$_]) } @funcs;

	} elsif ($mode eq 'grow') {
		# There are more devices than funcs.  Each function
		# in @funcs will repeat itself $n times; ie if @funcs = (f0, f1)
		# and $n is 2, then @funcs will grow to (f0, f0, f1, f1).
		# eg USB ehci/ohci will follow this.
		die 'internal error: more funcs than devices' if $nd < $nf;
		my @a = @funcs;
		@funcs = ();
		push @funcs, ($_) x $n
			foreach (@a);
		@props = map { gen_clocks_prop($rh_funcs, [$_]) } @funcs;

	} elsif ($mode eq 'interleave') {
		# There are more functions than devices.  We assign the
		# functions by interleaving.  For example, if @devs = (d0, d1)
		# and @func = (f0 f1 f2 f3), then d0 will get (f0, f2) and
		# d1 will get (f1, f3).  Genet uses this.
		die 'internal error' if ($nf < $nd);
		for (my $i=0; $i<$nd; $i++) {
			my @a;
			for (my $j=0; $j<$n; $j++) {
				push @a, $funcs[$nd*$j + $i];
			}

			# Hack for usb-phy.
			if ($re1 eq qr/^usb-phy\@/) {
				my $missing_xhci = !$devs[$i]->find_prop('has_xhci');
				# drop the sw_usb30 if we don't have xhci.
				pop @a
					if ($missing_xhci && $a[-1] =~ /^USB3/);
			}

			my $x = gen_clocks_prop($rh_funcs, [@a], $re1);
			push @props, $x;
		}
        
	} elsif ($mode eq 'partition') {
		# There are more functions than devices.  We assign the
		# functions by partitioning.  For example, if @devs = (d0, d1)
		# and @func = (f0 f1 f2 f3), then d0 will get (f0, f1) and
		# d1 will get (f2, f3).
		die 'internal error' if ($nf < $nd);
		for (my $i=0; $i<$nd; $i++) {
			my @a;
			for (my $j=0; $j<$n; $j++) {
				push @a, $funcs[$nd*$i + $j];
			}
			push @props, gen_clocks_prop($rh_funcs, [@a]);
		}
	}

	while (@devs) {
		my $d = shift @devs;
		my $prop = shift @props;
		my $clks = $prop->{clks};
		my $clock_names = $prop->{clock_names};
		if (!$clks || !$clock_names || !$d) {
			die "internal error";
		}
		$d->add_prop($clks);
		$d->add_prop($clock_names);
		$num_inserted++;
	}
OUT:
	return $num_inserted;
}


###############################################################
# FUNCTION:
#   merge_clocks_file
# PARAMS:
#   $bchp_defines -- hashref of BCHP constants and values
#   $dt -- the device tree which we will be inserting things.
#   $file -- Perl Data::Dumper file which describes the clock
#       dependency tree.
# DESCRIPTION:
#   Reads a file produced by Data::Dumper into a hashref.  The
#   hashref has some noteworthy keys:
#     o version -- the version of clkgen.pl that created the file.
#     o clks -- the DT-compatible string of clock definitions.
#     o funcs -- a hashref of the power "functions".  Each function
#         has a name, and an associated arrayref which is a list
#         of the sw clocks that comprise that function.  For example,
#         here is a dump of the old but backwards compatible funcs
#         structure:
#
#          'funcs' => {
#                       'MEMSYS1' => [
#                                      'memsys1_gisb_ck',
#                                      'memsys1_scb_ck'
#                                    ],
#                       'GENET2' => [
#                                     'genet2_sys_fast_ck',
#                                     'genet2_sys_slow_ck',
#                                     'genet2_ck_250_ck_genet2',
#                                     'genet2_eee_ck_genet2',
#                                     'genet2_gisb_ck_genet2',
#                                     'genet2_gmii_ck_genet2',
#                                     'genet2_hfb_ck_genet2',
#                                     'genet2_l2intr_ck_genet2',
#                                     'genet2_scb_ck_genet2',
#                                     'genet2_umac_sys_rx_ck_genet2',
#                                     'genet2_umac_sys_tx_ck_genet2',
#                                     'net_pll_pst_div_hld_ch1'
#                                   ],
#                        ...
#
#          Here is a dump of the new funcs structure which uses
#          "software clocks":
#
#           'funcs' => {
#                        'MEMSYS1' => [
#                                       'sw_memsys1'
#                                     ],
#                        'GENET2' => [
#                                      'sw_genet2'
#                                    ],
#                        'USB20' => [
#                                     'sw_usb20'
#                                   ],
#                        ...
#
#
#   The whole purpose of this function is to take the clock dependency
#   tree information and match up the right clock functions with
#   the right devices.  Of course, this is done by modifying the
#   device tree.
#
# RETURNS:
#   undef
sub merge_clocks_file($$$)
{
	my ($bchp_defines, $dt, $file) = @_;
	my $rh = do $file
		or die "bad or missing Perl Dumper file '$file'";
	my $ni;
	my $bd = $bchp_defines;
	my $rh_funcs = $rh->{funcs};
	my ($rdb) = $dt->find_node(qr/^rdb$/);
	my $clocks_prop;
	my (@a, @p);

	# Add the brcm-clocks node and its many subnodes.
	my $str = $rh->{clks};

	# Insert sw_scb skeleton, so we can reference it by phandle.
	my $sw_scb =<<'STOP';
		sw_scb : sw_scb {
			compatible = "fixed-clock";
			#clock-cells = <0x0>;
		};
STOP

	# Insert a synthetic clock
	my $upg_clock =<<'STOP';
		upg_fixed : upg_fixed {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <27000000>;
		};
STOP

	my $injected_clocks = "$upg_clock\n$sw_scb";

	# Insert the injected clocks right after the 'ranges;'
	$str =~ s/ranges;\n/ranges;\n\n$injected_clocks/sm;

	my $clks = DevTree::node->new($str);
	$rdb->add_node($clks);

	# Now insert the clock property for the devices.

	# GENET
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'interleave',
		qr/^GENET\w*\d$/, qr/^ethernet\@/);
	print "$P: WARN: no clocks inserted for genet!\n"
		if (!$ni && BcmUtils::get_num_genet($bd));

	# MOCA
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'partition',
		qr/^MOCA/, qr/^bmoca\@/);
	print "$P: WARN: no clocks inserted for moca!\n"
		if (!$ni && BcmUtils::get_num_moca($bd));

	# MPI
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^MPI$/, qr/^mpi\@/);

	# PCIE
	$ni = insert_clocks_prop_into_devs($dt, $rh_funcs, 'none',
		qr/^PCIE\d?$/, qr/^pcie\@/);
	print "$P: WARN: no clocks inserted for pcie!\n"
		if (!$ni && BcmUtils::get_num_pcie($bd));

	# SATA3
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^SATA3\d?$/, qr/^sata\@/);
	print "$P: WARN: no clocks inserted for SATA!\n"
		if (!$ni && BcmUtils::get_num_sata($bd));

	# SDIO
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^SDIO$/, qr/^sdhci\@/);
	print "$P: WARN: no clocks inserted for SDIO!\n"
		if (!$ni && BcmUtils::get_num_sdio($bd));

	# SPI
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^SPI$/, qr/^spi\@/);
	print "$P: WARN: no clocks inserted for SPI!\n"
		if (!$ni && BcmUtils::get_num_spi($bd));

	# QSPI
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^SPI$/, qr/^qspi\@/);
	print "$P: WARN: no clocks inserted for QSPI!\n"
		if (!$ni && BcmUtils::get_num_spi($bd));

	# SYSTEMPORT
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'partition',
		qr/^SYSPORT/, qr/^ethernet\@/)
		if $rh_funcs->{SYSPORT} && !$rh_funcs->{SYSPORT0};
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'interleave',
		qr/^SYSPORT0\w*?$/, qr/^ethernet\@/) if $rh_funcs->{SYSPORT0};

	print "$P: WARN: no clocks inserted for systemport!\n"
		if (!$ni && BcmUtils::get_num_systemport($bd));

	# SWITCH
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'partition',
		qr/^SWITCH/, qr/^ethernet_switch\@/);
	print "$P: WARN: no clocks inserted for switch!\n"
		if (!$ni && BcmUtils::get_num_sf2_switch($bd));

	# UART
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^UART/, qr/^serial/);
	if ($ni) {
	    print "$P: WARN: no clocks inserted for uart!\n"
		if (!$ni && BcmUtils::get_num_serial($bd));
	} else {
	    $ni = insert_clk_frequency_into_uarts($rdb);
	    print "$P: WARN: no clocks or clock-freqs inserted for uart!\n"
		if (!$ni && BcmUtils::get_num_serial($bd));
	}


	####################
	# USB, old way
	####################
	# USB, ehci, ohci
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USB2[01]$/, qr/^usb\@/, qr/^[eo]hci\@/);
	# USB xhci
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USB3[01]$/, qr/^usb\@/, qr/^xhci\@/);
	# USB, containing node
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'none',
		qr/^USB2[01]$/, qr/^usb\@/);
	print "$P: WARN: no clocks inserted for USB!\n"
		if (!$ni && BcmUtils::get_num_usb($bd));

	####################
	# USB, new way
	####################
	# ehci
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USB2[01]$/, qr/^ehci_v2\@/, undef, 1);
	# ohci
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USB2[01]$/, qr/^ohci_v2\@/, undef, 1);
	# xhci
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USB3[01]$/, qr/^xhci_v2\@/, undef, 1);
	# bdc (only needed for new style because it onlys exists
	# on new systems which will have the new style driver.
	insert_clocks_prop_into_devs($rdb, $rh_funcs, 'grow',
		qr/^USBD$/, qr/^bdc_v2\@/, undef, 1);
	# usb-phy -- do some ad hoc things here to match what we want:
	# If there is only one device, use the functions (USB2x, USB3x).
	# If there is more than one device, create a list of
	# (USB20, USB21, USB30, USB30) and then use the 'interleave'
	# algorithm to assign the clocks.
	@a = get_devs($dt, qr/^usb-phy\@/, 1);
	my $num_usb_phy = @a;
	my $re0_arr = [ qr/^USB2[01]$/, qr/^USB3[01]$/, qr/^USB3[01]$/ ];
	my $re1_arr = [ qr/^USB2[01]$/, qr/^USB3[01]$/];
	$ni = insert_clocks_prop_into_devs($rdb, $rh_funcs, 'interleave',
		$num_usb_phy == 1 ? $re1_arr:$re0_arr, qr/^usb-phy\@/, undef, 1);
	print "$P: WARN: no clocks inserted for USB!\n"
		if (!$ni && BcmUtils::get_num_usb($bd));
	# End of new USB clock assignment.
	####################
    
	# Add the clock aliases
	foreach my $k (sort keys %{$rh->{aliases}}) {
		add_reference_alias($dt, $k, ${$rh->{aliases}}{$k});
	}
}


1;
