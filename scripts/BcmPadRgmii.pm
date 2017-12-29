################################################################################
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
################################################################################

# NOTE: only 7271b0 requires additional control for RGMII (and MTSIF) PAD

package BcmPadRgmii;
use strict;
use warnings FATAL=>q(all);

sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;

	# 'id' and 'voltage' have been verified to exist via check_opts()
	::cfg_error('applicable if and only if 7271b0')
		if ::get_family_name() !~ /^(7271b0)$/i;
	::cfg_error('BAD RGMII/MTSIF identifier') if $s->{id} !~ /^[01]$/i;
	::cfg_error('option for voltage must be none, high or low')
		if $s->{voltage} !~ /^(none|high|low)$/i;

	return $s;
}

sub get_num($) {
	my ($f) = @_;

	return 2 # 2 sets of pads for 7271b0
		if $f->{familyname} =~ /^(7271b0)$/i;

	return 0;
}

########################################################################
# FUNCTION:
#   scan_pinsel()
#
# DESCRIPTION:
#   PINMUX is scanned to check whether RGMII (or MTSIF) is selected per
#   board. If so, an internal data structure (internal to scripting
#   engine) is prepared unless the PAD of the corresponding RGMII
#   interface is explicitly configured via the build configuration file.
#   The prepared data structure gets translatd into a C file later on.
#
# PARAMS:
#   $family_name
#
########################################################################
sub scan_pinsel($) {
	my ($f) = @_;

	return if $f->{familyname} !~ /^(7271b0)$/i; # only for 7271b0

	my %set_1 = (
		gpio_001 => 'MTSIF1_CLK', # ENET1_RGMII_RX_CLK, SD_CARD1_1_CMD
		gpio_002 => 'MTSIF1_DATA0',
			# ENET1_RGMII_RX_EN_CTL, SD_CARD1_1_CLK
		gpio_003 => 'MTSIF1_SYNC', # ENET1_RGMII_RXD_00, SD_CARD1_1_DAT0
		gpio_004 => 'MTSIF1_DATA2',
			# ENET1_RGMII_RXD_01, SD_CARD1_1_DAT1
		gpio_005 => 'MTSIF1_DATA3',
			# ENET1_RGMII_RXD_02, SD_CARD1_1_DAT2
		gpio_006 => 'MTSIF1_DATA4',
			# ENET1_RGMII_RXD_03, SD_CARD1_1_DAT3
		gpio_007 => 'MTSIF1_DATA1',
			# ENET1_RGMII_TX_CLK, SD_CARD1_1_CLK_IN
		gpio_008 => 'MTSIF1_DATA6',
			# ENET1_RGMII_TX_EN_CTL, SD_CARD1_1_PRES
		gpio_009 => 'MTSIF1_DATA5',
			# ENET1_RGMII_TXD_00, SD_CARD1_1_WPROT
		gpio_011 => 'MTSIF1_DATA7',);
			# ENET1_RGMII_TXD_02, SD_CARD1_1_VOLT
	my %set_0 = (
		gpio_021 => 'MTSIF0_CLK', # ENET0_RGMII_RX_CLK, SD_CARD0_0_CMD
		gpio_022 => 'MTSIF0_DATA0',
			# ENET0_RGMII_RX_EN_CTL, SD_CARD0_0_CLK
		gpio_023 => 'MTSIF0_SYNC', # ENET0_RGMII_RXD_00, SD_CARD0_0_DAT0
		gpio_024 => 'MTSIF0_DATA1',
			# ENET0_RGMII_RXD_01, SD_CARD0_0_DAT1
		gpio_026 => 'MTSIF0_DATA2',
			# ENET0_RGMII_RXD_04, SD_CARD0_0_DAT3
		gpio_027 => 'MTSIF0_DATA3',
			# ENET0_RGMII_TX_CLK, SD_CARD0_0_CLK_IN
		gpio_028 => 'MTSIF0_DATA4',
			# ENET0_RGMII_TX_EN_CTL, SD_CARD0_0_PRES
		gpio_029 => 'MTSIF0_DATA5',
			# ENET0_RGMII_TXD_00, SD_CARD0_0_WPROT
		gpio_030 => 'MTSIF0_DATA6', # ENET0_RGMII_TXD_01
		gpio_031 => 'MTSIF0_DATA7',); # ENET0_RGMII_TXD_02
	my %set_1_alt = (
		aon_gpio_03 => 'MTSIF1_DATA5_ALT', # SD_CARD0_1_DAT0
		aon_gpio_05 => 'MTSIF1_DATA7_ALT', # SD_CARD0_1_DAT2
		aon_gpio_06 => 'MTSIF1_DATA6_ALT', # SD_CARD0_1_DAT3
		aon_gpio_07 => 'SD_CARD0_1_LED',
		aon_gpio_08 => 'SD_CARD0_1_CLK',
		aon_gpio_09 => 'SD_CARD0_1_PWR0',
		aon_gpio_11 => 'MTSIF1_CLK_ALT', # SD_CARD0_1_CMD
		aon_gpio_12 => 'MTSIF1_DATA0_ALT', # SD_CARD0_1_CLK_IN
		aon_gpio_14 => 'MTSIF1_SYNC_ALT', # SD_CARD0_1_PRES
		aon_gpio_15 => 'MTSIF1_DATA1_ALT', # SD_CARD0_1_WPROT
		aon_gpio_17 => 'MTSIF1_DATA2_ALT',
		aon_gpio_18 => 'MTSIF1_DATA3_ALT',
		aon_gpio_20 => 'MTSIF1_DATA4_ALT',);
	my $fsbl_mtsif_0 = 0;
	my $fsbl_rgmii_0 = 0;
	my $fsbl_mtsif_1 = 0;
	my $fsbl_rgmii_1 = 0;
	my $fsbl_mtsif_1_alt = 0;
	my $fsbl_sd_0 = 0;
	my $fsbl_sd_0_alt = 0;
	my $fsbl_sd_1_alt = 0;
	my $pattern_mtsif = "mtsif";
	my $pattern_rgmii = "enet";
	my $pattern_sd = "sd_card";

	for my $p (@{$f->{pmux}}) {
		# scan through pinmux at FSBL (family wide)
		my @pins = $p->get_pins(1);
		foreach (@pins) {
			if (exists $set_0{$_->{pin}}) {
				$fsbl_mtsif_0 = 1
					if $_->{select} =~ /^$pattern_mtsif/i;
				$fsbl_rgmii_0 = 1
					if $_->{select} =~ /^$pattern_rgmii/i;
				$fsbl_sd_0 = 1
					if $_->{select} =~ /^$pattern_sd/i;
			} elsif (exists $set_1{$_->{pin}}) {
				$fsbl_mtsif_1 = 1
					if $_->{select} =~ /^$pattern_mtsif/i;
				$fsbl_rgmii_1 = 1
					if $_->{select} =~ /^$pattern_rgmii/i;
				$fsbl_sd_1_alt = 1
					if $_->{select} =~ /^$pattern_sd/i;
			} elsif (exists $set_1_alt{$_->{pin}}) {
				$fsbl_mtsif_1_alt = 1
					if $_->{select} =~ /^$pattern_mtsif/i;
				$fsbl_sd_0_alt = 1
					if $_->{select} =~ /^$pattern_sd/i;
			}
		}
		#    PAD/PIN      MTSIF   RGMII    SD          Notes
		# ============== ======= ======= ======= ====================
		# gpio_001~011      1       1     1_alt   mutually exclusive
		# gpio_021~031      0       0       0     mutually exclusive
		# aon_gpio_03~20  1_alt    N/A    0_alt   mutually exclusive
		# ebi_xxx          N/A     N/A      1

		die "mixing up MTSIF and RGMII in set #0"
			if $fsbl_mtsif_0 && $fsbl_rgmii_0;
		die "mixing up MTSIF and SD in set #0"
			if $fsbl_mtsif_0 && $fsbl_sd_0;
		die "mixing up RGMII and SD in set #0"
			if $fsbl_rgmii_0 && $fsbl_sd_0;
		die "mixing up MTSIF and RGMII in set #1"
			if $fsbl_mtsif_1 && $fsbl_rgmii_1;
		die "mixing up MTSIF and SD in set #1"
			if $fsbl_mtsif_1 && $fsbl_sd_1_alt;
		die "mixing up RGMII and SD in set #1"
			if $fsbl_rgmii_1 && $fsbl_sd_1_alt;
		die "mixing up MTSIF across ON/OFF and AON"
			if $fsbl_mtsif_1 && $fsbl_mtsif_1_alt;
		die "mixing up MTSIF and SD in AON"
			if $fsbl_mtsif_1_alt && $fsbl_sd_0_alt;
	}

	# process per-board pinmux (at SSBL)
	for $b (::get_valid_boards()) {
		my $ssbl_mtsif_0 = $fsbl_mtsif_0;
		my $ssbl_rgmii_0 = $fsbl_rgmii_0;
		my $ssbl_mtsif_1 = $fsbl_mtsif_1;
		my $ssbl_rgmii_1 = $fsbl_rgmii_1;
		my $ssbl_mtsif_1_alt = $fsbl_mtsif_1_alt;
		my $ssbl_sd_0 = $fsbl_sd_0;
		my $ssbl_sd_0_alt = $fsbl_sd_0_alt;
		my $ssbl_sd_1_alt = $fsbl_sd_1_alt;
		for my $p (@{$b->{pmux}}) {
			my @pins = $p->get_pins(0);
			foreach (@pins) {
				if (exists $set_0{$_->{pin}}) {
					$ssbl_mtsif_0 = 1
						if $_->{select} =~
							/^$pattern_mtsif/i;
					$ssbl_rgmii_0 = 1
						if $_->{select} =~
							/^$pattern_rgmii/i;
					$ssbl_sd_0 = 1
						if $_->{select} =~
							/^$pattern_sd/i;
				} elsif (exists $set_1{$_->{pin}}) {
					$ssbl_mtsif_1 = 1
						if $_->{select} =~
							/^$pattern_mtsif/i;
					$ssbl_rgmii_1 = 1
						if $_->{select} =~
							/^$pattern_rgmii/i;
					$ssbl_sd_1_alt = 1
						if $_->{select} =~
							/^$pattern_sd/i;
				} elsif (exists $set_1_alt{$_->{pin}}) {
					$ssbl_mtsif_1_alt = 1
						if $_->{select} =~
							/^$pattern_mtsif/i;
					$ssbl_sd_0_alt = 1
						if $_->{select} =~
							/^$pattern_sd/i;
				}
			}
			die "mixing up MTSIF and RGMII in set #0"
				if $ssbl_mtsif_0 && $ssbl_rgmii_0;
			die "mixing up MTSIF and SD in set #0"
				if $ssbl_mtsif_0 && $ssbl_sd_0;
			die "mixing up RGMII and SD in set #0"
				if $ssbl_rgmii_0 && $ssbl_sd_0;
			die "mixing up MTSIF and RGMII in set #1"
				if $ssbl_mtsif_1 && $ssbl_rgmii_1;
			die "mixing up MTSIF and SD in set #1"
				if $ssbl_mtsif_1 && $ssbl_sd_1_alt;
			die "mixing up RGMII and SD in set #1"
				if $ssbl_rgmii_1 && $ssbl_sd_1_alt;
			die "mixing up MTSIF across ON/OFF and AON"
				if $ssbl_mtsif_1 && $ssbl_mtsif_1_alt;
			die "mixing up MTSIF and SD in AON"
				if $fsbl_mtsif_1_alt && $fsbl_sd_0_alt;
		}

		my $using_set_0 = $ssbl_mtsif_0 || $ssbl_rgmii_0;
		my $using_set_1 = $ssbl_mtsif_1 || $ssbl_rgmii_1;
		# There is no PAD control for MTSIF_ALT_1.
		# PAD's of MTSIF_1_ALT can only be 3.3V.

		# explict 'pad_rgmii' cfg command has the priority
		for my $r (@{$b->{pad_rgmii}}) {
			$using_set_0 = 0 if $r->{id} eq '0';
			$using_set_1 = 0 if $r->{id} eq '1';
		}

		# if not overruled, high voltage is the default
		if ($using_set_0) {
			my %h = (
				"id" => "0",
				"voltage" => "high",
			);
			$b->add('pad_rgmii', \%h);
		}

		if ($using_set_1) {
			my %h = (
				"id" => "1",
				"voltage" => "high",
			);
			$b->add('pad_rgmii', \%h);
		}
	}
}

sub gen_pad_rgmii($) {
	my ($rgmii) = @_;
	my $text = "\t\t.pad_rgmii = {\n";
	for my $r (@$rgmii) {
		$text .= "\t\t\t{ ";
		$text .= $r->{id} . ", ";
		$text .= "PAD_CTRL_" . uc($r->{voltage}) . " },\n";
	}
	return $text . "\t\t\t{ -1, PAD_CTRL_NONE }\n\t\t},\n";
}

1;
