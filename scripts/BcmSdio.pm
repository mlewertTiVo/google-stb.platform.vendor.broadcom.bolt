################################################################################
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
################################################################################
package BcmSdio;
use strict;
use warnings FATAL=>q(all);

sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

sub get_num($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $bchp_defines = $rh->{rh_defines};
	my $count = 0;
	my $tmp;

	do {
		$tmp = sprintf('BCHP_SDIO_%d_HOST_REG_START', $count);
		$count++ if $bchp_defines->{$tmp};
	} while ($bchp_defines->{$tmp});

	cfg_error("unexpected script problem with get_num_sdio(): $count < 0")
		if scalar($count) < 0;
	return $count;
}

########################################################################
# FUNCTION:
#   get_pinsel_type()
#
# DESCRIPTION:
#   Takes in a chip family, outputs the type of SDIO pin selections
#
# PARAMS:
#   $family_name
#
# RETURNS:
#   0 if SDIO pinout selection is not required
#   1 if SUN_TOP_CTRL_GENERAL_CTRL_0[sdio0_alt_pin_sel] determines
#     whether SDIO pin outs are from (onoff_gpio_079:087,091:093) or
#     from (aon_gpio_03:09,11,12,14,15)
#   2 if SUN_TOP_CTRL_GENERAL_CTRL_1[sdio_0_pin_sel] determines
#     whether pin outs of SDIO#0 are from (gpio_021:029,033:037) or
#     from (aon_gpio_03:09,11,12,14,15), and
#     SUN_TOP_CTRL_GENERAL_CTRL_1[sdio_1_pin_sel] determines whether
#     pin outs of SDIO#1 are from (ebi_data_00:03,ebi_dsb,ebi_nand_rbb,
#     ebi_nand_wpb,ebi_nand_dqs,ebi_we0b,ebi_rdb,ebi_tsb) or
#     (gpio_000:011,033:037)
########################################################################
sub get_pinsel_type($)
{
	my ($f) = @_;
	my ($family_name) = $f->{familyname};
	my %pin_sel_tbl = (
	       "7250b0" => "1",
	       "7260b0" => "2",
	       "7268a0" => "2",
	       "7268b0" => "2",
	       "7271a0" => "2",
	       "7271b0" => "2",
	       "7364b0" => "1",
	       "7364c0" => "1",
        );

	return ($pin_sel_tbl{$family_name})
		if defined($pin_sel_tbl{$family_name});

	return 0;
}

sub prepare_sdio_pinsel_type1($) {
	my ($board) = @_;
	my %type1_def = (
		onoff_gpio_079 => 'SD_CARD0_CMD',
		onoff_gpio_080 => 'SD_CARD0_CLK',
		onoff_gpio_081 => 'SD_CARD0_DAT0',
		onoff_gpio_082 => 'SD_CARD0_DAT1',
		onoff_gpio_083 => 'SD_CARD0_DAT2',
		onoff_gpio_084 => 'SD_CARD0_DAT3',
		onoff_gpio_085 => 'SD_CARD0_CLK_IN',
		onoff_gpio_086 => 'SD_CARD0_PRES',
		onoff_gpio_087 => 'SD_CARD0_WPROT',
		onoff_gpio_091 => 'SD_CARD0_',
		onoff_gpio_092 => 'SD_CARD0_',
		onoff_gpio_093 => 'SD_CARD0_PWR0',
		onoff_gpio_094 => 'SD_CARD0_LED',
		onoff_gpio_095 => 'SD_CARD0_VOLT',);
	my %type1_alt = (
		aon_gpio_03 => 'SD_CARD0_DAT0',
		aon_gpio_04 => 'SD_CARD0_DAT1',
		aon_gpio_05 => 'SD_CARD0_DAT2',
		aon_gpio_06 => 'SD_CARD0_DAT3',
		aon_gpio_07 => 'SD_CARD0_LED',
		aon_gpio_08 => 'SD_CARD0_CLK',
		aon_gpio_09 => 'SD_CARD0_PWR0',
		aon_gpio_11 => 'SD_CARD0_CMD',
		aon_gpio_12 => 'SD_CARD0_CLK_IN',
		aon_gpio_15 => 'SD_CARD0_WPROT',
		aon_gpio_14 => 'SD_CARD0_PRES',);
	my $pattern = "sd_card0_";
	my $mask = "BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_sdio0_alt_pin_sel_MASK";
	my $val = "1<<BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0_sdio0_alt_pin_sel_SHIFT";

	my $fsbl_from_def = 0;
	my $fsbl_from_alt = 0;

	for my $p (@{$board->{pmux}}) {
		my @pins = $p->get_pins(1);
		foreach (@pins) {
			if (exists $type1_def{$_->{pin}}) {
				$fsbl_from_def = 1
					if $_->{select} =~ /^$pattern/i;
			} elsif (exists $type1_alt{$_->{pin}}) {
				$fsbl_from_alt = 1
					if $_->{select} =~ /^$pattern/i;
			}
			die "SDIO pinouts from ON/OFF and AON"
				if $fsbl_from_def && $fsbl_from_alt;
		}
	}

	for $b (::get_valid_boards()) {
		my $ssbl_from_def = $fsbl_from_def;
		my $ssbl_from_alt = $fsbl_from_alt;
		for my $p (@{$b->{pmux}}) {
			my @pins = $p->get_pins(0);
			foreach (@pins) {
				if (exists $type1_def{$_->{pin}}) {
					$ssbl_from_def = 1
						if $_->{select} =~ /^$pattern/i;
				} elsif (exists $type1_alt{$_->{pin}}) {
					$ssbl_from_alt = 1
						if $_->{select} =~ /^$pattern/i;
				}
				die "SDIO pinouts from ON/OFF and AON"
					if $ssbl_from_def && $ssbl_from_alt;
			}
		}
		if ($ssbl_from_alt) {
			$b->{sdio_pinsel}->{regset} =
				"BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0";
			$b->{sdio_pinsel}->{mask} = $mask;
			$b->{sdio_pinsel}->{val} = $val;
		}
	}
}

sub prepare_sdio_pinsel_type2($) {
	my ($board) = @_;
	my %type2_0_def = (
		gpio_021 => 'SD_CARD0_0_CMD',
		gpio_022 => 'SD_CARD0_0_CLK',
		gpio_023 => 'SD_CARD0_0_DAT0',
		gpio_024 => 'SD_CARD0_0_DAT1',
		gpio_025 => 'SD_CARD0_0_DAT2',
		gpio_026 => 'SD_CARD0_0_DAT3',
		gpio_027 => 'SD_CARD0_0_CLK_IN',
		gpio_028 => 'SD_CARD0_0_PRES',
		gpio_029 => 'SD_CARD0_0_WPROT',
		gpio_033 => 'SD_CARD0_0_',
		gpio_034 => 'SD_CARD0_0_',
		gpio_035 => 'SD_CARD0_0_PWR0',
		gpio_036 => 'SD_CARD0_0_LED',
		gpio_037 => 'SD_CARD0_0_VOLT',);
	my %type2_0_alt = (
		aon_gpio_03 => 'SD_CARD0_1_DAT0',
		aon_gpio_04 => 'SD_CARD0_1_DAT1',
		aon_gpio_05 => 'SD_CARD0_1_DAT2',
		aon_gpio_06 => 'SD_CARD0_1_DAT3',
		aon_gpio_07 => 'SD_CARD0_1_LED',
		aon_gpio_08 => 'SD_CARD0_1_CLK',
		aon_gpio_09 => 'SD_CARD0_1_PWR0',
		aon_gpio_11 => 'SD_CARD0_1_CMD',
		aon_gpio_12 => 'SD_CARD0_1_CLK_IN',
		aon_gpio_14 => 'SD_CARD0_1_PRES',
		aon_gpio_15 => 'SD_CARD0_1_WPROT',);
	my $fsbl_0_def = 0;
	my $fsbl_0_alt = 0;

	my %type2_1_def = (
		ebi_data_03 => 'SD_CARD1_0_DAT3',
		ebi_data_02 => 'SD_CARD1_0_DAT2',
		ebi_data_01 => 'SD_CARD1_0_DAT1',
		ebi_data_00 => 'SD_CARD1_0_DAT0',
		ebi_tsb => 'SD_CARD1_0_PRES',
		ebi_rdb => 'SD_CARD1_0_CLK_IN',
		ebi_we0b => 'SD_CARD1_0_CLK',
		ebi_nand_dqs => 'SD_CARD1_0_PWR0',
		ebi_nand_wpb => 'SD_CARD1_0_WPROT',
		ebi_nand_rbb => 'SD_CARD1_0_CMD',
		ebi_dsb => 'SD_CARD1_0_LED',);
	my %type2_1_alt = (
		gpio_000 => 'SD_CARD1_1_LED',
		gpio_001 => 'SD_CARD1_1_CMD',
		gpio_002 => 'SD_CARD1_1_CLK',
		gpio_003 => 'SD_CARD1_1_DAT0',
		gpio_004 => 'SD_CARD1_1_DAT1',
		gpio_005 => 'SD_CARD1_1_DAT2',
		gpio_006 => 'SD_CARD1_1_DAT3',
		gpio_007 => 'SD_CARD1_1_CLK_IN',
		gpio_008 => 'SD_CARD1_1_PRES',
		gpio_009 => 'SD_CARD1_1_WPROT',
		gpio_010 => 'SD_CARD1_1_PWR0',
		gpio_011 => 'SD_CARD1_1_VOLT',
		gpio_033 => 'SD_CARD1_1_WPROT',
		gpio_034 => 'SD_CARD1_1_PRES',
		gpio_035 => 'SD_CARD1_1_VOLT',
		gpio_036 => 'SD_CARD1_1_LED',
		gpio_037 => 'SD_CARD1_1_PWR0',);
	my $fsbl_1_def = 0;
	my $fsbl_1_alt = 0;
	my $pattern = "sd_card";
	my $mask0 = "BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sdio_0_pin_sel_MASK";
	my $mask1 = "BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sdio_1_pin_sel_MASK";
	my $val0 = "(1<<BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sdio_0_pin_sel_SHIFT)";
	my $val1 = "(1<<BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_sdio_0_pin_sel_SHIFT)";

	for my $p (@{$board->{pmux}}) {
		my @pins = $p->get_pins(1);
		foreach (@pins) {
			if (exists $type2_0_def{$_->{pin}}) {
				$fsbl_0_def = 1
					if $_->{select} =~ /^$pattern/i;
			} elsif (exists $type2_0_alt{$_->{pin}}) {
				$fsbl_0_alt = 1
					if $_->{select} =~ /^$pattern/i;
			} elsif (exists $type2_1_def{$_->{pin}}) {
				$fsbl_1_def = 1
					if $_->{select} =~ /^$pattern/i;
			} elsif (exists $type2_1_alt{$_->{pin}}) {
				$fsbl_1_alt = 1
					if $_->{select} =~ /^$pattern/i;
			}
			die "SDIO pinouts from ON/OFF and AON"
				if $fsbl_0_def && $fsbl_0_alt;
			die "SDIO pinouts from EBI and GPIO"
				if $fsbl_1_def && $fsbl_1_alt;
		}
	}

	for $b (::get_valid_boards()) {
		my $ssbl_0_def = $fsbl_0_def;
		my $ssbl_0_alt = $fsbl_0_alt;
		my $ssbl_1_def = $fsbl_1_def;
		my $ssbl_1_alt = $fsbl_1_alt;
		for my $p (@{$b->{pmux}}) {
			my @pins = $p->get_pins(0);
			foreach (@pins) {
				if (exists $type2_0_def{$_->{pin}}) {
					$ssbl_0_def = 1
						if $_->{select} =~ /^$pattern/i;
				} elsif (exists $type2_0_alt{$_->{pin}}) {
					$ssbl_0_alt = 1
						if $_->{select} =~ /^$pattern/i;
				} elsif (exists $type2_1_def{$_->{pin}}) {
					$ssbl_1_def = 1
						if $_->{select} =~ /^$pattern/i;
				} elsif (exists $type2_1_alt{$_->{pin}}) {
					$ssbl_1_alt = 1
						if $_->{select} =~ /^$pattern/i;
				}
				die "SDIO pinouts from ON/OFF and AON"
					if $ssbl_0_def && $ssbl_0_alt;
				die "SDIO pinouts from EBI and GPIO"
					if $ssbl_1_def && $ssbl_1_alt;
			}
		}
		if ($ssbl_0_alt || $ssbl_1_alt) {
			$b->{sdio_pinsel}->{regset} =
				"BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1";
			if (!$ssbl_0_alt) {
				$mask0 = "0";
				$val0 = "0";
			}
			if (!$ssbl_1_alt) {
				$mask1 = "0";
				$val1 = "0";
			}
			$b->{sdio_pinsel}->{mask} = $mask0 . " | " . $mask1;
			$b->{sdio_pinsel}->{val} = $val0 . " | " . $val1;
		}
	}
}

sub prepare_sdio_pinsel($$) {
	my ($pin_sel_type, $board) = @_;

	if ($pin_sel_type == 1) {
		prepare_sdio_pinsel_type1($board);
	} elsif ($pin_sel_type == 2) {
		prepare_sdio_pinsel_type2($board);
	}
}

sub gen_sdio($) {
	my ($sdio) = @_;
	my $text = "\t\t.sdio = {\n";
	for my $s (@$sdio) {
		$text .= "\t\t\t{ ";
		$text .= $s->{controller} . ", ";
		$text .= "SDIO_TYPE_" . uc($s->{type}) . ", ";
		if ($s->{uhs}) {
		    $text .= "1";
		} else {
		    $text .= "0";
		}
		$text .= ", ";
		if ($s->{host_driver_strength}) {
		    $text .= "SDIO_DRIVER_STRENGTH_" .
			$s->{host_driver_strength};
		} else {
		    $text .= "SDIO_DRIVER_STRENGTH_UNDEFINED";
		}
		$text .= ", ";
		if ($s->{host_hs_driver_strength}) {
		    $text .= "SDIO_DRIVER_STRENGTH_" .
			$s->{host_hs_driver_strength};
		} else {
		    $text .= "SDIO_DRIVER_STRENGTH_UNDEFINED";
		}
		$text .= ", ";
		if ($s->{card_driver_strength}) {
		    $text .= "SDIO_DRIVER_STRENGTH_" .
			$s->{card_driver_strength};
		} else {
		    $text .= "SDIO_DRIVER_STRENGTH_UNDEFINED";
		}
		$text .= " },\n";
	}
	return $text . "\t\t\t{ -1, -1, -1, -1, -1, -1 }\n\t\t},\n";
}

sub gen_sdio_pinsel($) {
	my ($sdio_pinsel) = @_;
	my $text = "\t\t.sdio_pinsel = { ";

	if (!defined $sdio_pinsel) {
		$text .= "0, 0, 0 },\n";
	} else {
		$text .= $sdio_pinsel->{regset} . ", ";
		$text .= $sdio_pinsel->{mask} . ", ";
		$text .= $sdio_pinsel->{val} . " },\n";
	}
	return $text;
}

1;
