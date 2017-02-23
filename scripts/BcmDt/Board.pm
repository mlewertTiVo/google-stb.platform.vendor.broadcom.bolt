################################################################################
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
################################################################################
package BcmDt::Board;
use strict;
use warnings FATAL=>q(all);
no if $] >= 5.017011, warnings => 'experimental::smartmatch';
use Data::Dumper;
use File::Basename;
use BcmUtils;
use BcmDt::Devices;
use Carp;
our $Debug = 0;

sub gen_compat_and_model($$) {
	my ($board, $family_name) = @_;
	my $text = "";

	$text .= "\tcompatible = \"brcm,".lc($board)."\", ".
		 "\"brcm,$family_name\", \"brcm,brcmstb\";\n";
	$text .= "\tmodel = \"Broadcom $board\";\n";

	return $text;
};

sub gen_board_dts_header($$) {
	my ($board, $Family) = @_;
	my $text = "";

	# Purposedly do not include any /dts-v1/ header to avoid compilation
	# failures
	$text .= "/include/ \"" . $Family->{dts}->{filename_dtx} . "\"\n\n";
	$text .= "/ {\n";
	$text .= gen_compat_and_model($board, $Family->{familyname});
	$text .= "};\n\n";

	return $text;
}

sub gen_cpu_dts($)
{
	my ($cset) = @_;
	my $text = "";
	my $freq;

	for my $c (@$cset) {
		next if $c->{name} ne "CHIP_TYPICAL_FREQUENCY";
		$freq = $c->{value};
		$freq =~ s/UL//g;
	}

	$text .= "/ {\n";
	$text .= "\tcpus {\n";
	for (my $cpu = 0; $cpu < 4; $cpu++) {
		$text .= sprintf("\t\tcpu@%d {\n", $cpu);
		$text .= sprintf("\t\t\tclock-frequency = <%d>;\n", $freq);
		$text .= "\t\t};\n\n";
	}
	$text .= "\t};\n\n";
	$text .= "};\n\n";

	return $text;
}

sub map_ddr_to_cpu($$$)
{
	my ($dram_map, $memc, $size_mb) = @_;
	my $num_dram_entries = scalar(@$dram_map);
	my @regs = ();

	if ($size_mb eq 0) {
		return @regs;
	}

	for my $n (@$dram_map) {
		my $offset_mb;
		my $size_tbl_mb;
		my $length_mb;
		my $reg_elem;

		next if ($memc ne $n->{memc});

		$offset_mb = Math::BigInt->new($n->{to_bytes})->bdiv(1024*1024);
		$size_tbl_mb = Math::BigInt->new($n->{size_bytes})->bdiv(1024*1024);

		if ($size_mb >= $size_tbl_mb) {
			$length_mb = $size_tbl_mb;
			$size_mb -= $size_tbl_mb;
		} else {
			$length_mb = $size_mb;
			$size_mb = 0;
		}

		$reg_elem->{base} = Math::BigInt->new($offset_mb)->bmul(1024*1024);
		$reg_elem->{size} = Math::BigInt->new($length_mb)->bmul(1024*1024);
		push @regs, $reg_elem;

		return @regs if ($size_mb eq 0);
	}

	return @regs;
}

sub gen_memory_dts($$) {
	my ($ddr, $dram_map) = @_;
	my $text = "";
	my %defaults = (
		"\t\t#address-cells" => [ 'dec', 1 ],
		"\t\t#size-cells" => [ 'dec', 1 ],
		"\t\tdevice_type" => "memory",
	);
	my $nb_dram = scalar(@$dram_map);
	my $nb_ddr = scalar(@$ddr);
	my $count = 0;
	my @regs;
	my $num_entries;

	if ($nb_dram >= $nb_ddr) {
		$num_entries = $nb_dram;
	} else {
		$num_entries = $nb_ddr;
	}

	for my $n (@$ddr) {
		if ($nb_dram) {
			push @regs, map_ddr_to_cpu($dram_map, $n->{n}, $n->{size_mb});
		} else {
			my $elem;
			$elem->{base} = Math::BigInt->new($n->{base_mb})->bmul(1024*1024);
			$elem->{size} = Math::BigInt->new($n->{size_mb})->bmul(1024*1024);

			push @regs, $elem;
		}
	}

	my @regs_sorted = sort { $a->{base} <=> $b->{base} } @regs;

	$text .= "/ {\n";
	$text .= "\tmemory {\n";
	$text .= BcmDt::Devices::output_default(\%defaults);
	$text .= "\t\treg = <";
	for my $r (@regs_sorted) {
		my $last = $regs_sorted[-1];
		my $sep = $r eq $last ? "" : " ";
		my @addr = BcmUtils::bigint_to_two_hexint($r->{base});
		my @size = BcmUtils::bigint_to_two_hexint($r->{size});

		next if ($size[0] eq Math::BigInt->bnan() or
			 $size[1] eq Math::BigInt->bnan());

		$text .= sprintf("0x%x 0x%x 0x%x 0x%x%s",
				 hex($addr[0]), hex($addr[1]),
				 hex($size[0]), hex($size[1]), $sep);
	}
	$text .= ">;\n";
	$text .= "\t};\n\n";
	$text .= "};\n\n";

	return $text;
}

sub enet_needs_fixed_link($)
{
	my $e = shift;

	return 1 if (defined($e->{phy_type}) && substr($e->{phy_type}, 1, -1) eq "MOCA") ||
		    (defined($e->{mdio_mode}) && substr($e->{mdio_mode}, 1, -1) eq "0") ||
		    ($e->{mdio_mode} eq "NULL" && defined($e->{phy_type}));
	return 0;
}

sub enet_phy_mode($)
{
	my $e = shift;
	my $type = substr($e->{phy_type}, 1, -1);
	my %phy_types = (
		"INT" => "internal",
		"MII" => "mii",
		"GMII" => "gmii",
		"RGMII_NO_ID" => "rgmii",
		"RGMII_IBS" => "rgmii-ibs",
		"RGMII" => "rgmii-txid",
		"RVMII" => "rev-mii",
		"MOCA" => "moca",
	);

	die "Unknown type: $type" if (!defined($phy_types{$type}));

	return $phy_types{$type};
}

sub gen_phy_dts($$)
{
	my ($e, $mdio_text) = @_;
	my $id = substr($e->{phy_id}, 1, -1);
	my $speed = substr($e->{phy_speed}, 1, -1);
	my $type = substr($e->{phy_type}, 1, -1);
	my %default = (
		"\t\treg" => [ 'dec', $id ],
		"\t\tmax-speed" =>  [ 'dec', $speed ],
	);

	$mdio_text .= sprintf("\tphy%d: ethernet-phy@%d {\n", $id, $id);
	$mdio_text .= "\t\tcompatible = ";
	if ($type eq "INT") {
		$mdio_text .= "\"brcm,28nm-gphy\", \"ethernet-phy-ieee802.3-c22\";\n";

		# Newer 28nm chips with EPHY don't have a sw_gphy clock
		if (defined(BcmDt::Devices::get_path_by_label("sw_gphy"))) {
			$mdio_text .= "\t\tclocks = <&sw_gphy>;\n";
			$mdio_text .= "\t\tclock-names = \"sw_gphy\";\n";
		}
	} else {
		# TODO: add support for the new DSA Ethernet switch binding
		$mdio_text .= sprintf("\"brcm,bcm%s\";\n", substr($e->{ethsw}, 3, -1));
	}
	$mdio_text .= BcmDt::Devices::output_default(\%default);

	# Broadcom switches like the 53125 have a borken turn around in their
	# internal MDIO controller, flag this
	if (defined($e->{ethsw}) && $e->{ethsw} ne "NULL") {
		$mdio_text .= "\t\tbroken-turn-around;\n";
	}

	$mdio_text .= "\t};\n\n";

	return $mdio_text;
}

# Missing dynamic properties:
# 	- local-mac-address
sub gen_enet_dts($) {
	my ($enet) = @_;
	my $text = "";
	my $mdio_text = "";
	my $is_switch = 0;

	for my $e (@$enet) {
		if (defined($e->{genet})) {
			$text .= "&eth" . $e->{genet};
			$mdio_text .= "&mdio" . $e->{genet} . " {\n";
		} else {
			$text .= "&sw_port" . $e->{switch_port};
			if ($is_switch eq 0) {
				$mdio_text .= "&mdio {\n";
			}
			$is_switch = 1;
		}
		$text .= " {\n";

		if ($e->{phy_type} eq "NULL") {
			$text .= "\tstatus = \"disabled\";\n";
			$text .= "};\n\n";
			if ($is_switch eq 0) {
				$mdio_text .= "\tstatus = \"disabled\";\n";
				$mdio_text .= "};\n\n";
			}
			next;
		}

		$text .= sprintf("\tphy-mode = \"%s\";\n", enet_phy_mode($e));

		if (enet_needs_fixed_link($e) eq 1) {
			$text .= "\tfixed-link {\n";
			$text .= sprintf("\t\tspeed = <%d>;\n",
					 substr($e->{phy_speed}, 1, -1));
			$text .= "\t\tfull-duplex;\n";
			$text .= "\t};\n";
		} else {
			$text .= sprintf("\tphy-handle = <&phy%d>;\n",
					 substr($e->{phy_id}, 1, -1));

			$mdio_text = gen_phy_dts($e, $mdio_text);

		}

		# We have one MDIO controller per GENET instance, so terminate
		# the node when we iterate overa new GENET instance
		if ($is_switch eq 0) {
			$mdio_text .= "};\n\n";
		}

		$text .= "};\n\n";
	}

	# There is only one MDIO controller with a SWITCH configuration
	# terminate its node now
	if ($is_switch eq 1) {
		$mdio_text .= "};\n\n";
	}

	$text .= $mdio_text;

	return $text;
}

# Missing dynamic properties:
# 	brcm,avs-rmon-vt
#	brcm,avs-rmon-hz
#	local-mac-address
#
sub gen_moca_dts($) {
	my ($moca) = @_;
	my $text = "";

	for my $m (@$moca) {
		$text .= "&bmoca {\n";
		$text .= sprintf("\trf-band = \"%s\";\n", substr($m->{moca_band}, 1, -1));
		$text .= sprintf("\tenet-id = <&%s>;\n", substr($m->{enet_node}, 1, -1));
		$text .= "};\n\n";
	}

	return $text;
}

sub gen_gpio_key_dts($) {
	my ($gpio) = @_;
	my $text = "";
	my %default = (
		"\tcompatible" => "gpio-keys-polled",
		"\t#address-cells" => [ 'dec', 2 ],
		"\t#size-cells" => [ 'dec', 0 ],
		"\tpoll-interval" => [ 'dec', 100 ],
	);

	return $text if scalar(@$gpio) eq 0;

	$text .= "/ {\n";
	$text .= "\tgpio_keys_polled {\n";
	$text .= BcmDt::Devices::output_default(\%default);

	for my $g (@$gpio) {
		$text .= sprintf("\n\t\t%s {\n", substr($g->{name}, 1, -1));
		$text .= sprintf("\t\t\tlinux,code = <%d>;\n", $g->{code});
		$text .= sprintf("\t\\ttgpios = <&%s %d 1>;\n",
				 substr($g->{gpio}, 1, -1), $g->{pin});
		$text .= "\t\t};\n";
	}

	$text .= "\t};\n\n";
	$text .= "};\n\n";

	return $text;
}

sub gen_sdio_dts($) {
	my ($sdio) = @_;
	my $text = "";
	my $width;
	my $non_removable;
	my @strength_props = (
		"host_driver_strength",
		"host_hs_driver_strength",
		"card_driver_strength",
	);

	for my $s (@$sdio) {
		my $use_cmd_12 = 0;
		$text .= "\&sdhci_$s->{controller} {\n";

		if ($s->{type} eq "nodevice") {
			$text .= "\tstatus = \"disabled\";\n";
			$text .= "};\n\n";
			next;
		}

		if ($s->{type} eq "emmc") {
			$width = 8;
			$non_removable = 1;
		} elsif ($s->{type} eq "emmc_on_sdpins") {
			$width = 4;
			$non_removable = 1;
		} else {
			$width = 0;
			$non_removable = 0;
			$use_cmd_12 = 1;
		}

		if ($width ne 0) {
			$text .= sprintf("\tbus-width = <%d>;\n", $width);
		}
		if ($non_removable ne 0) {
			$text .= "\tnon-removable;\n";
		}
		if ($use_cmd_12 ne 0) {
			$text .= "\tsdhci,auto-cmd12;\n";
		}

		if (defined($s->{uhs}) && $s->{uhs} eq 0 ||
		    !defined($s->{uhs})) {
			$text .= "\tno-1-8-v;\n";
		} else {
			# Add properties for newer kernels
			if ($non_removable) {
				$text .= "mmc-hs200-1_8v;\n";
			} else {
				$text .= "sd-uhs-ddr50;\n";
				$text .= "sd-uhs-sdr50;\n";
			}
		}

		for my $strength (@strength_props) {
			if ($s->{"$strength"}) {
				$text .=  sprintf("\t$strength = \"%s\";\n",
						$s->{$strength});
			}
		}

		$text .= "};\n\n";
	}

	return $text;
}

sub gen_memc_dts($)
{
	my ($ddr) = @_;
	my $text = "";

	for my $d (@$ddr) {
		next if (!$d->{size_mb});
		my $label = BcmDt::Devices::get_path_by_label("memc_ddr" . $d->{n});
		next if !defined($label);
		$text .= sprintf("&memc_ddr%d {\n", $d->{n});
		if ($d->{size_mb} eq '-') {
			$text .= "\tstatus=\"disabled\";\n";
		} else {
			$text .= sprintf("\tclock-frequency = <%d>;\n",
				 substr($d->{clk}, 0, -3)  * 1000 * 1000);
		}
		$text .= "};\n\n";
	};

	return $text;
}

sub replace_full_path_by_label($)
{
	my $pval = shift;
	my $label;

	# Extract the full path from this phandle value
	# First kind looks like this:
	# 	<&/rdb/gpio@f040a000 80 0>
	# Second kind could look like this:
	# 	<&/rdb/vreg-wifi-pwr &/rdb/vreg-wifi-pwr>
	if ($pval =~ m/^<&\/(.*)\/(.*)\@([a-z0-9]+)(.*)?>/) {
		my $index = $4;
		my $offset = $3;
		my $full_path = $2 . "\@";
		$offset =~ s/^0+//g;
		$full_path .= $offset;
		$label = BcmDt::Devices::get_label_by_path($full_path);
		my $nval = "<&$label$index>";
		return $nval;
	} elsif ($pval =~ m/^<&\/(.*)\/(.*)>\s?/) {
		my @elems = split(" ", $pval);
		my $nval = "<";
		foreach (@elems) {
			if ($_ =~ m/(<)?&\/([a-z-_]+)\/([a-z-_]+)(>)?/) {
				my $offset = $3;
				$offset =~ s/^0+//g;
				$label = BcmDt::Devices::get_label_by_path($offset);
				$nval .= "&$label";
				$nval .= ($_ eq $elems[-1]) ? "" : " ";
			}
		}
		$nval .= ">";
		return $nval;
	} else {
		return $pval;
	}

	return "NULL"
}

sub gen_dt_ops_dts($)
{
	my ($ops) = @_;
	return if (!$ops);
	my $text = "";

	for my $c (@$ops) {
		my $nodes;
		# If we are not the top-level root, we need to
		# create it
		my $depth = 0;
		my $node_label = "";

		foreach ($c->{root}, $c->{node}, $c->{prop}) {
			s/^[\"\']//;
			s/[\"\']$//;
		}
		my @nodes = split("/", $c->{root});
		my $nodes_count = scalar(@nodes);
		my $prop_name;

		if ($nodes_count eq 0) {
			$text .= "\t" x $depth. "/ {\n";
			$depth++;
		} else {
			foreach (@nodes) {
				$text .= "\t" x $depth;
				if ($_ eq "") {
					$text .= "/";
				} else {
					$text .= "$_";
				}
				$text .= " {\n";
				$depth++;
			}
		}

		# If this is a node with a proper unit, chances are we already
		# created a label for it
		if ($c->{node} =~ m/(.*)\@([a-z0-9]+)/) {
		} else {
			# Else just create a label with the same name
			$node_label = $c->{node};
			$node_label =~ s/-/_/g;
			BcmDt::Devices::insert_label($node_label, $c->{node});
			$node_label .= ": ";
		}

		$text .= "\t" x $depth . $node_label . $c->{node} . " {\n";
		$depth++;

		if ($c->{command} eq "mac") {
			$prop_name = "local-mac-address";
		} elsif ($c->{command} eq "cull") {
			$prop_name = "status";
		} else {
			$prop_name = $c->{prop};
		}
		$text .= "\t" x $depth . $prop_name . " ";

		if (defined($c->{vec})) {
			die if ("ARRAY" ne ref $c->{vec});
			die if (!$c->{prop});
			my $n = scalar(@{$c->{vec}});
			die if (0 == $n);
			my $n_elem = $c->{int};
			my $count = 0;

			$text .= "= <";

			foreach my $i (@{$c->{vec}}) {
				$i = hex($i) if ($i =~ /^0x/i);
				$text .= sprintf("0x%x", $i);
				$text .= ($count++ eq $n_elem - 1) ? "" : " ";
			}
			$text .= ">;\n";
		} else {
			my $sval = "NULL";
			my $pval = "NULL";
			my $ival = 0;
			my $vval = 0;
			my $bval = 0;

			if ($c->{command} eq 'add_node') {
				; # all is good
			} elsif ($c->{command} eq 'cull') {
				$sval = "disabled";
			} elsif ($c->{command} eq 'compile_prop') {
				$c->{string} =~ s/\\"/"/g;
				$pval = replace_full_path_by_label($c->{string});
			} elsif ($c->{string}) {
				$sval = $c->{string};
			} elsif (defined $c->{int}) {
				$c->{int} = hex($c->{int})
					if ($c->{int} =~ /^0x/i);
				$ival = sprintf("0x%x", $c->{int});
			} elsif (defined($c->{bool})) {
				$bval = $c->{bool};
			}

			if ($sval ne "NULL") {
				$text .= "= \"" . $sval . "\";\n";
			} elsif ($pval ne "NULL") {
				$text .= "= " . $pval . ";\n";
			} elsif ($bval ne 0) {
				$text .= ";\n";
			} else {
				$text .= "= <";
				$text .= $ival;
				$text .= $vval;
				$text .= ">;\n";
			}
		}

		if ($nodes_count eq 0) {
			$nodes_count = 1;
		}
		for (my $i = 0; $i < $nodes_count + 1; $i++) {
			$depth--;
			$text .= "\t" x $depth . "};\n\n";
		}
	}

	return $text;
}

1;
