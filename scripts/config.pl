#!/usr/bin/perl -w
use strict;
use warnings FATAL=>q(all);
use Data::Dumper;
use File::Spec;
use File::Basename;
use Math::BigInt;
use Text::ParseWords;
use Getopt::Std;
use lib dirname(File::Spec->rel2abs(__FILE__));

use BcmUtils;
use DevTree;
use BcmDt::Devices;

my $P = basename $0;

# ***************************************************************************
# *     Copyright (c) 2012-2015, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************

my $debug = 2;

use constant THIS_VERSION => "1.32";

my $cheader = "/***************************************************************************
 *     Copyright (c) 2012-2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
\n";

my $makeheader = "#
#	 Copyright (c) 2012-2015, Broadcom Corporation
#	 All Rights Reserved
#	 Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
#\n";

my $_genmarker = " AUTOMATICALLY GENERATED - DO NOT EDIT ";

my $genmarker = "\n/*" . $_genmarker . "*/\n";

#$genprefix = "_gen_";
my $genprefix = "";

my $partion_header_files = "
#include \"lib_types.h\"
#include \"iocb.h\"
#include \"dev_emmcflash.h\"
#include \"flash-partitions.h\"
#include \"ssbl-common.h\"
#include <common.h>
\n";

my $rts_c_header_files = "
#include <rts.h>
#include <memc_arb.h>
#include <ssbl-common.h>
";

my $pinmux_header_files = "
#include \"pinmux.h\"
\n";

# ----------------------------------------

my @configs = (
	       "ALIGN_SIZE",
	       "AMD_SPANSION",
	       "BATTERY_BACKUP",
	       "BOARD_ID",
	       "BSU",
	       "CMA_DEF_ALIGN",
	       "CPU_COHERENT_DMA",
	       "CUSTOM_CODE",
	       "ELF_LDR",
	       "EMBEDDED_PIC",
	       "EMMC_FLASH",
	       "ENET",
	       "FAT32FS",
	       "FATFS",
	       "FLASH_CFI",
	       "FLASH_DMA",
	       "FSBL_CONSOLE",
	       "GENET",
	       "HARDWARE_SHA",
	       "HEAP_SIZE",
	       "INTEL_128J3",
	       "INTEL_P30",
	       "INTEL_P33",
	       "MHL",
	       "MIN_CONSOLE",
	       "MONITOR_OVERTEMP",
	       "NAND_FLASH",
		"SPI_QUAD_MODE",
	       "NETWORK",
	       "PM_S3",
	       "RAW_LDR",
	       "RUNFROMKSEG0",
	       "RUNNER",
	       "SATA",
	       "SERIAL_BAUD_RATE",
	       "SPLASH",
	       "SPLASH_FILE",
	       "SPLASH_PAL",
	       "SREC_LDR",
	       "SSBL_CONSOLE",
	       "STACK_PROTECT_FSBL",
	       "STACK_PROTECT_SSBL",
	       "STACK_SIZE",
	       "SYSINIT",
	       "SYSTEMPORT",
               "TCP",
	       "UI",
	       "UNCACHED",
	       "USB",
	       "USB_BDC",
	       "USB_DISK",
	       "USB_ETH",
	       "USB_HID",
	       "USB_SERIAL",
	       "USB_STARTMODE",
	       "VENDOR_EXTENSIONS",
	       "ZEUS4_1",
	       "ZEUS4_2",
	       "ZIMG_LDR",
	       "ZLIB",
	);

my @bsc_freqs = (375000, 390000, 187500, 200000, 
		 93750, 97500, 46875, 50000);

# ----------------------------------------
# cmdline options
my $arg_family = undef;
my $arg_config_file = undef;
my $arg_gen_dir = undef;
my $arg_rdb_dir = undef;
my $arg_working_dir = undef;
my $arg_odir = undef;
my $arg_single_board = undef;

# ----------------------------------------
#         helpers (no deps)
# ----------------------------------------

# SUB:
#   check_opts($args, $opts, $opt_opts)
#
# PARAMS:
#   $args -- arrayref of the arguments of the command
#   $opts -- arrayref of the mandatory options, without their '-'.
#   $opt_opts -- arrayref of the optional options, without their '-'.
#
# RETURNS:
#   \%h -- hashref with options as keys and values as values.
#
sub check_opts($$$)
{
	my ($args, $opts, $opt_opts) = @_;
	my (%h, $o, $v);
	$opt_opts ||= [];
	while (@$args) {
		$o = shift @$args;
		cfg_error("was expecting an option but got '$o'.")
			if ($o !~ /^-\S/);

		$v = shift @$args;
		cfg_error("expecting a value for option '$o'.")
			if (!defined $v || $v =~ /^-\S/);
		$o =~ s/^-//;
		$h{$o} = $v;
	}
	for $o (@$opts) {
		cfg_error("missing option '-$o'.")
			if (!defined $h{$o});
	}
	my %h_allowed = map { ($_, 1) } (@$opts, @$opt_opts);
	for $o (keys %h) {
		cfg_error("unknown option '-$o'.")
			if (!$h_allowed{$o});
	}
	return \%h;
}

sub stripcomments($) {
	my $s = shift;
	$s =~ s|\/\*.*?\*\/||;
	$s =~ s|\/\/.*?$||;
	$s =~ s|\#.*?$||;
	$s =~ s|^\s+||;
	$s =~ s|\s+| |;
	return $s;
}

sub mulpfix($) {
	my $s = shift;
	$s =~ s|G[bB]|ULL*(1024ULL*1024*1024)|g;
	$s =~ s|M[bB]|ULL*(1024*1024)|g;
	$s =~ s|K[bB]|ULL*(1024)|g;
	return $s;
}

sub mhz_remove($) {
	my $s = shift;
	$s =~ s/MHz//g;
	return $s;
}

sub size_in_mega($) {
	my $s = shift;
	$s =~ s|T|UL*(1024*1024)|g;
	$s =~ s|G|UL*(1024)|g;
	$s =~ s|M|UL|g;
	return $s;
}

sub ulong_remove($) {
	my $s = shift;
	$s =~ s|UL||g;
	return $s;
}

sub read_textfile($) {
	my $f = shift;
	open(FH, '<', $f) or die "Failed to read '$f'";
	return join('',<FH>);
}

sub write_textfile($$) {
	my ($f, $text) = @_;
	open(FH, '>', $f) or die "Failed to write '$f'";
	print FH $text;
	close($text);
	#print "[$f]\n";
}


sub append_textfile($$) {
	my ($f, $text) = @_;
	open(FH, '>>', $f) or die "Failed to append '$f'";
	print FH $text;
	close($text);
}

sub chip2hex($) {
	my ($c) = @_;
	my $l = length($c);
	#  chip=xxxx or xxxxx, rev=yy (6 chars minimum)
	cfg_error("bad chip $c length $l != 6 or 7 ")
		if ($l != 6 && $l != 7);
	#  last 2 char are a-z, no others
	# should exist for now...
	$c =~ tr/a-jA-J/0-9/;
	my $t = hex($c);
	my $r = 0;
	# shift all but last 2 nibbles to top of u32
	# 4 digit: [31..16] chip ID, [15..8] 0, [7..4] major rev, [3..0] minor
	# 5 digit: [31..28] 0, [27..8] chip ID, [7..4] major rev, [3..0] minor
	$r = $t if ($l == 7);
	$r = (($t & 0xffffff00) << 8) | ($t & 0xff) if ($l == 6);
	return sprintf("0x%08x", $r);
}

sub do_shell($)
{
	my $cmd = shift;
#	print "    [$cmd]\n";
	my @a = `$cmd`;
	cfg_error("$cmd (non-zero exit code)")
		if (($? >> 8) & 0xff);
	return wantarray ? @a : join("", @a);
}


# ----------------------------------------
#            classes
# ----------------------------------------
package SlurpCfgFile;
sub new($$) {
	my $class = shift;
	my $fname = shift;
	my $s = { files => [] };
	bless $s, $class;
	$s->open($fname);
	return $s;
}

sub open($$) {
	my ($s, $fname) = @_;
	$fname ||= '';
	open(my $fh, '<', $fname)
		or $s->error("could not open file '$fname'.");
	unshift @{$s->{files}}, { f => $fname, fh => $fh, n => 0};
	return $s;
}

sub close($) {
	my ($s) = @_;
	my $cur = shift @{$s->{files}};
	close $cur->{fh};
	return $s;
}

sub get_line($) {
	my ($s) = @_;
	my $cur = $s->{files}->[0];
	return undef if !$cur;
	my $fh = $cur->{fh};
	my ($line, $tmp) = ('', '');
    AGAIN:
	$tmp = <$fh>;
	$cur->{n}++;
	if (!defined $tmp) {
		$s->error("file '$cur->{f}' ends on a continuation marker!")
			if ($line);
		$s->close();
		return $s->get_line;
	}
	$line .= $tmp;
	chomp $line;
	if ($line =~ s/\\$//) {
		# line continuation marker
		goto AGAIN;
	}

	if ($line =~ /^\s*include\s+(\S+)/) {
		$s->open($1);
		return $s->get_line;
	}
	return $line;
}

sub where($) {
	my ($s) = @_;
	my $cur = $s->{files}->[0];
	return undef if !$cur;
	return sprintf("%s:%d", $cur->{f}, $cur->{n});
}


sub error($$) {
	my ($s, $msg) = @_;
	my $str = "Error: $msg\n";
	my $indent = 4;
	my $first = 1;
	while ( @{$s->{files}} ) {
		my $cur = shift @{$s->{files}};
		$str .= sprintf("%s%s '%s':%d\n", ' ' x $indent,
			$first ? "in" : "included from", $cur->{f}, $cur->{n});
		$first = 0;
		$indent += 4;
	}
	die $str;
}


package Partition;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{name} = shift;
	$s->{size} = shift;
	$s->{device} = shift;
	bless $s, $class;
	return $s;
}

package Flashmap;
sub new($$) {
	my ($class, $s) = (shift, {});
	$s->{name} = shift;
	$s->{maps} = [];
	bless $s, $class;
	return $s;
}

sub Append($$) {
	my $s = shift;
	my $partition = shift;
	push @{$s->{maps}}, $partition;
}

# ----------------
package Flash;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{ftype} = shift;
	$s->{flashmap} = shift;
	bless $s, $class;
	return $s;
}

package Drammap;
sub new($$$$$) {
	my ($class, $s) = (shift, {});
	$s->{memc} = shift;
	$s->{from_bytes} = shift;
	$s->{to_bytes} = shift;
	$s->{size_bytes} = shift;
	bless $s, $class;
	return $s;
}

package Mset;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{name} = shift;
	$s->{value} = shift;
	bless $s, $class;
	return $s;
}

package Cset;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{name} = shift;
	$s->{value} = shift;
	bless $s, $class;
	return $s;
}

package DTops;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Ddr;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Section;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Pmux;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	$s->init();
	$s->match_pins();
	return $s;
}

sub init($) {
	my ($s) = shift;
	$s->{n} = 1 if !defined($s->{n});
	::cfg_error("must have '-sel __NONE__' if setting pad ctrl w/ no gpio")
		if (!$s->{n} && $s->{sel} ne '__NONE__');
	$s->{pull_orig} = $s->{pull};
	$s->{pull} ||= 'none';
	$s->{pull} = "PULL_" . uc($s->{pull});
}

sub match_pins($) {
	my ($s) = shift;
	my $bchp = ::get_bchp_info($s->{family});
	my ($pins, $pads) = BcmUtils::pmux_match(
		$bchp->{rh_pmux}, $s->{pin}, $s->{sel}, $s->{pull});
	my $npins = scalar @$pins;
	my $npads = scalar @$pads;
	::cfg_error("expected to match $s->{n} pins but matched $npins")
		if ($npins != $s->{n});
	::cfg_error("expected to match $s->{n} pad cntls but matched $npads")
		if ($npads == 0 && $s->{pull_orig});
	@$pads = () if (! $s->{pull_orig});
	$s->{ra_pads} = $pads;
	$s->{ra_pins} = $pins;
}

sub output($$) {
	my ($s, $fsbl) = @_;
	return '' if (($fsbl && !$s->{fsbl}) || (!$fsbl && $s->{fsbl}));
	my %h = (st_pm => 'PMUX(SUN_TOP,',
		 ao_pm => 'PMUX(AON_PIN,',
		 cm_pm => 'PMUX(CM_TOP,',
		 st_pp => 'PMUX_PADC(SUN_TOP,',
		 ao_pp => 'PMUX_PADC(AON_PIN,',
		 cm_pp => 'PMUX_PADC(CM_TOP,',);

	my $bchp = ::get_bchp_info($s->{family});
	my $rh_pmux = $bchp->{rh_pmux};

	my $text = '';
	$text .= "\t/* $s->{where}: '$s->{cmd}' */\n";
	foreach (@{$s->{ra_pins}}, @{$s->{ra_pads}}) {
		my $pin = $rh_pmux->{$_->{type}}->{$_->{pin}};
		$text .= "\t$h{$_->{type}} $pin->{reg}, $_->{pin}, $_->{select});\n";
	}
	return $text;
}



package Enet;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Moca;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package RtsBase;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{regindex} = [];
	$s->{regbase} = [];
	bless $s, $class;
	return $s;
}

sub AddRegInfo($$$) {
	my $s = shift;
	push @{$s->{regindex}}, shift;
	push @{$s->{regbase}}, shift;
}

package RtsConfig;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package GpioKey;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Cache;
sub new($$) {
	my $class = shift;
	my $s = shift;
	$s->{id2} = ($s->{type} eq 'U') ? '-' : $s->{type};
	bless $s, $class;
	return $s;
}

package Nandshape;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{mask} = shift;
	$s->{val} = shift;
	bless $s, $class;
	return $s;
}


package Nandchip;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Sdio;
sub new($$) {
	my ($class, $s) = @_;
	bless $s, $class;
	return $s;
}

package Dts;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{filename_dts} = shift;
	$s->{loadaddr} = shift;
	bless $s, $class;
	return $s;
}

package Config;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{name} = shift;
	$s->{value} = shift;
	bless $s, $class;
	return $s;
}

package Ldfile;
sub new($$$) {
	my ($class, $s) = (shift, {});
	$s->{in} = shift;
	$s->{rename} = shift;
	bless $s, $class;
	return $s;
}


# ----------------
package Board;
use Storable qw/freeze thaw dclone/;
sub new($$) {
	my ($class, $s) = (shift, {});
	$s->{avs} = 1;
	$s->{sdio} = [];
	$s->{board_id} = undef;
	$s->{board} = "";
	$s->{rtsdefault} = 1;
	$s->{bogus} = 0;
	$s->{chip} = "";
	$s->{cmdlevel} = 3;
	$s->{comment} = "";
	$s->{config} = [];
	$s->{cset} = [];
	$s->{ddr} = [];
	$s->{drammap} = [];
	$s->{dt_ops} = [];
	$s->{dts} = undef;
	$s->{enet} = [];
	$s->{familyname} = shift;
	$s->{i2cbus} = undef;
	$s->{mset} = [];
	$s->{nandchip} = [];
	$s->{nandshape} = [];
	$s->{pmux} = [];
	$s->{rdb} = "";
	$s->{rtconfig} = [];
	$s->{gpio_key} = [];
	$s->{rtsbase} = undef;
	$s->{section} = [];
	$s->{mapselect} = "";
	$s->{memsys} = "std";
	$s->{ldfile} = [];
	bless $s, $class;
	return $s;
}

sub add($$$) {
	my ($s, $item, $part) = @_;
	my ($found, $r);

	if ($item eq 'rtsbase') {
		die "Internal error: RtsBase->new() should have been called"
			if ! defined $s->{rtsbase};
		$s->{rtsbase} = $part;
	} elsif ($item eq 'rtsconfig') {
		for $r (@{$s->{rtsconfig}}) {
			if ($r->{id} == $part->{id}) {
				$found = $r = $part;
				last;
			}
			if ($r->{file} eq $part->{file}) {
				$found = $r = $part;
				last;
			}
		}
		die("duplicate rtsconfig! $part->{file} id:$part->{id}\n")
			if ($found);
		push @{$s->{rtsconfig}}, $part;
	} elsif ($item eq 'pmux') {
		push @{$s->{pmux}}, $part;
	} elsif ($item eq 'gpio_key') {
		push @{$s->{gpio_key}}, $part;
	} elsif ($item eq 'section') {
		push @{$s->{section}}, $part;
	} elsif ($item eq 'nandshape') {
		push @{$s->{nandshape}}, $part;
	} elsif ($item eq 'ddr') {
		for $r (@{$s->{ddr}}) {
			if ($r->{n} == $part->{n}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{ddr}}, $part
			if (!$found);

	} elsif ($item eq 'mset') {
		for $r (@{$s->{mset}}) {
			if ($r->{name} eq $part->{name}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{mset}}, $part
			if (!$found);
	} elsif ($item eq 'cset') {
		for $r (@{$s->{cset}}) {
			if ($r->{name} eq $part->{name}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{cset}}, $part
			if (!$found);

	} elsif ($item eq 'dt_ops') {
		push @{$s->{dt_ops}}, $part;

	} elsif ($item eq 'enet') {
		for $r (@{$s->{enet}}) {
			if ((defined($r->{genet}) and ($r->{genet} eq $part->{genet})) or
				(defined($r->{switch_port}) and
				($r->{switch_port} eq $part->{switch_port}))) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{enet}}, $part
			if (!$found);
	} elsif ($item eq 'moca') {
		for $r (@{$s->{moca}}) {
			if ($r->{enet_node} eq $part->{enet_node}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{moca}}, $part
			if (!$found);
	} elsif ($item eq 'nandchip') {
		for $r (@{$s->{nandchip}}) {
			if ($r->{name} eq $part->{name}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{nandchip}}, $part
			if (!$found);

	} elsif ($item eq 'sdio') {
		for $r (@{$s->{sdio}}) {
			if ($r->{controller} eq $part->{controller}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{sdio}}, $part
			if (!$found);

	} elsif ($item eq 'config') {
		for $r (@{$s->{config}}) {
			if ($r->{name} eq $part->{name}) {
				$found = $r = $part;
				last;
			}
		}
		push @{$s->{config}}, $part
			if (!$found);
	} elsif ($item eq 'bid') {
		$s->{board_id} = $part;
	} elsif ($item eq 'i2cbus') {
		push @{$s->{i2cbus}}, uc($part);
	} elsif ($item eq 'ldfile') {
		push @{$s->{ldfile}}, $part;

	} else {
		die "incorrect value: '" + $item + "'";
	}
}

sub copy($) {
	return dclone(shift);
}

package main;
use strict;
use warnings;
use File::Basename;
use File::Spec;
use constant eStateInNone	=> 0;
use constant eStateInConfig	=> 1;
use constant eStateInFamily	=> 2;
use constant eStateInChip	=> 3;
use constant eStateInBoard	=> 4;

# ----------------------------------------
#       root object items & lists
# ----------------------------------------
my %boards = ();
my $nboards = 0;
my %chips = ();
my @flashmaps = ();
my @fixed_ddrs = ();
my %dt_autogen = map {($_,'')}
	qw/bolt genet serial bsc usb sdio sata memory pcie moca clocks sun_l2 gisb_arb hif_l2
	aon_pm_l2 avs_host_l2 waketimer avs_tmon sram aon_ctrl memcs mspi spi nand sun_top_ctrl
	cpu_biu_ctrl hif_cont systemport sf2_switch sun_top_ctrl_general_ctrl
	sun_top_ctrl_general_ctrl_no_scan thermal_zones cpuclock
	pinmux padmux aon_pinmux aon_padmux memc_client_info nexus_wakeups rf4ce
	bsp sdio_syscon irq0_l2 irq0_aon_l2 nexus_irq0 nexus_irq0_aon gpio pwm watchdog/;
my $Current = Board->new("");
my $Family = $Current;
my $ProcessingState = eStateInNone;

my $file_has_version = 0;
my $use_fixed_memsys_alt = 0;
my $max_gpio_keys = 0;

# family name are keys, values are pin dbase hashrefs
my %bchp_info;

# ----------------------------------------
#              helpers
# ----------------------------------------
sub get_bchp_info($) {
	my $family = shift or die;
	$family = lc $family;
	return $bchp_info{$family}
		if $bchp_info{$family};

	my @incs = ('bchp_sun_top_ctrl.h',
		'bchp_common.h',
		'bchp_aon_pin_ctrl.h',
		'bchp_uart?.h',
		'bchp_hif_cpu_intr1.h',
		'bchp_cm_top_ctrl.h',
		'bchp_sdio_?_{host,reg,cfg,boot}.h',
		'bchp_aon_ctrl.h',
		'bchp_hif_cpubiuctrl.h',
		'bchp_sun_l2.h',
		'bchp_hif_intr2.h',
		'bchp_sun_gisb_arb.h',
		'bchp_aon_pm_l2.h',
		'bchp_avs_host_l2.h',
		'bchp_ddr??_phy_control_regs_?.h',
		'bchp_shimphy_addr_cntl_?.h',
		'bchp_nand.h',
		'bchp_phy_control_regs_?.h',
		'bchp_systemport_topctrl.h',
		'bchp_systemport_tdma.h',
		'bchp_switch_acb.h',
		'bchp_switch_core.h',
		'bchp_switch_reg.h',
		'bchp_switch_fcb.h',
		'bchp_memc_arb_?.h',
		'bchp_memc_l2_0_?.h',
		'bchp_memc_l2_1_?.h',
		'bchp_memc_l2_2_?.h',
		'bchp_mc_scbarb_?.h',
		'bchp_irq0.h',
		'bchp_irq0_aon.h',
		'bchp_mac.h',
		'bchp_gio.h',
		'bchp_gio_aon.h',
		'bchp_timer.h',
		'bchp_webhif_timer.h',
		'bchp_watchdog.h',
		);

	@incs = map { "$arg_rdb_dir/$_" } @incs;

	my ($rh_defines, $ra_defines)
		= BcmUtils::grok_defines_from_c_incl_files( @incs );

	@incs = (
		'bchp_memc_clients.txt',
		);

	@incs = map { "$arg_rdb_dir/$_" } @incs;

	my @memc_client_defines = BcmUtils::grok_memc_client_names( @incs );

	my $rh_pmux
		= BcmUtils::grok_bchp_for_pin_mux($ra_defines);

	my %h = (
		phys_offset => $rh_defines->{BCHP_PHYSICAL_OFFSET},
		rh_defines => $rh_defines,
		ra_defines => $ra_defines,
		memc_client_defines => @memc_client_defines,
		rh_pmux => $rh_pmux,
		# For now, we just assume the system uses an ARM GIC (intc
		# phandle), but this can change later when we support other
		# kinds of architectures (e.g: MIPS's 7038 L1)
		l1_intc_props => {
			"name" => "intc",
			"num_cells" => "3",
			"chk_func" => \&BcmUtils::validate_gic_interrupt_cells,
			"fmt_func" => \&BcmUtils::format_gic_interrupt_cells,
		}
	);
	$bchp_info{$family} = \%h;
	return \%h;
}



sub seq_set_config() {
	cfg_error("only one [config] heading per config file!")
		if ($ProcessingState != eStateInNone);
	$ProcessingState = eStateInConfig;
}

sub seq_set_family() {
	cfg_error("only one [family] heading per config file!")
		if ($ProcessingState != eStateInNone &&
		    $ProcessingState != eStateInConfig);
	$ProcessingState = eStateInFamily;
}

sub seq_set_chip() {
	cfg_error("all [chip] headings must be done before [board] and after [family]")
		if ($ProcessingState != eStateInChip &&
		    $ProcessingState != eStateInFamily);
	$ProcessingState = eStateInChip;
}

sub seq_set_board() {
	cfg_error("all [board] headings must be done after [chip] and [family]")
		if ($ProcessingState != eStateInChip &&
		    $ProcessingState != eStateInBoard);
	$ProcessingState = eStateInBoard;
}

sub check_before_config() {
	cfg_error("non [map] function outside of its heading!")
		if ($ProcessingState != eStateInNone);
}

sub check_in_config() {
	cfg_error("non [config] function outside of its heading!")
		if ($ProcessingState != eStateInConfig);
}

sub check_in_family() {
	cfg_error("family only function outside of [family] heading!")
		if ($ProcessingState != eStateInFamily);
}

sub check_in_chip_or_board() {
	cfg_error("chip or board function outside of [chip]/[board] heading!")
		if ($ProcessingState != eStateInChip &&
		    $ProcessingState != eStateInBoard);
}

sub check_in_heading() {
	cfg_error("function outside of any heading!")
		if ($ProcessingState == eStateInNone);
}

sub is_in_family() {
	return ($ProcessingState == eStateInFamily);
}

sub check_in_board() {
	cfg_error("board function outside of [board] heading!")
		if ($ProcessingState != eStateInBoard);
}

sub check_family_onwards() {
   cfg_error("command outside of family, chip or board!")
	   if ($ProcessingState != eStateInChip &&
		   $ProcessingState != eStateInBoard &&
		   $ProcessingState != eStateInFamily);
}


# ----------------------------------------
sub get_flashmap($) {
	my ($mymap) = @_;
	for my $f (@flashmaps) {
		return $f if ($f->{name} eq $mymap);
	}
	return undef;
}

sub find_cset($)
{
	my ($name) = @_;
	for my $c (@{$Family->{cset}}) {
		return $c->{value} if $c->{name} eq $name;
	}
	return undef;
}

sub find_cfgvar($)
{
	my ($name) = @_;
	my $value = find_cset($name);
	return $value
		if defined $value;

	cfg_error("var $name was never defined, or never setup.")
}

# ----------------------------------------
#        decode section headers
# ----------------------------------------
sub heading_version($) {
	my ($f) = @_;
	my $ver = $f->[1];
	cfg_error("incorrect version: " . $ver . " != " . THIS_VERSION)
		if ($ver ne THIS_VERSION);
	$file_has_version = 1;
}

sub heading_config($) {
	my ($f) = @_;
	seq_set_config();
}

sub heading_family($) {
	my ($f) = @_;
	seq_set_family();
	die "$P: internal error"
		if ($Current->{familyname} || $Current->{board}
			|| $Current->{chip});
	$Current->{familyname} = $f->[1];
}

sub heading_chip($) {
	my ($f) = @_;
	shift @$f;
	my $t = join('', @$f);
	my @a = split /:/, $t;
	my $chipname = $a[0];

	seq_set_chip();
	cfg_error("chip '$chipname' has already been defined")
		if ($chips{$chipname});

	if (1 == scalar(@a)) {
		$chips{$chipname} = $Family->copy();
	} else {
		cfg_error("malformed chip heading")
			if (2 < scalar(@a));

		cfg_error("malformed derivation object '$a[1]'")
			if ($a[1] !~ /^(chip)\.(\S+)$/);

		my ($what, $inh_name) = ($1, $2);

		cfg_error("inherited chip '$inh_name' has not been defined")
			if (!$chips{$inh_name});
		$chips{$chipname} = $chips{$inh_name}->copy();
	}
	$chips{$chipname}->{chip} = $chipname;
	$Current = $chips{$chipname};
}


sub heading_board($) {
	my ($f) = @_;
	my $new;
	seq_set_board();
	shift @$f;
	my $t = join('', @$f);
	my @a = split /:/, $t;
	my $boardname = $a[0];
	my $bogus = 0;

	cfg_error("malformed board heading")
		if (2 != scalar(@a) && 3 != scalar(@a));

	if (3 == scalar(@a)) {
		cfg_error("malformed board heading")
			if ($a[2] ne 'BOGUS');
		$bogus = 1;
	}
	cfg_error("malformed derivation object '$a[1]'")
		if ($a[1] !~ /^(board|chip)\.(\S+)$/);

	my ($what, $inh_name) = ($1, $2);

	if ('board' eq $what) {
		cfg_error("inherited board '$inh_name' has not been defined")
			if (!$boards{$inh_name});
		$boards{$boardname} = $boards{$inh_name}->copy();

	} else {
		cfg_error("inherited chip '$inh_name' has not been defined")
			if (!$chips{$inh_name});
		$boards{$boardname} = $chips{$inh_name}->copy();
	}			
	$boards{$boardname}->{board} = $boardname;
	$boards{$boardname}->{bogus} = $bogus;
	$boards{$boardname}->{id} = $bogus ? -1 : $nboards++;
	$Current = $boards{$boardname};
}

sub heading_end($) {
	my ($f) = @_;
	check_in_chip_or_board();
}

sub heading_map($) {
	my ($f) = @_;
	check_before_config();
	for my $i (@flashmaps) {
		return if ($i->{name} eq $f->[1]);
	}
	push @flashmaps, Flashmap->new($f->[1]);
}


# ----------------------------------------
#       decode section commands
# ----------------------------------------
sub cmd_map($) {
	my ($f) = @_;
	check_before_config();
	for my $i (@flashmaps) {
		if ($i->{name} eq $f->[1]) {
			$i->Append(Partition->new($f->[2],$f->[3],$f->[4]));
			return "CMD_MAP";
		}
	}
	cfg_error("unknown/forward map heading: " . $f->[1]);
}

sub cmd_map_select($) {
	my ($f) = shift @_;
	check_family_onwards();
	cfg_error("missing mapselect rvalue!")
		if !defined $f->[1] || !length($f->[1]);
	$Current->{mapselect} = $f->[1];
}

sub cmd_dt($) {
	my ($args) = @_;
	shift(@$args);
	my $subcommand = shift @$args;
	cfg_error("must use 'dts' command before using 'dt' command")
		if (! defined $Current->{dts});
	if ($subcommand =~ /cull|prop|mac/) {
		subcmd_dt_ops($subcommand, $args);
		return;
	}
	cfg_error("unknown subcommand '${subcommand}' in dt command")
		if ($subcommand ne 'autogen');

	# process args
	my %h;
	while (@$args) {
		my $o = shift @$args;
		cfg_error("expected option, got '$o'")
			if ($o !~ /^-/);
		$h{$o} = [];
		my $count = 0;
		cfg_error("no arguments given for '$o'")
			if (!@$args || $args->[0] =~ /^-/);

		while (@$args && $args->[0] !~ /^-/) {
			push @{$h{$o}}, shift(@$args);
		}
	}

	cfg_error("missing option '-node' for 'dt autogen'")
		if (!$h{"-node"});
	my $node = $h{"-node"}->[0];

	# Some option checking may be done later on, so note our position
	# in the file.
	$h{__cfg_where__} = cfg_where();

	if ($subcommand eq 'autogen') {
		check_in_family();
		if ($node eq 'memory') {
			foreach (qw/-node -reg/) {
				cfg_error("missing option '$_' for 'dt autogen'")
					if (!defined $h{$_});
			}
			cfg_error("'$node' is not an auto generated DT node")
				if (! exists $dt_autogen{$node});
			cfg_error("'-reg' shoud have an even number of args")
				if (0 != scalar(@{$h{"-reg"}}) % 2);
			my $tmp = 0;
			map { $tmp = 1 if (! /^0x[0-9a-f]+/i) } @{$h{"-reg"}};
			cfg_error("args for '-reg' shoud be in hex form")
				if ($tmp);
		} elsif ($node eq 'clocks') {
			if (exists $h{"-cpu-div-table"}) {
				cfg_error("option '-cpu-div-table' requires an even number of elements")
					if (0 != scalar(@{$h{"-cpu-div-table"}}) % 2);
			}
		} elsif ($node eq 'bsc') {
			my @a = @{$h{"-clock-frequency"}}
				if ($h{"-clock-frequency"});
			my $re = join('|', @bsc_freqs);
			foreach (@a) {
			    cfg_error("bad arg $_ for '-clock-frequency';\nmust be one of @bsc_freqs")
				if (! /^$re$/);
                        }
		} elsif ($node eq 'serial') {
			if ($h{"-name"}) {
				# require an alias (SWBOLT-1248)
				foreach (qw/-l1intr -alias/) {
					cfg_error("missing option '$_' for 'dt autogen'")
						if (!defined $h{$_});
					$node = "named_serial_" . $h{"-name"}->[0];
				}
			}
		} elsif ($node eq 'watchdog') {
			my @b = @{$h{"-type"}}
				if ($h{"-type"});
			my @watchdog = ('upg', 'webhif', 'hyper');
			# check if type is a supported watchdog
			foreach my $ty (@b) {
				foreach my $i ( 0 .. $#watchdog) {
					last if ( $ty eq $watchdog[$i]);
					cfg_error("unknown watchdog type '$ty'")
						if ($i == $#watchdog);
				}
			}
		}
	}

	# Process options that are common to all autogen'd devices.
	if ($h{'-choose'}) {
		cfg_error("only one param required for the '-choose' option")
			if (1 < @{$h{'-choose'}});
		my @b;
		my @a = split(/,/, $h{'-choose'}->[0]);
		foreach (@a) {
			cfg_error("bad argument '$_' for '-choose' option")
				if (!/^\d+$/);
			$b[$_] = 1;
		}
		$h{'-choose'} = \@b;
	}
	$h{"-compatible"} = join('", "', @{$h{"-compatible"}})
		if ($h{"-compatible"});

	# Store node options in dt_autogen...
	$dt_autogen{$node} = \%h;

	delete $dt_autogen{$node}
		if ($h{"-undo"});
}

sub cmd_dts($) {
	my ($f) = @_;
	check_in_family();
	if (! defined $Current->{dts}) {
		if (scalar @{$f} == 2) {
			$Current->{dts} =  Dts->new($f->[1], 0);
		} elsif (scalar @{$f} == 4) {
			cfg_error("dts - expected '\@'")
				if ($f->[2] ne '@');
			$Current->{dts} = Dts->new($f->[1], $f->[3]);
		} else {
			cfg_error("dts - incorrect number of params");
		}
	}
}

sub cmd_rtsconfig($) {
	my ($f) = @_;
	check_in_family();
	shift @$f; # drop 'rtsconfig'.
	my $o = check_opts($f, [qw/id file/], []);
	$Current->add('rtsconfig', RtsConfig->new($o));
}

sub cmd_gpio_key($) {
	my ($f) = @_;
	check_in_chip_or_board();
	shift @$f;
	my $o = check_opts($f, [qw/name gpio pin code/], []);
	cfg_error("bad value for '-gpio'; must be 'upg_gio' or 'upg_gio_aon'")
		if ($o->{gpio} !~ /^upg_gio(_aon)?$/);
	cfg_error("bad value for '-pin'; must be a decimal number")
		if ($o->{pin} !~ /^\d+$/);
	cfg_error("bad value for '-code'; must be a decimal number")
		if ($o->{code} !~ /^\d+$/);
	$Current->add('gpio_key', GpioKey->new($o));
	my $n = @{$Current->{gpio_key}};
	$max_gpio_keys = $n
		if ($n > $max_gpio_keys);
}

sub cmd_rtsbase($) {
	my ($f) = @_;
	check_in_family();
	$Current->{rtsbase} = RtsBase->new()
		if (!defined $Current->{rtsbase});
	$Current->{rtsbase}->AddRegInfo($f->[1], $f->[2]);
}

sub cmd_comment($) {
	my ($f) = @_;
	check_in_heading();
	return if !@{$f};
	$Current->{comment} = "";
	shift @$f; # drop comment
	map { $Current->{comment} .= "$_ " } @$f;
}

sub cmd_ddr($) {
	my ($f) = @_;
	check_in_heading();
	shift @$f; # drop 'ddr'.
	my $o = check_opts($f, [qw/n size_mb base_mb clk size_bits width phy/], [qw/fixed custom/]);
	$Current->add('ddr', Ddr->new($o));
}


sub cmd_mset($) {
	my ($f) = @_;
	check_in_family();
	$Current->add('mset', Mset->new($f->[1], $f->[2]));
}

sub quoted_null_check($) {
	my ($v) = @_;
	return "NULL" if (!$v);
	return "\"" . $v . "\"";
}

# Third arg is optional
sub subcmd_dt_ops {
	my ($cmd, $f, $b) = @_;
	check_in_chip_or_board();
	my $o = check_opts($f, [qw/root node/], [qw/power mac prop string int bool vec addnode/]);
	my $r = DTops->new($o);
	$r->{command} = $cmd;
	$r->{root} = quoted_null_check($r->{root});
	$r->{node} = quoted_null_check($r->{node});
	$r->{prop} = quoted_null_check($r->{prop});
	$b ? $b->add('dt_ops', $r) : $Current->add('dt_ops', $r);
}

sub cmd_cset($) {
	my ($f) = @_;
	check_in_family();
	$Current->add('cset', Cset->new($f->[1], $f->[2]));
}

sub cmd_gset($) {
	my ($f) = @_;
	cmd_mset($f);
	cmd_cset($f);
}

sub cmd_enet($) {
	my ($f) = @_;
	check_in_heading();
	shift @$f; # drop 'enet'
	my $cmd = @$f[0];
	if ($cmd !~ /^-(genet|switch_port)$/) {
		cfg_error("bad arg: $cmd for enet; must be one of {genet,switch_port}");
		return;
	}
	$cmd =~ s/^-//;

	my $o;
	if ($cmd eq "genet") {
		$o = check_opts($f, [qw/genet/], [qw/phy_type mdio_mode phy_speed phy_id ethsw/]);
	} else {
		$o = check_opts($f, [qw/switch_port/], [qw/phy_type mdio_mode phy_speed phy_id ethsw/]);
	}
	$Current->add('enet', Enet->new($o));
}

sub cmd_moca($) {
	my ($f) = @_;
	check_in_heading();
	shift @$f; # drop 'moca'
	my $o = check_opts($f, [qw/enet_node/], [qw/moca_band/]);
	# Check moca_band specifier
	cfg_error("bad arg for moca_band; must be one of {highrf,midrf,wanrf,ext_d,d_low,d_high,e,f,g,h}")
		if (defined($o->{moca_band}) && $o->{moca_band} !~
			/^(-|highrf|midrf|wanrf|ext_d|d_low|d_high|e|f|g|h)$/);
	$Current->add('moca', Moca->new($o));
}


sub cmd_pmux($) {
	my ($f) = @_;
	my $cmd = join(' ', @$f);
	shift @$f; # drop 'pmux'
	my $o = check_opts($f, [qw/pin sel/], [qw/n pull/]);
	cfg_error("bad value for '-pull'; use 'none', 'up', or 'down'")
		if ($o->{pull} && $o->{pull} !~ /^(none|up|down)$/);
	cfg_error("bad value for '-n'")
		if ($o->{n} && $o->{n} !~ /^\d+$/);
	$o->{family} = $Current->{familyname};
	$o->{cmd} = $cmd;
	$o->{where} = cfg_where();
	$o->{fsbl} = is_in_family() ? 1 : 0;
	$Current->add('pmux', Pmux->new($o));
}

sub cmd_nandshape($) {
	my ($f) = @_;
	check_in_family();
	$Current->add('nandshape', Nandshape->new($f->[1], $f->[2]));
}

sub cmd_nandchip($) {
	my ($f) = @_;
	check_in_heading();
	shift @$f; # drop 'nandchip'.
	my $o = check_opts($f,
			   [qw/name mask_lo mask_hi val_lo val_hi page_size
			       block_size size oob ecc sector_size flags/], []);
	$Current->add('nandchip', Nandchip->new($o));
}

sub cmd_cmdlevel($) {
	my ($f) = @_;
	check_in_family();
	$Current->{cmdlevel} = $f->[1];
}

sub cmd_avs($) {
	my ($f) = @_;
	check_family_onwards();
	cfg_error("missing avs rvalue!")
		if !defined $f->[1];
	$Current->{avs} = $f->[1];
}

sub cmd_sdio($) {
	my ($f) = @_;
	check_family_onwards();
	shift @$f; # drop 'sdio'.
	my $o = check_opts($f, [qw/controller type/],
			   [qw/uhs host_driver_strength host_hs_driver_strength
			   card_driver_strength/]);
	$Current->add('sdio', Sdio->new($o));
}

sub annotate_vreg_name($)
{
	my ($name) = @_;
	$name =~ s/^vreg[-_]//;
	$name = 'vreg-' . $name;
	$name =~ s/[-_]pwr$//;
	$name .= '-pwr';
	return $name;
}

my %h_vreg_names;
sub cmd_vreg($)
{
	my ($f) = @_;
	check_family_onwards();
	check_in_chip_or_board();
	shift @$f; # drop 'vreg'.
	my $o = check_opts($f, [qw/name gpio pin active/], [qw/udelay uvolts/]);
	cfg_error("bad value for '-gpio'; must be 'upg_gio' or 'upg_gio_aon'")
		if ($o->{gpio} !~ /^upg_gio(_aon)?$/);
	cfg_error("bad value for '-pin'; must be a decimal number")
		if ($o->{pin} !~ /^\d+$/);
	cfg_error("bad value for '-active'; must be 'hi' or 'lo'")
		if ($o->{active} !~ /^hi|lo$/);
	cfg_error("bad value for '-udelay'; must be a decimal number")
		if ($o->{udelay} && $o->{udelay} !~ /^\d+$/);
	cfg_error("bad value for '-uvolts'; must be a decimal number")
		if ($o->{uvolts} && $o->{uvolts} !~ /^\d+$/);
	$o->{uvolts} ||= 3300000; # default value
	$o->{udelay} ||= 1000; # default value
	$o->{name} = annotate_vreg_name($o->{name});
	cfg_error("The '$o->{name} regulator has been previously defined!")
		if ($h_vreg_names{$o->{name}}
			&& $h_vreg_names{$o->{name}} eq $Current->{board});
	$h_vreg_names{$o->{name}} = $Current->{board};

	my %h_gpio_names;
	my $rh = ::get_bchp_info($Family->{familyname});
	foreach my $gio ("gio", "gio_aon") {
		my ($reg_first, $bank_size) =
			BcmDt::Devices::get_reg_range($rh, "BCHP_" . uc($gio));
		$h_gpio_names{"upg_$gio"} = sprintf("gpio\@%08x", $reg_first);
	}

	# The vreg node will be created at build time; see the 
	# end of process_dev_tree().  It will have now properties as
	# these will be added at runtime.

	# Now add the properties
	my $args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'compatible',
		-string => 'regulator-fixed'];
	subcmd_dt_ops('prop', $args);
	$args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'regulator-name',
		-string => $o->{name}];
	subcmd_dt_ops('prop', $args);
	$args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'regulator-min-microvolt',
		-int => $o->{uvolts}];
	subcmd_dt_ops('prop', $args);
	$args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'regulator-max-microvolt',
		-int => $o->{uvolts}];
	subcmd_dt_ops('prop', $args);
	$args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'enable-active-high',
		'-bool' => $o->{active} eq 'hi'];
	subcmd_dt_ops('prop', $args)
		if ($o->{active} eq 'hi');
	my $prop_val_str = sprintf("<\&/rdb/%s %d %d>", 
		$h_gpio_names{$o->{gpio}}, $o->{pin}, $o->{active} eq 'lo');
	$args = ['-root' => '/rdb', '-node' => $o->{name}, '-prop' => 'gpio',
		'-string' => $prop_val_str];
	subcmd_dt_ops('compile_prop', $args);
}
	
sub cmd_pcie($) {
	my ($f) = @_;
	check_family_onwards();
	check_in_chip_or_board();
	shift @$f; # drop 'pcie'.
	my $o = check_opts($f, [qw/controller type/], [qw/ssc gen mac bus slot pwr/]);
	# Locate the address for the pcie node.
	my $rh = ::get_bchp_info($Family->{familyname});
	$rh = $rh->{rh_defines};
	my $node = sprintf('BCHP_PCIE_%d_RC_CFG_TYPE1_REG_START', $o->{controller});
	cfg_error('bad pcie controller number')
		if !defined($rh->{$node});
	cfg_error('option for pcie gen must be 1, 2, or 3')
		if ($o->{gen} && $o->{gen} !~ /^[123]$/i);
	cfg_error('-mac argument must be 0 or 1')
		if ($o->{mac} && $o->{mac} !~ /^[01]$/);
	cfg_error('-bus argument must be a number')
		if ($o->{bus} && $o->{bus} !~ /^\d+/);
	cfg_error('-slot argument must be a number')
		if ($o->{slot} && $o->{slot} !~ /^\d+/);
	cfg_error('-mac option implies setting -slot and -bus')
		if ($o->{mac} && (!defined($o->{slot}) || !defined($o->{bus})));

	my @pwr;
	if ($o->{pwr}) {
		my @a = split /,/, $o->{pwr};
		@a = map { annotate_vreg_name($_); } @a;
		foreach my $vr (@a) {
			cfg_error("unknown voltage regulator '$vr'!")
				if (!$h_vreg_names{$vr});
		}
		@pwr = @a;
	}

	$node = sprintf('pcie@%08x', $rh->{$node} + $rh->{BCHP_PHYSICAL_OFFSET});

	if ($o->{type} eq 'nodevice') {
		# Turn this into a 'dt cull' operation.
		subcmd_dt_ops('cull', ['-root' => '/', '-node' => $node, ]);
	} else {
		# Turn this into 'dt prop' operation(s).
		my $args = ['-root' => '/', '-node' => $node,
			'-prop' => 'brcm,ssc', '-bool' => $o->{ssc}];
		subcmd_dt_ops('prop', $args)
			if (defined($o->{ssc}));

		$args = ['-root' => '/', '-node' => $node,
			'-prop' => 'brcm,gen', '-int' => $o->{gen} ];
		subcmd_dt_ops('prop', $args)
			if ($o->{gen});

		if ($o->{mac}) {
			my $tmp = sprintf("pci\@%d,%d", $o->{bus}, $o->{slot});
			$args = ['-root' => "/$node", '-node' => $tmp,
				'-prop' => 'local-mac-address', ];
			subcmd_dt_ops('mac', $args);
		}

		if (@pwr) {
			my $prop_val_str = join(',', map { "\"$_\""; } @pwr);
			$args = ['-root' => "/", '-node' => $node,
				'-prop' => "supply-names", 
				'-string' => $prop_val_str, ];
			subcmd_dt_ops('compile_prop', $args);

			$prop_val_str = join(' ', map { "\&/rdb/$_"; } @pwr);
			$args = ['-root' => "/", '-node' => $node,
				'-prop' => "supplies",
				'-string' => "<$prop_val_str>",];
			subcmd_dt_ops('compile_prop', $args);
		}
	}
}

sub cmd_usb($) {
    my ($p) = @_;
    my @args1 = @$p;
    my @args2 = @args1;

    cmd_usb_sub(0, \@args1);
    cmd_usb_sub(1, \@args2);
}

sub cmd_usb_sub($$) {
	my ($new_style, $f) = @_;
	check_family_onwards();
	check_in_chip_or_board();
	shift @$f; # drop 'usb'.
	my $o = check_opts($f, [qw/controller type/], [qw/ipp ioc bdc/]);
	# Locate the address for the usb node.
	my $rh = ::get_bchp_info($Family->{familyname});
	my $pre = sprintf("BCHP_USB%s_CTRL", $o->{controller} ? $o->{controller} : "");

	my ($usb_start, $usb_size) 
		= BcmDt::Devices::get_reg_range($rh, $pre);

	cfg_error('bad usb controller number')
		if !defined($usb_start);
	cfg_error('option for ipp must be 0 or 1')
		if (defined($o->{ipp}) && $o->{ipp} !~ /^[01]$/i);
	cfg_error('option for ioc must be 0 or 1')
		if (defined($o->{ioc}) && $o->{ioc} !~ /^[01]$/i);
	cfg_error('option for bdc must be on, off, or dual')
		if (defined($o->{bdc}) && $o->{bdc} !~ /^on|off|dual$/i);

	$rh = $rh->{rh_defines};
	my $node = sprintf('usb%s@%08x', $new_style ? "-phy" : "", $usb_start);

	if ($o->{type} eq 'nodevice') {
		# Turn this into a 'dt cull' operation.
		subcmd_dt_ops('cull', ['-root' => '/rdb', '-node' => $node, ]);
	} else {
		# Turn this into 'dt prop' operation(s).
		my $args = ['-root' => '/rdb', '-node' => $node,
			'-prop' => 'ipp', '-int' => $o->{ipp}];
		subcmd_dt_ops('prop', $args)
			if (defined($o->{ipp}));
		$args = ['-root' => '/rdb', '-node' => $node,
			'-prop' => 'ioc', '-int' => $o->{ioc}];
		subcmd_dt_ops('prop', $args)
			if (defined($o->{ioc}));
		$args = ['-root' => '/rdb', '-node' => $node,
			'-prop' => 'device', '-string' => $o->{bdc}];
		subcmd_dt_ops('prop', $args)
			if ($o->{bdc});

		if ($o->{bdc} && ($o->{bdc} =~ /^on|dual$/)) {
			my $postfix = '';
			my $root = "/rdb";
			if ($new_style) {
			    $postfix .= "_v2";
			} else {
			    $root .= "/$node";
			}
			my $i = $o->{controller};
			my $rh = get_bchp_info($Current->{familyname});
			my $n = BcmUtils::get_num_usb($rh->{rh_defines});
			my $ra_usb_info = BcmDt::Devices::add_usb
				(undef, $rh, $n, {}, "aon_pm_l2");
			my $node_bdc = sprintf "bdc%s@%s", $postfix,
				$ra_usb_info->[$i]->{bdc}->[0];
			my $node_ehci = sprintf "ehci%s@%s", $postfix,
				$ra_usb_info->[$i]->{ehci}->[1];
			my $node_ohci = sprintf "ohci%s@%s", $postfix,
				$ra_usb_info->[$i]->{ohci}->[1];

			if ($o->{bdc} eq 'on') {
				subcmd_dt_ops('cull', ['-root' => $root, '-node' => $node_ehci, ]);
				subcmd_dt_ops('cull', ['-root' => $root, '-node' => $node_ohci, ]);
			} 
			subcmd_dt_ops('prop', ['-root' => $root, '-node' => $node_bdc,
				'-prop' => 'status', '-string' => "okay",]);
		}
	}
}

sub in_preset_config_list($) {
	my ($name) = @_;
	for my $c (@configs) {
		return 1 if ($c eq $name);
	}
	return 0;
}

sub cmd_config($) {
	my ($f) = @_;
	my $tmp = "";
	my $f1 = "";
	check_in_config();
	cfg_error("unknown config '" . $f->[1] . "'")
		if (in_preset_config_list($f->[1]) == 0);
	if (scalar(@$f) > 3) {
		$f1 = $f->[1];
		shift @$f;
		shift @$f;
		map { $tmp .= "$_ " } @$f;
		$Current->add('config', Config->new($f1, $tmp));
	} else {
		$Current->add('config', Config->new($f->[1], $f->[2]));
	}
}

sub cmd_board_id($) {
	my ($f) = @_;
	check_in_board();
	$Current->{board_id} = $f->[1];
}

sub cmd_i2cbus($) {
	my ($f) = @_;
	check_in_family();
	shift(@$f);
	foreach (@$f) {
		$Current->add('i2cbus', $_);
	}
}

sub cmd_bsc($) {
	my ($f) = @_;
	check_in_heading();
	shift @$f; # drop 'bsc'
	my $o = check_opts($f, ["clock-frequency"], []);
	$Current->add('bsc', Bsc->new($o));
}

sub cmd_mmap($) {
	my ($f) = @_;
	check_in_family();
	@{$Current->{drammap}} = sort {
		$a->{memc} <=> $b->{memc} ||
		Math::BigInt->new($a->{to_bytes}) <=> Math::BigInt->new($b->{to_bytes})
	} Drammap->new($f->[1],$f->[2],$f->[3],$f->[4]),
	  @{$Current->{drammap}};
}

sub cmd_section($) {
	my ($f) = @_;
	check_in_family();
	shift(@$f);
	my $o = check_opts($f, [qw/name off size/], []);
	cfg_err("'-off' and '-size' cannot both be 'auto'")
		if ($o->{off} eq 'auto' && $o->{size} eq 'auto');
	$Current->add('section', Section->new($o));
}

sub cmd_rtsdefault($) {
	my ($f) = @_;
	check_family_onwards();
	shift(@$f);
	my $o = check_opts($f, [qw/id/], []);
	my $boxmode = scalar $o->{id};
	cfg_error("rtsdefault: id must be >= 1, and not " . $boxmode)
		if ($boxmode < 1);
	$Current->{rtsdefault} = $boxmode;
}

sub cmd_memsys($) {
	my ($f) = @_;
	check_family_onwards();
	cfg_error("missing memsys rvalue!")
		if !defined $f->[1];
	$Current->{memsys} = $f->[1];
}

sub cmd_ldfile($) {
	my ($f) = @_;
	check_in_family();
	shift(@$f);
	my $o = check_opts($f, [qw/in/], [qw/rename/]);
	cfg_error("cannot find " . $o->{in})
		if (! -e $o->{in});
	$Current->add('ldfile', Ldfile->new($o->{in}, $o->{rename}));
}

# ----------------------------------------
#          decode dispatch
# ----------------------------------------
my %headings = (
		'board' =>	\&heading_board,
		'chip' =>	\&heading_chip,
		'config' =>	\&heading_config,
		'end' =>	\&heading_end,
		'family' =>	\&heading_family,
		'map' =>	\&heading_map,
		'version' =>	\&heading_version,
	);

my %commands = (
		'avs' =>	\&cmd_avs,
		'bid' =>	\&cmd_board_id,
		'bsc' =>	\&cmd_bsc,
		'cmdlevel' =>	\&cmd_cmdlevel,
		'comment' => 	\&cmd_comment,
		'config' =>	\&cmd_config,
		'cset' =>	\&cmd_cset,
		'ddr' =>	\&cmd_ddr,
		'dt' =>		\&cmd_dt,
		'dts' =>	\&cmd_dts,
		'enet' =>	\&cmd_enet,
		'gset' =>	\&cmd_gset,
		'i2cbus' =>	\&cmd_i2cbus,
		'map' =>	\&cmd_map,
		'mapselect' =>	\&cmd_map_select,
		'memsys' =>	\&cmd_memsys,
		'mmap' =>	\&cmd_mmap,
		'moca' =>	\&cmd_moca,
		'mset' =>	\&cmd_mset,
		'nandchip' =>	\&cmd_nandchip,
		'nandshape' =>	\&cmd_nandshape,
		'pcie' =>	\&cmd_pcie,
		'pmux' =>	\&cmd_pmux,
		'rtsbase' =>	\&cmd_rtsbase,
		'rtsconfig' =>	\&cmd_rtsconfig,
		'gpio_key' =>	\&cmd_gpio_key,
		'rtsdefault' =>	\&cmd_rtsdefault,
		'section' =>	\&cmd_section,
		'sdio' =>	\&cmd_sdio,
		'usb' =>	\&cmd_usb,
		'ldfile' =>	\&cmd_ldfile,
		'vreg' =>	\&cmd_vreg,
	);


sub decode($) {
	my ($line) = @_;
 	my @items = shellwords($line);
	my $is_heading = $items[0] =~ /^\[/;
	$items[0] =~ s/^\s*\[\s*//;
	$items[-1] =~ s/\s*\]\s*$//;
	my $i = $items[0];

	if ($is_heading) {
		cfg_error("unknown heading: " . "[$i]")
			if (!$headings{$i});
		my $h = $headings{$i};
		&${h}(\@items);
	} elsif ($commands{$i}) {
		my $c = $commands{$i};
		&${c}(\@items);
	} else {
		cfg_error("unknown keyword: " . $i);
	}
}


# ----------------------------------------
#              debug
# ----------------------------------------
sub show_cs($$) {
	my ($cs, $name) = @_;
	return if !$cs;
	print "     cs: " . $name . ", " . $cs->{ftype} . "\n";
	if ($cs->{flashmap}) {
		print "'" + $cs->{flashmap}->{name} + "'\n";
		for my $m (@{$cs->{flashmap}->{maps}}) {
			print "          " . $m->{name} . "\t"
				. $m->{size} . "\n";
		}
	}
}

sub show_flash_maps() {
	print " ------------ flash maps ------------\n";
	for my $f (@flashmaps) {
		print "nmap: " . $f->{name} . "\n";
		map { print "\t$_->{name}\t$_->{size}\t$_->{device}\n" }
			@{$f->{maps}};
	}
}

sub show_boards() {
	print " ------------ boards ------------\n";
	for my $b (get_valid_boards()) {
		print " family: " . $b->{familyname} . "\n";
		print "   chip: " . $b->{chip} . "\n";
		print "  board: " . $b->{board} . "\n";
		print "    dts: " . $b->{dts} . "\n";
		print "  bogus: " . $b->{bogus} . "\n";
		for my $r (@{$b->{rtsbase}}) {
			print "    rts: #" . $r->{memc} . "\n";
			print "$r->{filename}\n";
		}
		for my $d (@{$b->{ddr}}) {
			print "    ddr: #" . $d->{n} . "\t";
			print $d->{size_mb} . "\t" . $d->{base_mb} . "\t";
			print $d->{freq_mhz} . "\t" . $d->{width} . "\t"
				. $d->{mbits_per_chip} . "\n\n";
		}
	}
}


# ----------------------------------------
#        post-process functions
# ----------------------------------------
sub get_valid_boards() {
	# Return only valid boards in the order they were creatd.
	my @a = sort { $a->{id} <=> $b->{id} } values %boards;
	while (scalar(@a) && $a[0]->{id}<0) {
		shift @a;
	}

	# SINGLE_BOARD
	if (defined $arg_single_board) {
		cfg_error($arg_single_board . " - No such board.")
			if (!defined $boards{$arg_single_board});
		return $boards{$arg_single_board};
	}

	return @a;
}


# ----------------------------------------

sub gen_flash_map($) {
	my ($fm) = @_;
	my $title = "";
	my $text = "[] = {\n";
	my $items = scalar(@{$fm->{maps}});
	for my $m (0..$items-1) {
		my $size = $fm->{maps}->[$m]->{size};
		my $growable = ($size eq "0");
		if ($growable) {
			$text .= "\tGROWABLE_PARTITION(\""
		} else {
			$text .= "\tFIXED_PARTITION(\"";
		}
		$text .= $fm->{maps}->[$m]->{name} . "\", ";
		if (!$growable) {
			$text .= mulpfix($size) . ", ";
		}
		$text .= $fm->{maps}->[$m]->{device} . ")";
		if ($m == $items - 1) {
			$text .= ",\n\t{ NULL, 0, 0, 0 } /* Terminate */\n};\n\n";
		} else {
			$text .= ",\n";
		}
	}
	return $text;
}

# ls_mcbs() extracts the available MCB names from the given chip family
#       and returns the list of extracted MCB names
#
# PARAMS:
#   $memc_rev -- MEMC_REV that is defined in the chip family config file
#   $family -- the name of the chip family
#
# RETURNS:
#   list of normalized MCB names available for the given chip family
#
sub ls_mcbs($$) {
	my ($memc_rev, $family) = @_;
	my @list;
	my $infile = "shmoo/" . $memc_rev . "/mcb-" . $family . ".c";
		# e.g. "shmoo/B130_F001/mcb-7445d0.c"

	if (! -e $infile) {
		print "$infile does not exist.\n";
		$infile = "shmoo/" . $memc_rev . "/mcb-common.c";
		print "$infile, switching to common...\n";
		# e.g. "shmoo/B130_F001/mcb-common.c"
	}

	open my $fin, "<", $infile or die "$P: Error opening $infile";
	while (<$fin>) {
		# "static const uint32_t mcb_933mhz_32b_dev4Gx16[] ..."
		# "static", "const", "uint32_t", "mcb_933mhz_32b_dev4Gx16[]",,,
		# mcb_933mhz_32b_dev4Gx16[]
		# mcb_933mhz_32b_dev4Gx16
		my @a = grep { /^static const uint32_t/ } $_;
		next if (scalar @a == 0);
		my @b = split(' ', $a[0]);
		my @c = split('\[', $b[3]);
		push(@list, $c[0]);
	}
	close($fin);

	return @list;
}

# is_mcb_available() checks if there is a matching MCB in the given MCB list
#       for the given DDR parameters
#
# PARAMS:
#   $ddr_name -- DDR parameters that have been normalized into a string
#   @mcbs -- list of available MCB names
#
# RETURNS:
#   1 if MCB is available, 0 otherwise
#
sub is_mcb_available($$) {
	my $ddr_name = $_[0];
	my @mcbs = @{$_[1]};
	my $mcb_name;

	# $ddr_name can be one of the following two formats:
	#   7445d0/custom_933mhz_32b_dev4gx16 (custom)
	#   7445d0_933mhz_32b_dev4gx16
	# Then, a matching MCB name would be:
	#   custom_933mhz_32b_dev4gx16 (custom)
	#   mcb_933mhz_32b_dev4gx16

	my @custom_name = split('/', $ddr_name);
	if (defined $custom_name[1]) {
		# "7445d0", "custom_933mhz_32b_dev4gx16"
		$mcb_name = $custom_name[1];
	} else {
		# 7445d0_933mhz_32b_dev4gx16
		my @a = split('_', $ddr_name, 2);
		# "7445d0", "933mhz_32b_dev4gx16"
		return 0 if (!defined $a[1]);
		$mcb_name = "mcb_" . $a[1];
		# mcb_933mhz_32b_dev4gx16
	}

	for my $m (@mcbs) {
		return 1 if ($mcb_name =~ /$m/i);
	}

	return 0;
}

sub gen_ddr($) {
	my ($ddr) = @_;
	my $ddr_name = "";
	my $text = "\t\t.ddr\t=\t{\n";
	my $memc_rev = find_cset("MEMC_REV");
	my $noshmoo = find_cset("CFG_NOSHMOO");

	if ((defined $memc_rev and defined $noshmoo) or
		(!defined $memc_rev and !defined $noshmoo)) {
		cfg_error("MEMC_REV and CFG_NOSHMOO cannot be defined or " .
			"undefined at the same time");
	}

	# extract the available MCB's
	my @mcbs = ls_mcbs($memc_rev, $Family->{familyname}) if (defined $memc_rev);

	for my $d (@$ddr) {
		$text .= "\t\t\t\t\t{ " . $d->{n} . ",\t";
		if ($d->{size_mb} eq '-') {
			$text .= "0,\t0,\t0,\t0,\t0,\t0,\tNULL ";
		} else {
			$text .= $d->{size_mb} . ",\t" . $d->{base_mb} . ",\t";
			$text .= mhz_remove($d->{clk}) . ",\t";
			$text .= size_in_mega($d->{size_bits}) . ",\t";
			$text .= $d->{width} . ",\t";
			$text .= $d->{phy} . ",\t";
			if (defined($d->{custom})) {
				$text .= "\"" . $d->{custom} . "\"" . "\t";
				$ddr_name  = $Family->{familyname} . "/" . $d->{custom} . "_";
				# e.g. 7445d0/custom_933mhz_32b_dev4gx16

				# SINGLE_BOARD
				# If we find a DDR4 type then mark it so later
				# on we can switch to sourcing from the MEMC_ALT
				# path for our memsysinitlib.a
				$use_fixed_memsys_alt = 1
					if ((defined $arg_single_board) &&
						($d->{custom} eq "ddr4"));
			} else {
				$text .= "NULL" . "\t";
				$ddr_name  = $Family->{familyname} . "_";
				# e.g. 7445d0_933mhz_32b_dev4gx16
			}
			$ddr_name .= $d->{clk} . "_" . $d->{phy} . "b";
			$ddr_name .= "_dev" . $d->{size_bits} . "x" . $d->{width};
			$text .= "/* " .  $ddr_name . " */";

			# SINGLE_BOARD
			# Force fixed DDR type in MEMSYS section if
			# we have a single board - MCBs will be
			# integrated into the MEMSYS section of BOLT
			# instead of the SHMOO section.
			if (defined $arg_single_board) {
				push(@fixed_ddrs, "-f $ddr_name ");
			}
			elsif (defined($d->{fixed})) {
				if (defined($d->{custom})) {
					push(@fixed_ddrs, "-u $ddr_name "); 
				} else {
					push(@fixed_ddrs, "-f $ddr_name "); 
				}
			}

			if (defined $memc_rev) {
				cfg_error("NO MCB for $ddr_name")
					if !is_mcb_available($ddr_name, \@mcbs);
			}
		}
		$text .= " },\n";
	}
	$text .= "\t\t\t\t},\n";
	return $text;
}

sub gen_comment($) {
	my ($c) = @_;
	$c =~ s/\s+$// if ($c);
	return $c ? "\t\t.comment =\t\"" . $c . "\",\n" : "";
}

sub gen_boardinfo($) {
	my ($b) = @_;
	my $text = "\t{\n";
	$text .= "\t\t.name\t=\t\"";
	$text .= $b->{board};
	$text .= "\", \n";
	return $text;
}

sub gen_board_id($) {
	my ($b) = @_;
	my $text = "\t\t.bid\t=\t";
	if ( defined $b->{board_id} ) {
		$text .= $b->{board_id};
	} else {
		$text .= "0x0";
	}
	$text .= ",\n";
	return $text;
}

sub gen_board_prid($) {
	my ($b) = @_;
	my $text = "\t\t.prid\t=\t";
	$text .= chip2hex($b->{chip});
	$text .= ", /* $b->{chip} */\n";
	return $text;
}

sub check_enet_nulls($) {
	my ($e) = @_;
	if (! $e->{phy_type})  { $e->{phy_type}  = "-"; }
	if (! $e->{mdio_mode}) { $e->{mdio_mode} = "0"; }
	if (! $e->{phy_speed}) { $e->{phy_speed} = "-"; }
	if (! $e->{phy_id})    { $e->{phy_id}    = "-"; }
	if (! $e->{ethsw})     { $e->{ethsw}     = "-"; }

	foreach (qw/phy_type mdio_mode phy_speed phy_id ethsw/) {
		if ( $e->{$_} =~ /^-$/ ) {
			$e->{$_} = "NULL";
		} else {
			$e->{$_} = '"' . $e->{$_} . '"';
		}
	}
	return $e;
}

sub check_gpio_key_nulls($) {
    my ($m) = @_;
    if (!$m->{name}) { $m->{name} = "-"; }
    if (!$m->{gpio}) { $m->{gpio} = "-"; }
    if (scalar $m->{pin} < 0) { $m->{pin} = -1; }
    if (scalar $m->{code} < 0) { $m->{code} = -1; }

    foreach (qw/name gpio/) {
        if ( $m->{$_} =~ /^-$/ ) {
            $m->{$_} = "NULL";
        } else {
            $m->{$_} =  '"' . $m->{$_} . '"';
        }
    }
    return $m;
};

sub check_moca_nulls($) {
	my ($m) = @_;
	if (!$m->{enet_node}) { $m->{enet_node} = "-"; }
	if (!$m->{moca_band}) { $m->{moca_band} = "-"; }

	foreach (qw/enet_node moca_band/) {
		if ( $m->{$_} =~ /^-$/ ) {
			$m->{$_} = "NULL";
		} else {
			$m->{$_} = '"' . $m->{$_} . '"';
		}
	}
	return $m;
};

sub gen_rtsdefault($) {
	my $v = shift;
	return "\t\t.rtsdefault = $v,\n";
}

sub gen_enet($) {
	my ($enet) = @_;
	my $text = "\t\t.enet = {\n";
	my $phy_id;
	for my $e (@$enet) {
		$e = check_enet_nulls($e);
		next if ($e->{phy_type} eq "NULL");
		# Do not accept PHY addresses that are neither PHY_ID_NONE (0x101),
		# PHY_ID_AUTO (0x100) and not in the range from 0 to 31 (inclusive)
		if ($e->{phy_id} =~ /\"([0-9a-f]+)\"/) {
			$phy_id = $1;
			die "invalid PHY address: $phy_id"
				if (($phy_id < 0 or $phy_id >= 32)
					and $phy_id != 0x100 and $phy_id != 0x101);
		}
		$text .= "\t\t\t{\n";
		if (defined($e->{genet})) {
			$text .= "\t\t\t\t.genet = " . $e->{genet} . ",\n";
			$text .= "\t\t\t\t.switch_port = -1,\n";
			$text .= "\t\t\t\t.base = BCHP_GENET_" . $e->{genet} . "_SYS_REG_START,\n";
			$text .= "\t\t\t\t.umac_base = BCHP_GENET_" . $e->{genet} . "_UMAC_REG_START,\n";
		} else {
			$text .= "\t\t\t\t.genet = -1,\n";
			$text .= "\t\t\t\t.switch_port = " . $e->{switch_port} . ",\n";
		}
		$text .= "\t\t\t\t.phy_type = " . $e->{phy_type}  . ",\n";
		if ($e->{ethsw} ne "NULL" and $e->{mdio_mode} eq "NULL") {
			$e->{mdio_mode} = "\"boot\"";
		}

		# We are probing for the PHY address, or we did specify a PHY address, which implies
		# that we want MDIO accesses to be allowed, except for MoCA, and PHY_ID_NONE (0x101)
		if (($e->{phy_id} eq "\"probe\"" or
			($e->{phy_id} ne "NULL" and $phy_id != "257" and $e->{phy_type} ne "\"MOCA\""))
			and $e->{mdio_mode} eq "NULL") {
			$e->{mdio_mode} = "\"1\"";
		}
		$text .= "\t\t\t\t.mdio_mode = " . $e->{mdio_mode} . ",\n";
		$text .= "\t\t\t\t.phy_speed = " . $e->{phy_speed} . ",\n";

		if ($e->{phy_id} eq "NULL" and ($e->{mdio_mode} ne "NULL" or $e->{ethsw} ne "NULL")) {
			$e->{phy_id} = "\"0\"";
		}
		$text .= "\t\t\t\t.phy_id = " . $e->{phy_id}    . ",\n";
		$text .= "\t\t\t\t.ethsw = " . $e->{ethsw}. ",\n";
		$text .= "\t\t\t},\n";
	}
	return $text . "\t\t\t{ -1, -1, 0, 0, NULL, NULL, NULL, NULL, NULL, }\n\t\t},\n";
}

sub gen_moca($) {
	my ($moca) = @_;
	my $text = "\t\t.moca = {\n";
	for my $m (@$moca) {
		$m = check_moca_nulls($m);
		$text .= "\t\t\t\{\n";
		$text .= "\t\t\t\t.base = BCHP_DATA_MEM_REG_START,\n";
		$text .= "\t\t\t\t.enet_node = " . $m->{enet_node} . ",\n";
		$text .= "\t\t\t\t.moca_band = " . $m->{moca_band} . ",\n";
		$text .= "\t\t\t},\n";
	}

	return $text . "\t\t\t{ 0, NULL, NULL}\n\t\t},\n";
}

sub gen_gpio_key($) {
	my ($gpio_key) = @_;
	my $text = "\t\t.gpio_key = {\n";
	my $index = 0;
	for my $m (@$gpio_key) {
		$m = check_gpio_key_nulls($m);
		$text .= "\t\t\t\{\n";
		$text .= "\t\t\t\t.name = " . $m->{name} . ",\n";
		$text .= "\t\t\t\t.gpio  = " . $m->{gpio} . ",\n";
		$text .= "\t\t\t\t.pin  = " . $m->{pin} . ",\n";
		$text .= "\t\t\t\t.code = " .  $m->{code} . ",\n";
		$text .= "\t\t\t},\n";
		$index++;
	}

	return $text . "\t\t\t{ NULL, NULL, -1, -1}\n\t\t},\n";
}

sub resolve_var
{
	my ($h, $x, $depvars) = @_;
	$depvars = {} if (!$depvars);
	my $value = $x->{value};
	return $value if ($value !~ /\$\w/);
	my $hex = '0x[0-9a-f]+';

	while ($value =~ /\$(\w+)/) {
		my $dep_var = $1;
		die "$P: recursive definition of $x->{name}!\n"
			if ($depvars->{$dep_var});
		die "$P: cannot resolve variable '\$$dep_var'!\n"
			if (!$h->{$dep_var});
		$depvars->{$dep_var} = 1;
		my $subval = resolve_var($h, $h->{$dep_var}, $depvars);
		$depvars->{$dep_var} = 0;
		$value =~ s/\$(\w+)/$subval/;
	}
	# we don't do this often so a shell call is acceptable
	my $new_value;
	if ($value =~ /^(0x[0-9a-f]+)\+(0x[0-9a-f]+)$/i) {
		# If it's a simple addition, we can recognize it 
		# with a regexp and avoid calling a shell.
		# Eg : '0xffe06000+0x00005800'
		$new_value = hex($1) + hex($2);

	} else {
		$new_value = `bash -c 'echo \$(( $value ))'`;
		die "$P: could not evaluate value of var $x->{name}!\n"
			if ($? >> 8 || $new_value !~ /^\d+$/);
	}
	$new_value = sprintf("0x%08x", $new_value);
	$x->{value} = $new_value;
	return $new_value;
}

sub resolve_exprs($)
{
	my ($a) = @_;
	my (%h);

	map { $h{$_->{name}} = $_ } @$a;
	map { resolve_var(\%h, $_) } @$a;
}

sub gen_mset($) {
	my ($mset) = @_;
	my $text = "";
	resolve_exprs($mset);
	for my $m (@$mset) {
		$text .= $m->{name} . ":=" . $m->{value} . "\n";
	}
	$text .= "\n";
	return $text;
}

sub gen_cset($) {
	my ($cset) = @_;
	my $text = "";
	resolve_exprs($cset);
	for my $c (@$cset) {
		# Do not define BCHP_REV for C code usage as it creates
		# an undefined behavior, and as such should not be used
		next if ($c->{name} eq "BCHP_REV");
		$text .= "#define " . $c->{name};
		$text .= " " . $c->{value}
			if (defined $c->{value});
		$text .= "\n";
	}
	return $text;
}

sub max_ddrs($) {
	my ($aboard) = @_;
	my $count = 0;
	for my $d (@{$aboard->{ddr}}) {
		$count += 1;
	}
	return $count;
}

sub gen_numddr($) {
	my ($b) = @_;
	my $text = "\t\t.nddr\t=\t" . max_ddrs($b) .",\n";
	return $text;
}

sub gen_avs($) {
	my ($b) = @_;
	my $text = "\t\t.avs = ";
	if ($b->{avs} eq "enable") {
		$text .= "1";
	} elsif ($b->{avs} eq "disable") {
		$text .= "0";
	} elsif ($b->{avs} eq "1") {
		$text .= "1 /* unspecified in config script, using default */";
	} else {
		cfg_error("wrong avs rvalue!");
	}
	$text .= ",\n\t";
	return $text;
}

sub gen_pinmux_fn($) {
	my $bname = shift;
	my $text = "\t\t.pinmuxfn = " . $bname . "_pinmux,\n";
	return $text;
}

sub gen_dt_ops_ptr($) {
	my $bname = shift;
	my $text = "\t\t.dt_ops = " . $bname . "_dt_ops,\n";
	return $text;
}

sub gen_flash_map_selection($) {
	my $m = shift;
	return "\t\t.mapselect = partition_profile_" . $m . ",\n";
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

sub gen_nandchip($) {
	my ($n) = @_;
	my $text = "\t{\n";
	$text .= "\t\t.name         = \"" . $n->{name} . "\",\n";
	$text .= "\t\t.id_mask      = { " . $n->{mask_lo} . ", " .  $n->{mask_hi} . " },\n";
	$text .= "\t\t.id_val       = { " . $n->{val_lo}  . ", " .  $n->{val_hi}  . " },\n";
	$text .= "\t\t.page_size    = " . $n->{page_size}  . ",\n";
	$text .= "\t\t.block_size   = " . $n->{block_size} . ",\n";
	$text .= "\t\t.size         = " . $n->{size} . ",\n";
	$text .= "\t\t.oob_sector   = " . $n->{oob} . ",\n";
	$text .= "\t\t.min_ecc_bits = " . $n->{ecc} . ",\n";
	$text .= "\t\t.ecc_codeword = " . $n->{sector_size} . ",\n";
	$text .= "\t\t.flags        = " . $n->{flags} . ",\n";
	return $text . "\t},\n";
}

sub gen_num_memc($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $nmemc = BcmUtils::get_num_memc($rh->{rh_defines});
	cfg_error("RDB problem detecting BCHP_MEMC_GEN: $nmemc < 1")
		if (scalar($nmemc) < 1);
	return $nmemc;
}

sub gen_num_genet($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $genet = BcmUtils::get_num_genet($rh->{rh_defines});
	cfg_error("unexpected script problem with get_num_genet(): $genet < 0")
		if (scalar($genet) < 0);
	return $genet;
}

sub gen_num_switch_ports($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	return BcmUtils::get_sf2_num_switch_ports($rh->{rh_defines});
}

sub gen_num_switch_phy($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	return BcmUtils::get_sf2_num_switch_phy($rh->{rh_defines});
}

sub gen_num_moca($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $moca = BcmUtils::get_num_moca($rh->{rh_defines});
	cfg_error("unexpected script problem with get_num_moca(): $moca < 0")
		if (scalar ($moca) < 0);
	return $moca;
}

sub gen_num_bsc($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $bsc = BcmUtils::get_num_bsc($rh->{rh_defines});
	die("unexpected script problem with get_num_bsc(): $bsc < 0")
		if (scalar ($bsc) < 0);
	return $bsc;
}


sub gen_num_sdio($) {
	my ($f) = @_;
	my $rh = ::get_bchp_info($f->{familyname});
	my $sdio = BcmUtils::get_num_sdio($rh->{rh_defines});
	cfg_error("unexpected script problem with get_num_sdio(): $sdio < 0")
		if (scalar ($sdio) < 0);
	return $sdio;
}

sub generate_vector_property_arrays() {
	my $text = "";

	for my $b (get_valid_boards()) {
		my $board = $b->{board};
		my $ops = $b->{dt_ops};
		next if !$ops;

		# Scan list for vector properties
		for my $c (@$ops) {
			next if (!defined $c->{vec});
			die if ("ARRAY" ne ref $c->{vec});
			die if (!$c->{prop});
			my $n = scalar(@{$c->{vec}});
			die if (0 == $n);
			my $tmp0 = $c->{node};
			my $tmp1 = $c->{prop};
			foreach ($tmp0, $tmp1) {
				s/^[\"\']//;
				s/[\"\']$//;
				s/[-,]/_/g;
				s/\@/_at_/g;
			}
			my $var_name = "${board}_${tmp0}_${tmp1}";
			$text .= sprintf "static const uint32_t %s[] = {\n", $var_name;
			my $count = 0;
			foreach my $i (@{$c->{vec}}) {
				$i = hex($i) if ($i =~ /^0x/i);
				$text .= sprintf("0x%x,", unpack("N1", pack("J1", $i)));
				$text .= ++$count % 4 ? "" : "\n";
			}
			$text .= $count % 4 ? "\n" : "";
			$text .= "};\n\n";
			# replace array ref with name of variable.
			$c->{vec} = '&' . $var_name . '[0]';
			# use the int field to indicate array size.
			$c->{int} = $n;
		}
	}
	return $text . "\n\n";
}


sub gen_dt_ops($) {
	my ($ops) = @_;
	return if (!$ops);
	my $text = "";

	for my $c (@$ops) {
		my $sval = "NULL";
		my $ival = 0;
		my $vval = 0;

		if ($c->{command} eq 'add_node') {
			; # all is good
		} elsif ($c->{command} eq 'compile_prop') {
			$c->{string} =~ s/"/\\"/g;  # escape double quote chars.
			$sval = '"' . $c->{string} . '"';
		} elsif ($c->{string}) {
			$sval = '"' . $c->{string} . '"';
			$c->{command} ="S" . $c->{command};
		} elsif (defined $c->{vec}) {
			$c->{command} ="V" . $c->{command};
			$vval = $c->{vec};
			# we use ival to indicate vector size.
			$ival = $c->{int};
		} elsif (defined $c->{int}) {
			$c->{command} ="I" . $c->{command};
			$c->{int} = hex($c->{int})
				if ($c->{int} =~ /^0x/i);
			$ival = sprintf("0x%x", $c->{int});
		} elsif (defined($c->{bool})) {
			$c->{command} ="B" . $c->{command};
			$ival = $c->{bool};
		}

		$text .= "\n\t\t{DT_OP_" . uc($c->{command}) . ", ";

		$c->{power} = "NA"
			if (!$c->{power});

		$text .= "POWER_DET_" . uc($c->{power}) . ", ";
		$text .= $c->{root} . ", ";
		$text .= $c->{node} . ", ";
		$text .= $c->{prop} . ", ";
		$text .= $sval . ", ";
		$text .= $ival . ", ";
		$text .= $vval . ", ";
		$text .= "},";
	}
	$text .= "\n\t\t";
	return $text;
}


sub gen_memsys($) {
	my ($b) = @_;
	my $text = "\t.memsys = ";

	# SINGLE_BOARD
	# Override to always tell the FSBL to use MEMSYS rather than MEMSYS_ALT
	# flash region of BOLT. If DDR4 is specified for the board then the
	# MEMSYS_ALT Shmoo code will replace the DDR3 code in MEMSYS.
	if ((defined $arg_single_board) ||
		($b->{memsys} eq "std")) {
		$text .= "0";
	} elsif ($b->{memsys} eq "alt") {
		$text .= "1";
	} else {
		cfg_error("wrong memsys rvalue! of " . $b->{memsys});
	}
	$text .= ",\n\t";
	return $text;
}

# -----------------

sub processed_flash_maps() {
	my $t = "";
	for my $f (@flashmaps) {
		$t .= "static struct partition_profile __maybe_unused partition_profile_";
		$t .= $f->{name};
		$t .= gen_flash_map($f);
	}
	return $t;
}

sub bigint_to_two_hexint($) {
	my $bigint = Math::BigInt->new(shift);
	my $lo = $bigint & 0xffffffff;
	$lo = $lo->as_hex();
	my $hi = $bigint->brsft(32)->as_hex();
	return ($hi, $lo);
}

sub bigint_is_power_of_two($)
{
	my $x = Math::BigInt->new(shift);
	return (($x != 0) && (($x & (~$x + 1)) == $x));
}

sub debug_pcie_dma_ranges($$)
{
	my ($ra_dma_ranges, $ra_scb_log_sizes) = @_;
	my @a_dma_ranges = @$ra_dma_ranges;
	my @a_scb_log_sizes = @$ra_scb_log_sizes;

	die if scalar(@a_dma_ranges) % 7 != 0;

	while (@a_dma_ranges) {
		my $scb_info = '';
		my @a_dma_range = splice(@a_dma_ranges, 0, 7);
		my (undef, $pci_hi, $pci_lo, $mem_hi, $mem_lo, $size_hi, $size_lo)
			= map { hex($_) } @a_dma_range;

		if (!$mem_hi) {
			my $log2_size = shift @a_scb_log_sizes;
			$scb_info = Math::BigInt->new(1)->blsft($log2_size - 20);
			$scb_info = $scb_info . 'MB'
		}

		printf "%2x.%08x => %2x.%08x of size %x.%08x    %s\n",
			$mem_hi, $mem_lo, $pci_hi, $pci_lo, $size_hi, $size_lo,
			$scb_info;
	}
	print "\n";
}


sub processed_pcie_dma_ranges() {
	# This is merely a flag that indicates that a specific range is iomem.
	my $pci_region_info = '0x02000000';

	my $rh = get_bchp_info($Family->{familyname});
	my $num_pcie = $dt_autogen{pcie}
		? BcmUtils::get_num_pcie($rh->{rh_defines}) : 0;
	return if !$num_pcie;

	my @pcie_nodes;
	for (my $i = 0; $i < $num_pcie; $i++) {
		my ($beg, $size) = BcmDt::Devices::get_reg_range_from_regexp(
			$rh, "^BCHP_PCIE_${i}");
		die if !$beg;
		$pcie_nodes[$i] = sprintf("pcie\@%x", $beg);
	}

	for my $b (get_valid_boards()) {
		my $mmap = $b->{drammap};
		my $ddr = $b->{ddr};

		# Check for a dummy DDR configuration.
		my $empty_ddr = 1;
		for my $d (@$ddr) {
			$empty_ddr = 0
				if $d->{size_mb} && '-' ne $d->{size_mb};
		}
		next if $empty_ddr;

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

		# @dma_ranges is a collection of mappings concatenated together.
		# Each "mapping" requires 7 integers:
		# 	1 flags
		# 	2 pcie_addr
		# 	2 cpu_addr
		# 	2 size
		# So if @dma_ranges has 21 elements, it contains three mappings:
		# @dma_ranges[0..6], @dma_ranges[7..13], @dma_ranges[14..20].
		my @dma_ranges = ();

		# @scb_log2_sizes is an array of size #mem controllers.
		# Each entry is the log2 of the size of the SCB window
		# that is mapped to PCIe space.  Note that we sometimes
		# make this window larger than there is memory in the 
		# controller as we "cover" gaps between successive 
		# memc's to preserve the identity map.
		my @scb_log2_sizes = ();

		# Our PCIe controller wants to map all system memory to the PCIe
		# space in a linear fashion.  That means all the memc0 memory
		# is mapped at PCIe0, then comes memc1 memory, then comes memc2
		# memory.  This mapping puts all of memory into a single 
		# contiguous region so that an EndPoing may access the host's
		# memory with a single BAR.  The downside of this is that 
		# we may lose the preferred identity map and Linux will have
		# to massage dma_addr_t values.  At any rate, $pcie_offset
		# is where in PCIe space the next memc region is to be mapped.
		my $pcie_offset = Math::BigInt->new(0);

		# Use this for debugging specific boards.
		my $interesting = 1 if $b->{board} eq 'xxx';

		# When we can preserve the identity mapping, we do not need the
		# dma_ranges information and $keep_dma_ranges will be 0.  When
		# we cannot, $keep_dma_ranges is 1 and we must augment the DT
		# with information so Linux knows how to massage the DMA
		# addresses.
		my $keep_dma_ranges;

		for (my $i = 0; $i < @$ddr; $i++) {
			my (@sys_addr, @pcie_addr, @size);
			my $d = $ddr->[$i];
			last if !$d->{size_mb} || '-' eq $d->{size_mb};
			my $ddr_size = Math::BigInt->new($d->{size_mb}) * 1024 * 1024;
			my $m = $map->[$i]->[0];
			my $m_next = $map->[$i+1]->[0] if $map->[$i+1];
			my $map_size = Math::BigInt->new($m->{size_bytes});
			my $pcie_scb_size = Math::BigInt->new(1024*1024);
			# Find a power of two size that contains the ddr.
			while ($pcie_scb_size < $ddr_size) {
				$pcie_scb_size *= 2;
			}
			if ($ddr_size > $map_size) {
				# If we are here we will be using the extended
				# memory for this ddr.
				my $m_ext = $map->[$i]->[1];
				die if !$m_ext;
				my $map_ext_size = Math::BigInt->new(
					$m_ext->{size_bytes});

				# Calculate the first range.
				@sys_addr = bigint_to_two_hexint(
					Math::BigInt->new($m->{to_bytes}));
				@pcie_addr = bigint_to_two_hexint($pcie_offset);
				@size = bigint_to_two_hexint($map_size);
				# Push the first range.
				push @dma_ranges, ($pci_region_info, @pcie_addr,
					@sys_addr, @size);

				# Calculate the second range.
				@pcie_addr = bigint_to_two_hexint(
					$pcie_offset + $map_size);
				@sys_addr = bigint_to_two_hexint(
					Math::BigInt->new($m_ext->{to_bytes}));
				@size = bigint_to_two_hexint(
					$ddr_size - $map_size);
				# Push the second range.
				push @dma_ranges, ($pci_region_info,
					@pcie_addr, @sys_addr, @size);

				# If we are here, keep DMA ranges as we cannot
				# support the identity map.
				$keep_dma_ranges = 1;
			} else {
				# If we are here, we are not using extended
				# memory for this ddr.
				my $m_to = Math::BigInt->new($m->{to_bytes});
				my $m_next_to = Math::BigInt->new(
					$m_next ? $m_next->{to_bytes} : 0);
				my $tmp = $ddr->[$i+1];
				my $more_mem =  ($tmp && $tmp->{size_mb}
					&& $tmp->{size_mb} ne '-');
				if ($more_mem && $m_next_to != 0
					&& $ddr_size + $m_to != $m_next_to) {
					# We want to increase the scb_size so
					# we can preserve an identity mapping
					# from sys_mem to pcie space.
					my $delta = $m_next_to - $m_to;
					die 'cannot maintain identity map!'
						if (!bigint_is_power_of_two($delta));
					$pcie_scb_size = $delta;
				}
				# Calculate the first and only range for this ddr.
				@sys_addr = bigint_to_two_hexint($m_to);
				@pcie_addr = bigint_to_two_hexint($pcie_offset);
				@size = bigint_to_two_hexint($ddr_size);
				# Push the first and only range for this ddr.
				push @dma_ranges, ($pci_region_info,
					@pcie_addr, @sys_addr, @size);
			}
			$pcie_offset += $pcie_scb_size;
			my $base = Math::BigInt->new(2);
			push @scb_log2_sizes, $pcie_scb_size->blog($base) . '';
		}

		debug_pcie_dma_ranges(\@dma_ranges, \@scb_log2_sizes)
			if $interesting;

		# Now we compute the "ranges" vectors.  We used to
		# do this at PCIe autogeneration time in Devices.pm,
		# but certain boards' configurations require the 
		# outbound PCIe location to be moved higher in
		# PCIe space.
		#
		# We start with Four segments, starting at 0xc0000000, 
		# each with size 128M.  Each outbound window is 
		# described by a range, currently a 7-tuple. The
		# numbers in the range are:
		#
		# (<pci-info>,  
		#  <pci-addr-hi>,  <pci-addr-lo>, 
		#  <cpu-addr-hi>,  <cpu-addr-lo>,
		#  <size-hi>  <size-lo>)
		#
		my @all_ranges;
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

			my @ranges;
			my $chip = $Family->{familyname};

			if ($chip =~ /^(7145|3390)/) {
				if ($num_pcie == 3) {
					# The first device will get 128MB, the 
					# other two get 64MB each.
					my @rx = BcmDt::Devices::subdivide_pcie_range(0, 2, @r1);
					my @ry = BcmDt::Devices::subdivide_pcie_range(1, 2, @r1);
					@ranges = ($i == 0)
						? @r0 : ($i == 1) ? @rx : @ry;
				} else {
					# Each device gets 128M
					@ranges = ($i == 0) ? @r0 : @r1;
				}

			} elsif ($chip =~ /^(7250|7364|7366|74371|7439|7445)/
				&& $num_pcie == 1) {
				# The only device gets the full 512M.
				@ranges = (@r0, @r1, @r2, @r3);
				
			} elsif ($chip =~ /^(7445|7439|7366)/ && $num_pcie == 2) {
				# Each device gets 256M.
				@ranges = ($i == 0) ? (@r0, @r1) : (@r2, @r3);
			} else {
				die "$P: need to address PCIe ranges for $chip";
			}
			
			if ($pcie_offset > 0xc0000000) {
				# We need to push up the location of the
				# outbound window.
				my $delta = $pcie_offset - 0xc0000000;
				my $start = scalar(@ranges) - 7;
				for (my $j = $start; $j >= 0; $j-= 7) {
					my $tmp = Math::BigInt->new($ranges[$j+2]);
					$tmp += $delta;
					my ($hi, $lo) = bigint_to_two_hexint($tmp);
					splice(@ranges, $j+1, 2, ($hi, $lo));
				}
			}
			push @all_ranges, [@ranges];
		}
		
		# Now call the appropriate functions that will have Bolt adding
		# at runtime the PCIe ranges, dma-ranges and brcm,log2-scb-sizes
		# information according to the board being used.  Not that the
		# ranges property value is different for each PCIe device, whereas
		# the other two are the same for all PCIe devices.
		for (my $i = 0; $i < $num_pcie; $i++) {
			my $node = $pcie_nodes[$i];

			# Set property 'brcm,log2-scb-sizes'.
			my $args = ['-root' => '/', '-node' => $node,
				'-prop' => 'brcm,log2-scb-sizes',
				'-vec' => \@scb_log2_sizes,];
			subcmd_dt_ops('prop', $args, $b);

			# Set property 'ranges'.
			$args = ['-root' => '/', '-node' => $node,
				'-prop' => 'ranges',
				'-vec' => $all_ranges[$i]];
			subcmd_dt_ops('prop', $args, $b);

			next if !$keep_dma_ranges;

			# Set property 'dma-ranges'.
			$args = ['-root' => '/', '-node' => $node,
				'-prop' => 'dma-ranges',
				'-vec' => \@dma_ranges,];
			subcmd_dt_ops('prop', $args, $b);
		}
	}
}


sub processed_dt_ops() {
	my $text = generate_vector_property_arrays();

	for my $b (get_valid_boards()) {
		$text .= "static const __maybe_unused dt_ops_s " . $b->{board} . "_dt_ops[] = ";
		$text .= "{" . gen_dt_ops($b->{dt_ops})
			. "{DT_OP_NONE, POWER_DET_NA, NULL, NULL, NULL, NULL, 0, NULL}\n};\n\n";
	}
	$text .= "\n";
	return $text;
}

sub processed_board_types() {
	my $text = "static const struct board_type __maybe_unused "
		. $genprefix . "board_types[] = {\n";
	for my $b (get_valid_boards()) {
		$text .= gen_boardinfo($b);
#		$text .= gen_comment($b->{comment});
		$text .= gen_board_id($b);
		$text .= gen_board_prid($b);
		$text .= gen_numddr($b);
		$text .= gen_ddr($b->{ddr});
		$text .= gen_avs($b);
		$text .= gen_memsys($b);
		$text .= "},\n";
	}
	$text .= "};\n";
	return $text;
}

sub processed_fixed_ddr($) {
	my $D = shift;
	my $b = $Family;
	my $count = scalar @fixed_ddrs;
	my $num_memc = scalar gen_num_memc($b);
	my $outputfile;

	# SINGLE_BOARD
	# If we're using DDR4 then inform the shmoo memsys.inc
	# makefile (via this namechange) so that it can pick up
	# the right (DDR4) memsysinitlib.a.
	if ($use_fixed_memsys_alt) {
		$outputfile = "$D/memsys-fixed-alt.c";
	} else {
		$outputfile = "$D/memsys-fixed.c";
	}
	unlink $outputfile;

	return
		if ($count == 0);

	# must be <= the number of MEMC h/w units on the chip.
	#  Removing this check means you can pile in more
	# into memsys but it may overflow during build and/or
	# get stomped on by signing. YOU HAVE BEEN WARNED!
	cfg_error("Too many DDRs marked as fixed! $count >= $num_memc")
		if (($count > $num_memc) && ($count > 0));

	#the non-death version of find_cfgvar()
	my $rev = find_cset("MEMC_REV");
	return
		if (!defined $rev);
	my $text = "scripts/mcb2c -a -i " . $b->{familyname} .
		" -m " . "shmoo/" . $rev . "/mcb \\\n";

	for my $f (@fixed_ddrs) {
		$text .= $f;
	}
	$text .= " -o " . $outputfile . "\n";

	do_shell($text);
	return;
}


sub gen_partinfo($) {
	my ($cs) = @_;
	my $text;
	if (defined $cs->{flashmap}) {
		$text = $genprefix . $cs->{flashmap}->{name};
	} else {
		$text = "NULL";
	}
	return $text;
}

sub processed_ssbl_pinmux() {
	my $text = "";
	for my $b (get_valid_boards()) {
		$text .= "static void  __maybe_unused "
			. $b->{board} . "_pinmux(void) {\n";
		foreach (@{$b->{pmux}}) {
			$text .= $_->output(0);
		}
		$text .= "}\n\n";
	}
	$text .= "\n";
	return $text;
}

sub processed_board_params() {
	my $text = "const ssbl_board_params  __maybe_unused "
		. $genprefix . "board_params[] = {\n";
	for my $b (get_valid_boards()) {
		$text .= "\t{\n";
		$text .= gen_rtsdefault($b->{rtsdefault});
		$text .= gen_enet($b->{enet});
		$text .= gen_moca($b->{moca});
		$text .= gen_gpio_key($b->{gpio_key});
		$text .= gen_pinmux_fn($b->{board});
		$text .= gen_dt_ops_ptr($b->{board});
		$text .= gen_flash_map_selection($b->{mapselect});
		$text .= gen_sdio($b->{sdio});
		$text .= "\n\t}, /* " . $b->{board} . " */\n";
	}
	$text .= "};\n";
	return $text;
}

sub processed_board_params_header() {
	my $text = "const ssbl_board_params  __maybe_unused "
		. $genprefix . "board_params[];\n";

	return "extern " . $text . "\n";
}

sub processed_makefile_vars() {
	my $text = gen_mset($Family->{mset});
	return $text;
}

sub processed_makefile_cvars($) {
	my $D = shift;
	my $b = $Family;
	my $text = "";
	my $nb = 0;
	$text .= gen_cset($b->{cset});
	$text .= gen_chipdefs($b->{mset});
	my $maxddr = 0;
	for $b (get_valid_boards()) {
		$nb++;
		my $m = max_ddrs($b);
		$maxddr = $m
			if ($m > $maxddr);
	}
	$text .= "#define MAX_DDR " . $maxddr . "\n";
	$text .= "#define MAX_BOARDS " . $nb . "\n";
	$text .= "#define MAX_GPIO_KEY " . $max_gpio_keys . "\n";
	$text .= "#define CFG_CMD_LEVEL " . $b->{cmdlevel} . "\n";
	if (defined $b->{dts}) {
		if ($b->{dts}->{loadaddr}) {
			$text .= "#define CFG_DEVTREE_ADDRESS "
				. $b->{dts}->{loadaddr} . "\n";
		}
	}

	#SWBOLT-1715
	$text .= "#define ALLOW_MCP_WRITE_PAIRING " .
		BcmUtils::mcp_wr_pairing_allowed($b->{familyname}) . "\n\n";

	return $text;
}

sub gen_chipdefs($) {
	my ($mset) = @_;
	my $text = "";
	my $bchp_chip = "";
	my $bchp_rev = "";
	for my $m (@$mset) {
		if ($m->{name} eq "BCHP_CHIP") {
			$bchp_chip = $m->{value};
		}
		if ($m->{name} eq "BCHP_REV") {
			$bchp_rev = $m->{value};
		}
	}
	cfg_error("missing BCHP_CHIP") if ($bchp_chip eq "");
	cfg_error("missing BCHP_REV") if ($bchp_rev eq "");

	$text .= "#define CONFIG_BCM" . $bchp_chip . "\n";
	$text .= "#define CONFIG_BCM" . $bchp_chip . uc $bchp_rev . "\n";
	return $text;
}

sub get_regbase_at_idx($$) {
	my ($b, $idx) = @_;
	my $items = scalar @{$b->{rtsbase}->{regindex}};
	for my $n (0..$items-1) {
		return $b->{rtsbase}->{regbase}->[$n]
			if ($b->{rtsbase}->{regindex}->[$n] eq $idx);
	}
	die "missing index!";
}

sub get_regindex_at_idx($$) {
	my ($b, $idx) = @_;
	my $items = scalar @{$b->{rtsbase}->{regindex}};
	for my $n (0..$items-1) {
		return $n . ''
			if ($b->{rtsbase}->{regindex}->[$n] eq $idx);
	}
	die "missing index!";
}


sub memc_array($$$$)
{
	my ($current_memc, $filename, $rts_values, $structnames) = @_;
	my $out = "";

	if ($current_memc < 0 || @$rts_values == 0) {
		return $out;
	}

	my $name = "memc${current_memc}_${filename}";

	my $text = "static uint32_t __maybe_unused $name" . "[] = {\n";
	for (my $i = 0; $i < @$rts_values; $i++) {
		if ($i % 4 == 0) {
			$out .= "\t";
		}
		$out .= sprintf("0x%08x,", $$rts_values[$i]);
		if ($i % 4 == 3) {
			$out .= "\n";
		} else {
			$out .= " ";
		}
	}
	$out .= "};\n\n";
	push(@$structnames, $name);
	my $count = @$rts_values;
	$out = $text . "\t$count, /* entries */\n" . $out;

	return $out;
}


sub processed_rtsconfig() {
	my $b = $Family;
	my $text = "\n";
	my @rtsfilestructs = ();
	my $defconfigcnt = 0;
	my $defconfig = 0;

	# each RTS file
	for my $f (@{$b->{rtsconfig}}) {
		my $ver = "";
		my $struct = "";
		$text .= "/* $f->{file} */\n";

		my $raw = read_textfile($f->{file});
		$raw =~	s/[\r]//g;
		my @araw = split(/\n/, $raw);

		# get RTS_VER from file
		foreach my $t (@araw) {
			if (($ver eq "") && ($t =~ m/#define\s+RTS_VER/)) {
				my @line = split(/\s+/, $t);
				$ver = $line[2];
			}
		}

		cfg_error("missing RTS_VER in $f->{file}")
			if ($ver eq "");

		$defconfigcnt++;

		# decode rts client list in file
		my $current_memc = -1;
		my $next_client = 0;
		my @rts_values = ();
		my @structnames = ();
		my $num_rts = 0;
		my $filename = basename($f->{file});
		$filename =~ s/[-.]/_/g;

		$struct = "\nstatic struct __maybe_unused rts s_$filename = {\n";
		$struct .= "\t$ver, /* version */\n";

		foreach my $t (@araw) {
			if ($t =~ m/BCHP_MEMC_ARB_(\d)_CLIENT_INFO_(\d+)_VAL\s+0x([0-9a-f]+)/i) {
				my ($this_memc, $client, $val) = ($1, $2, $3);
				if ($this_memc != $current_memc) {
					$num_rts++;
					$text .= memc_array($current_memc, $filename, \@rts_values, \@structnames);
					$next_client = 0;
					$current_memc = $this_memc;
					@rts_values = ();
				}

				if ($next_client != $client) {
					die "MEMC${current_memc}: expected client $next_client, got $client";
				}

				push(@rts_values, hex($val));
				$next_client = $client + 1;
			}
		}
		$text .= memc_array($current_memc, $filename, \@rts_values, \@structnames);

		$struct .= "\t$num_rts, /* number of memc clients */\n";
		$struct .= "\t" . $f->{id} . ", /* id */\n";
		
		$struct .= "\t{\n";
		foreach my $sn (@structnames) {
			$struct .= "\t\t$sn,\n";
		}
		$text .= $struct . "\t}\n};\n\n";
		
		push(@rtsfilestructs, "s_$filename");
	}

	my $rcfg = "\nstruct rts *rts_cfgs[] = {\n";
	foreach my $t (@rtsfilestructs) {
		$rcfg .= "\t&$t,\n";
	}
	$text .= $rcfg . "\tNULL\n};\n";

	$text .= "const int num_rts_cfgs = $defconfigcnt;\n";

	return $text;
}

sub processed_rtsbase() {
	my $b = $Family;
	my $text = "\n";
	$text .= "const uint32_t rts_bases[] = {\n";
	my $items = scalar @{$b->{rtsbase}->{regindex}};
	for my $n (0..$items-1) {
		my $rb = get_regbase_at_idx($b, $n);
		if ($rb eq "-") {
			$text  .= "0";
		} else {
			$text .= "\tBCHP_" . $rb;
		}
		$text  .=  ",\n";
	}
	$text  .= "};\n\n";
	$text  .= "const uint32_t rts_base_size = ARRAY_SIZE(rts_bases);\n";
	return $text;
}

sub processed_fsbl_pinmux() {
	my $b = $Family;
	my $text = "void fsbl_pinmux(void) {\n";
	for my $p (@{$b->{pmux}}) {
		$text .= $p->output(1);
	}
	$text .= "}\n";
	return $text;
}

sub processed_nandshape() {
	my $b = $Family;
	my $text = "static const struct boot_shape_mask __maybe_unused "
		. "nand_boot_shapes[] = {\n";
	for my $n (@{$b->{nandshape}}) {
		$text .= "\t{ " . $n->{mask} . ", "
			. $n->{val} ." },\n";
	}
	$text .= "};\n\n";
	return $text;
}

sub processed_nandchip() {
	my $b = $Family;
	my $text = "static const struct nand_chip __maybe_unused "
		. "known_nand_chips[] = {\n";
	for my $n (@{$b->{nandchip}}) {
		$text .= gen_nandchip($n);
	}
	$text .= "};\n\n";
	return $text;
}


sub processed_aon_history() {
	my $rh = ::get_bchp_info($Family->{familyname});
	my $regex = '(?=^((?!reserved).)*$)BCHP_AON_CTRL_RESET_HISTORY_[A-Za-z_0-9]*_MASK';
	my $aon = BcmUtils::get_rdb_fields($regex, $rh->{rh_defines});
	my $all_lengths = 0;
	my $max_length = 0;
	my $text = "static const struct aon_history __maybe_unused aon[] = {\n";
	foreach my $mask (@{$aon}) {
		my $n = $mask;
		# matches get_aon_history_fields() grep
		$n =~ s/BCHP_AON_CTRL_RESET_HISTORY_//;
		$n =~ s/_MASK//;
		# we already know its a reset
		$n =~ s/_reset//;
		$n =~ s/reset//;
		$text .= "\t{ " . $mask . ",\t\"" . $n . "\" },\n";
		my $l = length($n);
		$all_lengths += $l + 1; # str plus '\0'
		$max_length = $l if ($l > $max_length);
	}
	$text .= "\t{ 0, NULL }\n};\n\n";
	$text .= "#define AON_HISTORY_TOTAL_STRLENS " . $all_lengths . "\n";
	$text .= "#define AON_HISTORY_MAX_STRLEN " . $max_length . "\n\n";
	return $text;
}

# format: BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_[$i]_otp_option_[$field]_MASK
sub processed_otp() {
	my $text = "\n";
	$text .= "#include <bchp_sun_top_ctrl.h>\n\n";
	my $struct = "#if defined(WANT_OTP_DECODE) /* do not define in cflags! */\n";
	$struct .= "const struct otp_status __maybe_unused g_otp_status[] = {\n";
	my $rh = ::get_bchp_info($Family->{familyname});
	my $regset = "BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS";
	# assumption: <10 of these registers
	for (my $i = 0; $i < 10; $i++) {
		my $otp_reg = $regset . "_" . $i;
		my $regex = "(?=^((?!reserved).)*\$)" . $otp_reg . "_[A-Za-z_0-9]*_MASK";
		my $otp = BcmUtils::get_rdb_fields($regex, $rh->{rh_defines});
		if (scalar((@{$otp})) < 1) {
			last;
		}
		foreach my $mask (@{$otp}) {
			# don't want spare bits
			if (!($mask =~ m/spare/)) {
				my $field = $mask;
				$field =~ s/$otp_reg+[^.]//;
				$field =~ s/_MASK//;
				$text .= "#define " . uc $field . "() \\\n";
				$text .= "\t (BDEV_RD($otp_reg) & \\\n\t\t\t$mask)\n\n";
				$field =~ s/otp_option_//;
				$struct .= "\t{$otp_reg,\t$mask,\t\"$field\"},\n";
			}
		}
	}
	$struct .= "\t{0,\t0,\tNULL}\n};\n\n#endif\n\n";
	return $text . $struct;
}

sub in_user_config_list($) {
	my ($name) = @_;
	my $b = $Family;
	for my $c (@{$b->{config}}) {
		return $c if ($name eq $c->{name});
	}
	return undef;
}

sub get_config_val($) {
	my ($name) = @_;
	my $u = in_user_config_list($name);
	return $u->{value} if $u;
	return undef;
}

sub processed_config() {
	my $mtext = "# [config]\n";
	my $ctext = "/* [config] */\n";
	my (@cfg, $c);
	for $c (@configs) {
		my $n = in_user_config_list($c);
		if (defined $n) {
			$n->{name} = "CFG_" . uc($n->{name});
			if (uc($n->{value}) eq "ON") {
				# SINGLE_BOARD
				# Override BOARD_ID, setting it to "OFF"
				# as its redundant for this configuration.
				if (($n->{name} eq "CFG_BOARD_ID") &&
						(defined $arg_single_board)) {
					$n->{value} = "0";
				}
				else {
					$n->{value} = "1";
				}
				push @cfg, $n;
			} elsif (uc($n->{value}) eq "OFF") {
				$n->{value} = "0";
				push @cfg, $n;
			} elsif (uc($n->{value}) eq "DEFAULT") {
				next;
			} else {
				push @cfg, $n;
			}
		} else {
			push @cfg, Config->new("CFG_" . $c, "0");
		}
	}
	for $c (@cfg) {
		$ctext .= "#define " . $c->{name} . " " . $c->{value} . "\n";
	}
	my $n_genet = gen_num_genet($Family);
	my $n_switch_ports = gen_num_switch_ports($Family);
	$ctext .= "#define NUM_MEMC " . gen_num_memc($Family) . "\n";
	$ctext .= "#define NUM_GENET " . $n_genet . "\n";
	$ctext .= "#define NUM_SWITCH_PORTS " . $n_switch_ports . "\n";
	$ctext .= "#define NUM_SWITCH_PHY " . gen_num_switch_phy($Family) . "\n";
	$ctext .= "#define NUM_MOCA " . gen_num_moca($Family) . "\n";
	$ctext .= "#define NUM_BSC " . gen_num_bsc($Family) . "\n";
	$ctext .= "#define NUM_SDIO " . gen_num_sdio($Family) . "\n";
	$ctext .= "#define NUM_ENET " . ($n_genet, $n_switch_ports)[$n_genet < $n_switch_ports] . "\n";
	$mtext .="\n# [config]\n";
	for $c (@cfg) {
		$mtext .= $c->{name} . ":=" . $c->{value} . "\n";
	}
	$mtext .="\n";
	return ($mtext, $ctext);
}

sub processed_i2cbus() {
	return "/* no i2cbus defined */\n"
		if !defined $Family->{i2cbus};
	my $txt = "#include <bchp_common.h>\n\n";
	my $ref = $Family->{i2cbus};
	$txt .= "static const uint32_t fsbl_i2c_bus[] = {\n";
	foreach (@$ref) {
		$txt .= "\tBCHP_" . $_ . "_REG_START,\n";
	}
	$txt .= "\t0,\n};\n";
	return $txt;
}

sub find_cpupll_frequency()
{
	return scalar(ulong_remove(find_cfgvar("CHIP_TYPICAL_FREQUENCY")));
}

sub get_sram_range()
{
	return (hex(find_cfgvar("SRAM_ADDR")), hex(find_cfgvar("SRAM_LEN")));
}

sub create_rdb_parent_node()
{
	my $rdb_str = "rdb\n{\n#address-cells = <1>;\n#size-cells = <1>;\n";
	$rdb_str .= "compatible = \"simple-bus\";\n";
	$rdb_str .= "ranges = <0x00000000 0x00 0x00000000 0xffffffff>;\n";
	$rdb_str .= "};\n";
	return DevTree::node->new($rdb_str);
}

sub empty($) 
{
	my $x = shift;
	return !$x || !%$x;
}

sub process_dev_tree($)
{
	my ($family) = @_;
	my $src = $Family->{dts}->{filename_dts};
	my $dst = $Family->{dts}->{filename_dtx};
	my $familyname = $Family->{familyname};
	my $dt = DevTree::dts->new_from_str("/dts-v1/;\n/ {\n};\n");
	my $rdb = create_rdb_parent_node();
	my $chip_cpupll_frequency = find_cpupll_frequency();
	my @sram = get_sram_range();

	my $rh = get_bchp_info($familyname);
	BcmUtils::gen_irq0_int_mapping($rh->{rh_defines});
	my $num_serial = $dt_autogen{serial} ? BcmUtils::get_num_serial($rh->{rh_defines}) : 0;
	my $num_bsc = $dt_autogen{bsc} ? BcmUtils::get_num_bsc($rh->{rh_defines}) : 0;
	my $num_usb = $dt_autogen{usb} ? BcmUtils::get_num_usb($rh->{rh_defines}) : 0;
	my $num_genet = $dt_autogen{genet} ? BcmUtils::get_num_genet($rh->{rh_defines}) : 0;
	my $num_sata = $dt_autogen{sata} ? BcmUtils::get_num_sata($rh->{rh_defines}) : 0;
	my $num_sdio = $dt_autogen{sdio} ? BcmUtils::get_num_sdio($rh->{rh_defines}) : 0;
	my $num_pcie = $dt_autogen{pcie} ? BcmUtils::get_num_pcie($rh->{rh_defines}) : 0;
	my $num_moca = $dt_autogen{moca} ? BcmUtils::get_num_moca($rh->{rh_defines}) : 0;
	my $num_sun_l2 = $dt_autogen{sun_l2} ? BcmUtils::get_num_sun_l2($rh->{rh_defines}) : 0;
	my $num_gisb_arb = $dt_autogen{gisb_arb} ? BcmUtils::get_num_gisb_arb($rh->{rh_defines}) : 0;
	my $num_hif_l2 = $dt_autogen{hif_l2} ? BcmUtils::get_num_sun_l2($rh->{rh_defines}) : 0;
	my $num_aon_pm_l2 = $dt_autogen{aon_pm_l2} ? BcmUtils::get_num_aon_pm_l2($rh->{rh_defines}) : 0;
	my $num_avs_host_l2 = $dt_autogen{avs_host_l2} ? BcmUtils::get_num_avs_host_l2($rh->{rh_defines}) : 0;
	my $num_waketimer = $dt_autogen{waketimer} ? BcmUtils::get_num_waketimer($rh->{rh_defines}) : 0;
	my $num_avs_tmon = $dt_autogen{avs_tmon} ? BcmUtils::get_num_avs_tmon($rh->{rh_defines}) : 0;
	my $num_memcs = $dt_autogen{memcs} ? BcmUtils::get_num_memc($rh->{rh_defines}) : 0;
	my $num_spi = $dt_autogen{spi} ? BcmUtils::get_num_spi($rh->{rh_defines}) : 0;
	my $num_mspi = $dt_autogen{mspi} ? BcmUtils::get_num_mspi($rh->{rh_defines}) : 0;
	my $num_nand = $dt_autogen{nand} ? BcmUtils::get_num_nand($rh->{rh_defines}) : 0;
	my $num_sysport = $dt_autogen{systemport} ? BcmUtils::get_num_systemport($rh->{rh_defines}) : 0;
	my $num_sf2_switch = $dt_autogen{sf2_switch} ? BcmUtils::get_num_sf2_switch($rh->{rh_defines}) : 0;
	my $num_irq0_l2 = $dt_autogen{irq0_l2} ? BcmUtils::get_num_irq0_l2($rh->{rh_defines}, "irq0") : 0;
	my $num_irq0_aon_l2 = $dt_autogen{irq0_l2} ? BcmUtils::get_num_irq0_l2($rh->{rh_defines}, "irq0_aon") : 0;
	my $num_rf4ce = $dt_autogen{rf4ce} ? BcmUtils::get_num_rf4ce($rh->{rh_defines}) : 0;
	my $num_pwm = $dt_autogen{pwm} ? BcmUtils::get_num_pwm($rh->{rh_defines}) : 0;

	map { $dt_autogen{$_} ||= {} } keys %dt_autogen;

	# These device nodes belong at the root of the DT
	BcmDt::Devices::add_memory($dt, $rh, 1, $dt_autogen{memory})
		if (!empty($dt_autogen{memory}));
	BcmDt::Devices::add_bolt($dt, $rh);
	BcmDt::Devices::add_gic($dt, $rh, 1);
	BcmDt::Devices::add_reboot($dt, $rh);
	BcmDt::Devices::add_smpboot($dt, $rh);
	BcmDt::Devices::add_pmu($dt, $rh) if (!empty($dt_autogen{pmu}));
	BcmDt::Devices::add_nexus_wakeups($dt, $rh, $dt_autogen{nexus_wakeups})
		if !empty($dt_autogen{nexus_wakeups});
	BcmDt::Devices::add_nexus_irq0($dt, $rh, $dt_autogen{nexus_irq0})
		if !empty($dt_autogen{irq0_l2}) && !empty($dt_autogen{nexus_irq0}) &&
		    $num_irq0_l2;
	BcmDt::Devices::add_nexus_irq0_aon($dt, $rh, $dt_autogen{nexus_irq0_aon})
		if !empty($dt_autogen{irq0_aon_l2}) && !empty($dt_autogen{nexus_irq0_aon}) &&
		    $num_irq0_aon_l2;
	BcmDt::Devices::add_pcie($dt, $rh, $num_pcie, $dt_autogen{pcie}, $familyname)
		if ($num_pcie && !empty($dt_autogen{pcie}));

	# This set of the device nodes should be subnodes of the RDB parent node
	$dt->add_node($rdb);
	BcmDt::Devices::add_serial($rdb, $rh, $num_serial, $dt_autogen{serial}, get_config_val("SERIAL_BAUD_RATE"))
		if ($num_serial && !empty($dt_autogen{serial}));

	# Don't presume Tie::Hash::Regex is installed.
	my @named_serials = grep /named_serial_/, keys %dt_autogen;
	foreach my $n (@named_serials) {
		BcmDt::Devices::add_named_serial($rdb, $rh, $dt_autogen{$n});
	}

	BcmDt::Devices::add_usb_v2($rdb, $rh, $num_usb, $dt_autogen{usb}, "aon_pm_l2")
		if ($num_usb && !empty($dt_autogen{usb}));
	BcmDt::Devices::add_usb($rdb, $rh, $num_usb, $dt_autogen{usb}, "aon_pm_l2")
		if ($num_usb && !empty($dt_autogen{usb}));
	BcmDt::Devices::add_genet($rdb, $rh, $num_genet, $dt_autogen{genet}, "aon_pm_l2")
		if ($num_genet && !empty($dt_autogen{genet}));
	BcmDt::Devices::add_bsc($rdb, $rh, $num_bsc, $dt_autogen{bsc})
		if ($num_bsc && !empty($dt_autogen{bsc}));
	BcmDt::Devices::add_gpio($rdb, $rh, $dt_autogen{gpio})
		if (!empty($dt_autogen{gpio}));
	BcmDt::Devices::add_sata($rdb, $rh, $num_sata, $dt_autogen{sata})
		if ($num_sata && !empty($dt_autogen{sata}));
	BcmDt::Devices::add_sdio($rdb, $rh, $num_sdio, $dt_autogen{sdio})
		if ($num_sdio && !empty($dt_autogen{sdio}));
	BcmDt::Devices::add_moca($rdb, $rh, $num_moca, $dt_autogen{moca}, "aon_pm_l2")
		if ($num_moca && !empty($dt_autogen{moca}));
	BcmDt::Devices::add_clocks($rdb, $rh, $dt_autogen{clocks}, $chip_cpupll_frequency)
		if !empty($dt_autogen{clocks});
	BcmDt::Devices::add_l2_interrupt($rdb, $rh, $dt_autogen{sun_l2}, "sun_l2")
		if ($num_sun_l2 && !empty($dt_autogen{sun_l2}));
	BcmDt::Devices::add_l2_interrupt($rdb, $rh, $dt_autogen{hif_l2}, "hif_intr2")
		if ($num_hif_l2);
	BcmDt::Devices::add_l2_interrupt($rdb, $rh, $dt_autogen{aon_pm_l2}, "aon_pm_l2")
		if ($num_aon_pm_l2 && !empty($dt_autogen{aon_pm_l2}));
	BcmDt::Devices::add_l2_interrupt($rdb, $rh, $dt_autogen{avs_host_l2}, "avs_host_l2")
		if ($num_avs_host_l2 && !empty($dt_autogen{avs_host_l2}));
	if ($num_sun_l2) {
		BcmDt::Devices::add_gisb_arb($rdb, $rh, $dt_autogen{gisb_arb}, "sun_l2")
			if ($num_gisb_arb && !empty($dt_autogen{gisb_arb}));
	} else {
		die "$P: GISB ARB needs a L2 interrupt controller\n";
	}
	BcmDt::Devices::add_waketimer($rdb, $rh, $dt_autogen{waketimer},
		"aon_pm_l2")
		if ($num_waketimer && !empty($dt_autogen{waketimer}));
	BcmDt::Devices::add_avs_tmon($rdb, $rh, $dt_autogen{avs_tmon},
		"avs_host_l2")
		if ($num_avs_tmon && !empty($dt_autogen{avs_tmon}));
	BcmDt::Devices::add_thermal_zones($dt, $rh, $dt_autogen{thermal_zones})
		if ($num_avs_tmon && !empty($dt_autogen{thermal_zones}));
	BcmDt::Devices::add_sram($rdb, $sram[0], $sram[1], $dt_autogen{sram});
	BcmDt::Devices::add_aon_ctrl($rdb, $rh, $dt_autogen{aon_ctrl});
	BcmDt::Devices::add_memcs($rdb, $rh, $num_memcs, $dt_autogen{memcs})
		if ($num_memcs && !empty($dt_autogen{memcs}));
	BcmDt::Devices::add_spi($rdb, $rh, $dt_autogen{spi})
		if ($num_spi && !empty($dt_autogen{spi}));

	my $u = get_config_val("FLASH_DMA");
	my $flash_dma = $u && uc($u) eq "ON";
	BcmDt::Devices::add_nand($rdb, $rh, $flash_dma, "hif_intr2", $dt_autogen{nand})
		if ($num_nand && !empty($dt_autogen{nand}));

	BcmDt::Devices::add_rf4ce($rdb, $rh, $dt_autogen{rf4ce}, "rf4ce_stb",
		"aon_pm_l2", "rf4ce")
		if ($num_rf4ce && !empty($dt_autogen{rf4ce}));

	BcmDt::Devices::add_sun_top_ctrl($rdb, $rh);
	BcmDt::Devices::add_cpu_biu_ctrl($rdb, $rh, $family->{familyname});
	BcmDt::Devices::add_hif_cont($rdb, $rh);

	BcmDt::Devices::add_systemport($rdb, $rh, $dt_autogen{systemport},
		"aon_pm_l2")
		if ($num_sysport && !empty($dt_autogen{systemport}));
	BcmDt::Devices::add_sf2($rdb, $rh, $dt_autogen{sf2_switch})
		if ($num_sf2_switch && !empty($dt_autogen{sf2_switch}));

	my $clks_file = "config/clks-" . $family->{familyname} . ".plx";
	BcmDt::Devices::merge_clocks_file($rh->{rh_defines}, $dt, $clks_file)
		if -f $clks_file;

	BcmDt::Devices::add_sun_top_pin_mux_ctrl($rdb, $rh, $dt_autogen{pinmux})
		if (!empty($dt_autogen{pinmux}));
	BcmDt::Devices::add_sun_top_pad_mux_ctrl($rdb, $rh, $dt_autogen{padmux})
		if (!empty($dt_autogen{padmux}));
	BcmDt::Devices::add_aon_pin_mux_ctrl($rdb, $rh, $dt_autogen{aon_pinmux})
		if (!empty($dt_autogen{aon_pinmux}));
	BcmDt::Devices::add_aon_pad_mux_ctrl($rdb, $rh, $dt_autogen{aon_padmux})
		if (!empty($dt_autogen{aon_padmux}));
	BcmDt::Devices::add_memc_client_info($rdb, $rh, $dt_autogen{memc_client_info})
		if (!empty($dt_autogen{memc_client_info}));
	BcmDt::Devices::add_sun_top_ctrl_general_ctrl($rdb, $rh,
		$dt_autogen{sun_top_ctrl_general_ctrl})
		if (!empty($dt_autogen{sun_top_ctrl_general_ctrl}));
	BcmDt::Devices::add_sun_top_ctrl_general_ctrl_no_scan($rdb, $rh,
		$dt_autogen{sun_top_ctrl_general_ctrl_no_scan})
		if (!empty($dt_autogen{sun_top_ctrl_general_ctrl_no_scan}));

	BcmDt::Devices::add_sdio_syscon($rdb, $rh, $dt_autogen{sdio_syscon})
		if (!empty($dt_autogen{sdio_syscon}));
	BcmDt::Devices::add_l2_irq0_interrupt($rdb, $rh, $dt_autogen{irq0_l2}, "irq0")
		if (!empty($dt_autogen{irq0_l2}) && $num_irq0_l2);
	BcmDt::Devices::add_l2_irq0_interrupt($rdb, $rh,
		$dt_autogen{irq0_aon_l2}, "irq0_aon")
		if !empty($dt_autogen{irq0_aon_l2}) && $num_irq0_aon_l2;

	BcmDt::Devices::add_watchdog($rdb, $rh, $dt_autogen{watchdog}, $clks_file)
		if (!empty($dt_autogen{watchdog}));

	BcmDt::Devices::add_bsp($rdb, $rh, $dt_autogen{bsp})
		if (!empty($dt_autogen{bsp}));

	# Specify whether a clocks file exist such that we can reference clock
	# nodes or not
	BcmDt::Devices::add_mspi($rdb, $rh, $dt_autogen{mspi}, "irq0_aon",
		-f $clks_file)
		if ($num_mspi && !empty($dt_autogen{mspi}));
	BcmDt::Devices::add_pwm($rdb, $rh, $num_pwm, $dt_autogen{pwm}, -f $clks_file)
		if (!empty($dt_autogen{pwm}) && $num_pwm);

	BcmDt::Devices::add_cpu_clock($dt, $dt_autogen{cpuclock})
		if (!empty($dt_autogen{cpuclock}));

	BcmDt::Devices::final_write_all_rdb_syscon_references($dt);

	# This inserts the voltage regulator nodes at the end of the "rdb"
	# node so that it will be probed after the gpio nodes are probed.
	# We could create the nodes at runtime via the cmd_vreg(), but
	# when this is done it puts the vreg node at the top of the 
	# rdb node's subnodes.  So we insert the node here, and have
	# its properties set at runtime on a per board basis.
	if (%h_vreg_names) {
		my @vreg_nodes = map { "$_ {};" } keys %h_vreg_names;
		map { $rdb->add_node(DevTree::node->new($_)); } @vreg_nodes;
	}

	open(my $fh, '>', $dst)
		or die "$P: could not open '$dst' for writing.\n";
	print $fh "/* This file is automatically generated -- do not edit. */\n\n";
	my $dt_str = $dt->to_str();
	my $include_str = "/include/ \"$src\"\n";
	$dt_str =~ s!/dts-v1/;!$include_str!;
	print $fh $dt_str;
	close($fh);
}


sub processed_ldfile($)
{
	my ($d_out) = @_;
	my %h;
	my @a = @{$Family->{cset}};
	map { $h{"\@$_->{name}\@"} = $_->{value} } @a;
	$h{'@ODIR@'} = $d_out;
	my $memc_rev = find_cset("MEMC_ALT");

	my @ldfile = @{$Family->{ldfile}};

	foreach my $f (@ldfile) {
		my $f_in = my $f_out = $f->{in};
		my $str = "";

		if (defined $f->{rename}) {
			$f_out = $f->{rename};
		} else {
			$f_out =~ s/\.in$//;
			$f_out = basename($f_out);
		}

		open(my $fh, '<', $f_in) or die;
		# slurp file line by line
		while(<$fh>) {
			# Filter out unused sections. This
			# allows us to *not* have to update
			# all chip family config files for layout
			# changes in only one Zeus file.
			next if ((!defined $memc_rev) && ($_ =~ m/memsys_alt/));
			$str .= $_;
		}
		close $fh;

		while (my($key, $val) = each %h) {
			$str =~ s/$key/$val/gsm;
		}

		unlink "$d_out/$f_out";
		open($fh, '>', "$d_out/$f_out") or die;
		print $fh $str;
		close $fh;

		# Generate memsys_alt.lds so we present an identical
		# layout for both parts of the MEMSYS section.
		if ((defined $memc_rev) && ($f_out =~ m/memsys.lds/)) {
			$str =~ s/memsys/memsys_alt/g;
			open($fh, '>', "$d_out/memsys_alt.lds") or die;
			print $fh $str;
			close $fh;
		}
	}
}


sub processed_sections()
{
	my @sections = @{$Family->{section}};
	my ($cur, $prev);

	# Find BOLT_TEXT_ADDR
	my $bolt_text_addr;
	foreach (@{$Family->{mset}}) {
		$bolt_text_addr = hex($_->{value})
			if ($_->{name} eq 'BOLT_TEXT_ADDR');
	}
	die "$P: BOLT_TEXT_ADDR was not set in configuration"
		if (! defined $bolt_text_addr);

	# Go through the list of sections and fill in any
	# offset or size that was specified as 'auto'.
	$prev = { name => 'bogus', off => 0, size => 0};
	foreach $cur (@sections) {
		$cur->{size} = $cur->{size} eq 'auto' ? undef 
			: hex($cur->{size});
		$cur->{off} = $cur->{off} eq 'auto' ? undef 
			: hex($cur->{off});
		if (!defined $prev->{size}) {
			die "$P: section '$prev->{name}' size unknown.\n"
				if (!defined $cur->{off});
			$prev->{size} = $cur->{off} - $prev->{off};
		}
		if (!defined($cur->{off})) {
			die "$P: section '$cur->{name}' offset unknown.\n"
				if (!defined $prev->{off});
			$cur->{off} = $prev->{off} + $prev->{size};
		}
		$prev = $cur;
	}
	die "$P: must specify size for last section ($prev->{name}).\n"
		if (!defined $prev->{size});

	# Do a sanity check on the offsets values.
	$prev = { name => 'bogus', off => 0, size => 0};
	foreach $cur (@sections) {
		die "$P: offset for section $cur->{name} is wrong!\n"
			if ($cur->{off} < $prev->{off} + $prev->{size});
	}

	# Create a list of all of the variables we want to convert to
	# makefile vars and cflag defs.  If a section's name is XYZ,
	# this will create XYZ_TEXT_OFFS, XYZ_SIZE, XYZ_TEXT_ADDR.
	my @a;
	foreach $cur (@sections) {
		my $name = $cur->{name};
		push @a, ["${name}_TEXT_OFFS", sprintf("0x%08x", $cur->{off})];
		push @a, ["${name}_SIZE", sprintf("0x%08x", $cur->{size})];
		my $addr = sprintf("0x%08x", $bolt_text_addr + $cur->{off});
		push @a, ["${name}_TEXT_ADDR", $addr];
	}
	# Go through our list and put each elm on the makefile var and 
	# cflags def list.
	foreach (@a) {
		push @{$Family->{mset}}, Mset->new(@$_);
		push @{$Family->{cset}}, Cset->new(@$_);
	}
}

# Checks that the memory map doesn't go beyond 32-bit space for the first
# mapped space on any memory controller.
sub check_drammap32($) {
	my ($drammap) = @_;
	my %seen_memc;
	# This loop relies on the memory map being sorted in cmd_mmap().
	foreach (@{$drammap}) {
		my $memc = $_->{memc};
		next if exists $seen_memc{$memc};
		# TODO: change source data to get rid of unnecessary conversion
		my $to_mb = (Math::BigInt->new($_->{to_bytes}))->bdiv(1024*1024);
		my $size_mb =
			(Math::BigInt->new($_->{size_bytes}))->bdiv(1024*1024);
		cfg_error("Bad memory map: for MEMC$memc, to_mb + size_mb" .
				"($to_mb + $size_mb) exceeds 32-bit range")
			if ($to_mb + $size_mb > 4096);
		$seen_memc{$memc} = 1;
	}
}

sub processed_mmap() {
	my $b = $Family;
	my $nentries = scalar @{$b->{drammap}};
	my $text = qq(#include "addr-mapping.h"\n\n);
	$text .= "static const unsigned int NUM_DRAM_MAPPING_ENTRIES = "
		. $nentries . ";\n";
	$text .= "static const struct addr_mapping_entry __maybe_unused "
		. "dram_mapping_table[" . $nentries . "] = {\n";
	if (!@{$b->{drammap}}) {
		die "$P: Missing DRAM map.  Please see mmap in doc/build_configuration_script.txt\n"
	}
	foreach (@{$Family->{mset}}) {
		if ($_->{name} eq 'CFG_ARCH_ARM' and $_->{value} eq '1') {
			check_drammap32($b->{drammap});
			last;
		}
	}
	# The map will be sorted in cmd_mmap().
	for my $n (@{$b->{drammap}}) {
		my $from_mb =
			(Math::BigInt->new($n->{from_bytes}))->bdiv(1024*1024);
		my $to_mb = (Math::BigInt->new($n->{to_bytes}))->bdiv(1024*1024);
		my $size_mb =
			(Math::BigInt->new($n->{size_bytes}))->bdiv(1024*1024);

		$text .= "\t{\n";
		$text .= "\t\t.which   = " . $n->{memc} . ",\n";
		$text .= "\t\t.from_mb = " . $from_mb . ",\n";
		$text .= "\t\t.to_mb   = " . $to_mb .   ",\n";
		$text .= "\t\t.size_mb = " . $size_mb . ",\n";
		$text .= "\t},\n";
	}
	$text .= "};\n\n";
	return $text;
}

sub processed_memc_arb($) {
	my ($rdbdir) = shift @_;
	my $text = "";

	opendir my $hrdb, $rdbdir
	    or die "Cannot open RDB directory $!";

	my @arbfiles = grep { /memc/*/arb/ } readdir $hrdb;
	if (scalar(@arbfiles) == 0) {
		rewinddir $hrdb;
		@arbfiles = grep { /mc_scbarb_/ } readdir $hrdb;
	}

	closedir $hrdb;

	foreach (@arbfiles) {
		$text .= "#include \<" . $_ . "\>\n";
	}

	return $text;
}


sub usage($$)
{
	my ($p, $e) = @_;

	print "err: " . $e . "\n";
	print "usage: $p [options]\n";
	print " -f <chip family>\n";
	print " -c <config file> (path+file)\n";
	print " -r <rdb include dir>\n";
	print " -w <working directory> (optional)\n";
	print " -g <generated files base dir> (optional),";
	print " defaults to gen/<FAMILY>\n";
	print " -D <build object base directory> (optional),";
	print " defaults to objs/<FAMILY>\n";
	print " -b <single board name> (optional)\n";
	exit(-22); # Invalid argument
}


sub wrap_headers($)
{
	my ($din) = @_;

	opendir (D, $din) or die $!;

	while (my $f = readdir(D)) {
		next unless ((-f "$din/$f") && ($f =~ m/\.h$/));

		my $text = read_textfile("$din/$f");

		my $cname = uc($f);
		$cname =~ tr/.-/__/;
		$cname = '__' . $cname .  '__';

		my $t = $cheader . $genmarker;
		$t .= "#ifndef $cname\n#define $cname\n\n" . $text;
		$t .= "#endif /* $cname */\n";

		write_textfile("$din/$f", $t);
	}
	closedir(D);
}


sub main()
{
	my %cmdflags=();
	getopts("hf:c:g:r:w:D:b:", \%cmdflags) or
		usage($P,"bad options");

	usage($P, "none")
		if (defined $cmdflags{h});

	$arg_family = $cmdflags{f}
		if (defined $cmdflags{f});

	$arg_config_file = $cmdflags{c}
		if (defined $cmdflags{c});

	$arg_gen_dir = $cmdflags{g}
		if (defined $cmdflags{g});

	$arg_rdb_dir = $cmdflags{r}
		if (defined $cmdflags{r});

	$arg_working_dir = $cmdflags{w}
		if (defined $cmdflags{w});

	$arg_odir = $cmdflags{D}
		if (defined $cmdflags{D});

	$arg_single_board = $cmdflags{b}
		if (defined $cmdflags{b});

	usage($P, "family is required")
		if (!defined $arg_family);

	usage($P, "config file is required")
		if (!defined $arg_config_file);

	usage($P, "rdb include path is required")
		if (!defined $arg_rdb_dir);

	$arg_gen_dir = "gen"
		if (!defined $arg_gen_dir);

	if (defined $arg_working_dir) {
		chdir($arg_working_dir) or
			die "cannot change to working dir $arg_working_dir.\n";
	} else {
		my $scripts_dir = dirname(File::Spec->rel2abs(__FILE__));
		chdir("$scripts_dir/..") or
			die "cannot find Bolt root dir.\n";
	}

	my $cfgin = SlurpCfgFile->new($arg_config_file);

	# Define closures that are globally accessible
	no warnings 'closure';
	sub cfg_error($) { $cfgin->error(shift); }
	sub cfg_where($) { $cfgin->where(shift); }

	while (defined(my $i = $cfgin->get_line)) {
		my $stripped = stripcomments($i);
		decode($stripped) if ($stripped);
	}

	if ($debug & 1) {
		show_boards();
		show_flash_maps();
	}

	# ---------- START post-process ----------

	my $D = $arg_gen_dir . "/" . $Family->{familyname};

	$arg_odir = "objs/" . $Family->{familyname}
		if (!defined $arg_odir);

	die "no [version] specified!"
		if ($file_has_version != 1);

	my $text = "";

	# ----------------------------------------

	processed_sections();

	write_textfile("$D/board_types.h", processed_board_types());

	# ----------------------------------------

	write_textfile("$D/board_params.c", $cheader . $genmarker
		. $partion_header_files . $pinmux_header_files);

	$text = processed_ssbl_pinmux();
	append_textfile("$D/board_params.c",  $text);

	processed_pcie_dma_ranges();

	$text = processed_dt_ops();
	append_textfile("$D/board_params.c",  $text);

	$text = read_textfile("config/flash-partitions.c.in");
	$text .= processed_flash_maps();
	append_textfile("$D/board_params.c",  $text);

	$text = processed_board_params();
	append_textfile("$D/board_params.c", $text);

	write_textfile("$D/board_params.h", processed_board_params_header());

	# ----------------------------------------

	write_textfile("$D/include.mk",
		       $makeheader . "#" . $_genmarker . "\n\n");

	append_textfile("$D/include.mk",
			"FAMILY:=" . $Family->{familyname} . "\n");

	# ----------------------------------------

	die "DTS file not specified!"
		if (!(defined($Family->{dts})
		      && $Family->{dts}->{filename_dts}));
	my $family_dts = $Family->{dts}->{filename_dts};

	die "$P: cannot find DTS file '$family_dts'.\n"
		if (! -f $family_dts);

	(my $family_dtx = "$D/" . basename($family_dts)) =~ s/s$/x/;
	$Family->{dts}->{filename_dtx} = $family_dtx;

	process_dev_tree($Family);
	append_textfile("$D/include.mk", "DTS:="
			. $family_dtx
			. "\n\n");

	# ----------------------------------------

	write_textfile("$D/aon_history.h", processed_aon_history());

	write_textfile("$D/otp_status.h", processed_otp());

	# ----------------------------------------

	my $fout = "$D/rts.c";

	$text = $cheader . $genmarker. $rts_c_header_files;

	$text .= processed_rtsconfig();
	$text .= processed_rtsbase();

	write_textfile($fout, $text);

	# ----------------------------------------

	write_textfile("$D/memc_arb.h", processed_memc_arb($arg_rdb_dir));

	write_textfile("$D/fsbl-i2c.h", processed_i2cbus());

	# ----------------------------------------

	$text = processed_makefile_vars();
	append_textfile("$D/include.mk", $text);

	write_textfile("$D/config.h", processed_makefile_cvars($D));

	my $ctext;
	($text, $ctext)  = processed_config();
	append_textfile("$D/include.mk", $text);
	append_textfile("$D/config.h", $ctext);

	# Do this *after* any gset/cset as we
	# replace '@' delimited text with cset values.
	processed_ldfile($arg_odir);

	# ----------------------------------------

	$text = processed_fsbl_pinmux();
	write_textfile("$D/fsbl-pinmux.c", $cheader . $genmarker
		. $pinmux_header_files . $text);

	# ----------------------------------------

	write_textfile("$D/nand_chips.h", processed_nandshape());
	append_textfile("$D/nand_chips.h", processed_nandchip());

	write_textfile("$D/mmap-dram.h", processed_mmap());

	write_textfile("$D/family", $Family->{familyname});

	processed_fixed_ddr("$D");

	# prevent multiple inclusion of generated headers.
	wrap_headers("$D");

	return 0;
}

exit main();
