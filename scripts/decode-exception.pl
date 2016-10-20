#!/usr/bin/perl -w
# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************

# Decode an exception dump output by the BOLT console UART, e.g.
#
# CPU exception: ABT
# r0     : 07220000
# r1     : 00000000
# r2     : 00000000
# r3     : 00000000
# r4     : 00000003
# r5     : 07061c30
# r6     : 07061c3c
# r7     : 07061c34
# r8     : 00000000
# r9     : 07061a1c
# r10     : 00000000
# r11     : 00000000
# r12     : 00000000
# r13 (sp): 091ffb10
# r14 (lr): 0703139a
# r15 (pc): 07031392
# cpsr    : 800001d3
# dfsr    : 00000007
# dfar    : 00000010
# ifar    : 00000000
#
# Mainly for use by SSBL, though can be used for FSBL problems.
#
# Example:
#
# ./scripts/decode-exception.pl -f bolt_uart_bad.log -k
#
# cat data | ./scripts/decode-exception.pl
#
# ./scripts/decode-exception.pl -f bolt_uart_bad.log -g gen/7271a0 -d -k
#
# Add '-g' if you are in the BOLT root dir and gen/$FAMILY files
# are present. Add '-d' to that if you also have the build objects
# e.g. objs/$FAMILY/ssbl.o present.
#
# ====> IMPORTANT: The exception dump should be well formed!

use strict;
use warnings FATAL=>q(all);
use File::Basename;
use Getopt::Std;
use Data::Dumper;
my $P = basename $0;


my $fh;
my $fname = undef;
my $gendir = undef;
my $keep_going = 0;
my $super_source_dump_mode = 0;
my @unasm_text = ();
my $rdb_start = 0;
my $rdb_end = 0;
my %bchp_info = ();

my %cmdflags=();
my %extra_defines = ();
my $FAMILY = undef;


# -----------------------------------------------------
# Utils

sub usage()
{
	print " help: $P [options]\n";
	print "  -h ................ This help message\n";
	print "  -f <file> ......... BOLT console logfile\n";
	print "  -g <dir> .......... BOLT gen/FAMILY directory, if known\n";
	print "  -k ................ Try harder to keep going\n";
	print "  -d ................ Dump source, if avalable. Requires -g\n";
}


sub usage_exit($)
{
	my $x = shift @_;
	usage();
	exit $x;
}


sub failed($)
{
	my $x = shift @_;
	print " *** fail: " . $x . "\n";
	return
		if ($keep_going);
	usage_exit(-1);
}


sub strim
{
	my $s = shift;
	$s =~ s/^\s+|\s+$//g;
	return $s
}


sub regtrim($) {
	my $s = shift;
	my @f = split /:/, $s;

	failed("Missing or malformed register line? \"$s\"")
		if (! defined $f[1]);

	my $r = strim($f[1]);

	if ($r =~ /^0[xX][0-9a-fA-F]+$/) {
		return hex $r;
	}

	if ($r =~ /^0[0-9a-fA-F]+$/) {
		$r =~ s/^0//;
	}

	return scalar hex("0x" . $r);
}


# for get_extra_defines
sub left_def_trim {
	my $s = strim(shift);
	$s =~ s/^-D//g;
	$s =~ s/\=.*//g;
	return strim($s)
}


sub right_def_trim {
	my $s = strim(shift);
	$s =~ s/^-D//g;
	$s =~ s/.*\=//g;
	return strim($s)
}


# for get_config_defines
sub left_h_trim {
	my $s = strim(shift);
	$s =~ s/^\#define//g;
	$s = strim($s);
	$s =~ s/(\s+).*//g;
	return strim($s)
}


sub right_h_trim {
	my $s = shift;
	$s =~ s/^\#define//g;
	$s =~ s/.*(\s+)//g;
	return strim($s)
}


sub read_textfile($) {
	my $f = shift;
	open(FH, '<', $f)
		or failed("could not open file '$fname'");
	return join('',<FH>);
}

sub do_shell($)
{
	my $cmd = shift;
	my @a = `$cmd`;
	failed("$cmd (non-zero exit code)")
		if (($? >> 8) & 0xff);
	return wantarray ? @a : join("", @a);
}

# -----------------------------------------------------
# CFG

sub cfg_is($)
{
	my $ref = shift;

	return 0
		if (! defined $extra_defines{$ref});

	return 0
		if (0 == $extra_defines{$ref});

	return 1;
}


sub cfg_value($)
{
	my $ref = shift;

	return undef
		if (! defined $extra_defines{$ref});

	return scalar($extra_defines{$ref});
}

sub get_extra_defines()
{
	# .common_flags only on swbolt-1810 branch for now
	return;

	my $f = $gendir . "/.common_flags";

	failed( $f . " does not exist")
		if (! -f $f);

	my @lines = split / /, read_textfile($f);

	foreach my $i (@lines)
	{
		$i = strim($i);
		if ($i =~ /^-D/) {
			my $l = left_def_trim($i);
			my $r =  right_def_trim($i);
			if ($l ne $r) {
				$extra_defines{$l} = $r;
			} else {
				# mark as enabled
				$extra_defines{$l} = 1;
			}
		}
	}
}


sub get_config_defines()
{
	my $f = $gendir . "/config.h";

	failed( $f . " does not exist")
		if (! -f $f);

	my @lines = split /\n/, read_textfile($f);

	foreach my $i (@lines)
	{
		$i = strim($i);
		if ($i =~ /#define/) {
			my $l = left_h_trim($i);
			my $r = right_h_trim($i);
			if ($l ne $r) {
				if ($r =~ /^0[xX][0-9a-fA-F]+$/) {
					$extra_defines{$l} = hex $r;
				} else {
					$extra_defines{$l} = $r;
				}
			} else {
				# mark as enabled
				$extra_defines{$l} = 1;
			}
		}
	}
}

# -----------------------------------------------------

sub __address_sane($$$$$$$)
{
	my ($addr, $start, $end, $cfg_start, $cfg_end, $section, $doprint) = @_;

	if ($addr < $start) {
		printf("\t!!! %s address is LESS than %s (0x%08x < 0x%08x)",
					$section, $cfg_start, $addr, $start)
			if ($doprint);
		return -1;
	}

	if ($addr > $end) {
		printf("\t!!! %s address is MORE than %s+%s (0x%08x > 0x%08x)",
				$section, $cfg_start, $cfg_end, $addr, $end)
			if ($doprint);
		return +1;
	}

	printf("\t%s address looks sane [0x%08x ... 0x%08x]",
				$section, $start, $end)
			if ($doprint);
	return 0;
}


sub report_is_address_sane($$$$)
{
	my ($addr, $cfg_start, $cfg_end, $section) = @_;

	my $start = cfg_value($cfg_start);
	my $end = $start + cfg_value($cfg_end);

	__address_sane($addr, $start, $end, $cfg_start,
			$cfg_end, $section, 1);
}


sub is_address_sane($$$$)
{
	my ($addr, $cfg_start, $cfg_end, $section) = @_;

	my $start = cfg_value($cfg_start);
	my $end = $start + cfg_value($cfg_end);

	return __address_sane($addr, $start, $end, $cfg_start,
			$cfg_end, $section, 0);
}


#  - - - - - - - - - - - - - - - - - - - - - - - - - -

sub report_is_ssbl_instruction_address_sane($)
{
	my $addr = shift;

	return
		if (! defined $gendir);

	report_is_address_sane($addr, "SSBL_RAM_ADDR", "SSBL_SIZE",
			"SSBL .text ");
}


sub is_any_bolt_instruction_address_sane($)
{
	my $addr = shift;

	return
		if (! defined $gendir);

	my $s = is_address_sane($addr,
		"SSBL_RAM_ADDR", "SSBL_SIZE", "");

	my $f = is_address_sane($addr,
		"SRAM_ADDR", "SRAM_LEN", "");

	return $f + $s;
}


#  - - - - - - - - - - - - - - - - - - - - - - - - - -

my $stacktop = 0;
my $stackbottom	= 0;

my $mem_heapstart = 0;
my $mem_heapend = 0;

sub align_up($$)
{
	my ($addr, $by) = @_;

	return (($addr + $by) - 1) & ~($by - 1);
}

# mem_heapstart = ALIGN_UP_TO(_end, CFG_ALIGN_SIZE);
# stacktop = mem_heapstart + (__STACK_SIZE + __HEAP_SIZE);
sub init_find_stacktop()
{
	return
		if (! defined $gendir);

	my $align_size = eval cfg_value("CFG_ALIGN_SIZE");

	my $max_ssbl_size = eval(cfg_value("SSBL_RAM_ADDR")
					+ cfg_value("SSBL_SIZE"));

	$mem_heapstart = align_up($max_ssbl_size, $align_size);

	my $stack_size = eval cfg_value("CFG_STACK_SIZE");
	my $heap_size = eval cfg_value("CFG_HEAP_SIZE");

	$mem_heapend = $mem_heapstart + align_up($heap_size, $align_size);

	$stacktop = $mem_heapstart +
			align_up($stack_size, $align_size) +
			align_up($heap_size, $align_size);

	$stackbottom = $stacktop - align_up($stack_size, $align_size);
}


sub is_stack_address_sane($)
{
	my $addr = shift;

	return
		if (! defined $gendir);

	__address_sane($addr, $stackbottom, $stacktop,
			"stack bottom", "stack top", "SSBL .stack", 1);
}


#  - - - - - - - - - - - - - - - - - - - - - - - - - -

sub notski($$$$)
{
	my ($rc, $text, $f, $t) = @_;

	print "\t ";
	if ($rc != 0) {
		printf(" - Not in %s (0x%08x - 0x%08x)", $text, $f, $t);
	} else {
		printf("** In %s (0x%08x - 0x%08x)", $text, $f, $t);
	}
	print "\n";
}

sub find_rdb_reg_by_addr($);

sub report_is_bolt_address_sane($)
{
	my $addr = shift;
	my $rc;

	return
		if (! defined $gendir);

	# BOLT normal access may be one of:
	# * From SSBL_PAGE_TABLE to stacktop.
	# * Flash/EBI @ 0xe0000000 (BOLT_TEXT_ADDR) to += ~256MiB
	# * RDB (BCHP_PHYSICAL_OFFSET): BCHP_REGISTER_START to BCHP_REGISTER_END
	# * SRAM

	my $bolt_start = cfg_value("SSBL_PAGE_TABLE");
	$rc = __address_sane($addr, $bolt_start, $stacktop,
				"", "", "", 0);
	notski($rc, "SSBL program or data areas", $bolt_start, $stacktop);

	$rc = __address_sane($addr, $stackbottom, $stacktop,
				"", "", "", 0);
	notski($rc, "SSBL stack", $stackbottom, $stacktop);

	my $ssbl_start = cfg_value("SSBL_RAM_ADDR");
	my $ssbl_end   = $ssbl_start + cfg_value("SSBL_SIZE");
	$rc = __address_sane($addr, $ssbl_start, $ssbl_end,
				"", "", "", 0);
	notski($rc, "SSBL program code", $ssbl_start, $ssbl_end);

	# EBI, hard wired for now.
	my $ebi_base = 0xE0000000;
	my $ebi_top = $ebi_base + 256 * (1024 * 1024);

	$rc = __address_sane($addr, $ebi_base, $ebi_top,
				"", "", "", 0);
	notski($rc, "EBI/FLASH area",  $ebi_base, $ebi_top);

	my $sram_base = cfg_value("SRAM_ADDR");
	my $sram_top = $sram_base + cfg_value("SRAM_LEN");
	$rc = __address_sane($addr, $sram_base, $sram_top,
				"", "", "", 0);
	notski($rc, "SRAM", $sram_base, $sram_top);

	$rc = __address_sane($addr, $mem_heapstart, $mem_heapend,
				"", "", "", 0);
	notski($rc, "HEAP", $mem_heapstart, $mem_heapend );

	if (($rdb_start != 0) && ($rdb_end != 0)) {
		$rc = __address_sane($addr, $rdb_start, $rdb_end,
				"", "", "", 0);
		notski($rc, "RDB", $rdb_start, $rdb_end);
		if ($rc == 0) {
			my $rdb_name = find_rdb_reg_by_addr($addr);
			if (defined $rdb_name) {
				printf("\t\t-> %s @ 0x%08x\n",
					$rdb_name, $addr);
			} else {
				printf("\t\t-- Register location not found" .
					" in BOLT RDB\n");
			}
		}
	}

	print "\n";
}


sub decode_data_fault_address($)
{
	my $addr = shift;

	if ($addr == 0x0) {
		print "\t - NULL: Either not a data fault, " .
			"or a NULL pointer.\n";
	}

	if ($addr < 0x1000) {
		print "\t - Less than 4KiB: Suggests a " .
			"NULL pointer to struct deref.\n";
	}

	report_is_bolt_address_sane($addr);
}


# -----------------------------------------------------

sub cpsr_1bitfield($$$)
{
	my ($name, $cpsr, $mask) = @_;

	print "$name=";

	if ($cpsr & $mask) {
		print "1";
	} else {
		print "0";
	}
	print " ";
}


sub decode_cpsr_mode($)
{
	my $c = shift;

	$c = $c & 0x1f;

	return "USR"
		if ($c == 0x10);
	return "FIQ"
		if ($c == 0x11);
	return "IRQ"
		if ($c == 0x12);
	return "SVC"
		if ($c == 0x13);
	return "MON"
		if ($c == 0x16);
	return "ABT"
		if ($c == 0x17);
	return "HYP"
		if ($c == 0x1a);
	return "UND"
		if ($c == 0x1b);
	return "SYS"
		if ($c == 0x1f);

	return "???";
}


sub decode_cpsr($)
{
	my $c = shift;

	print "CPSR:   mode=" . decode_cpsr_mode($c) . ", ";

	cpsr_1bitfield("T",      $c, (0x01 << 5));
	cpsr_1bitfield("FIQ",    $c, (0x01 << 6));
	cpsr_1bitfield("IRQ",    $c, (0x01 << 7));
	cpsr_1bitfield("ASYNC",  $c, (0x01 << 8));
	cpsr_1bitfield("ENDIAN", $c, (0x01 << 9));

	cpsr_1bitfield("Q",      $c, (0x01 << 27));
	cpsr_1bitfield("V",      $c, (0x01 << 28));
	cpsr_1bitfield("C",      $c, (0x01 << 29));
	cpsr_1bitfield("Z",      $c, (0x01 << 30));
	cpsr_1bitfield("N",      $c, (0x01 << 31));

	printf(" (0x%08x)\n", $c);
}


#  - - - - - - - - - - - - - - - - - - - - - - - - - -

sub sep() {
	print "---------------------------------------------------\n";
}


# http://infocenter.arm.com/help/index.jsp?
# topic=/com.arm.doc.ddi0438d/BABFFDFD.html
# Table 4.60. DFSR bit assignments for Short-descriptor translation
# table format.
# NOTE: The "long-descriptor" translation is not done.

sub fault2string($) {
	my $f = shift;

	# b00001
	return "Alignment fault"
		if ($f == 0x01);

	# b00100
	return "Instruction cache maintenance fault (should not for A15)"
		if ($f == 0x04);

	# b01100
	return "Synchronous external abort on translation table walk, 1st level"
		if ($f == 0x0c);

	# b01110
	return "Synchronous external abort on translation table walk, 2nd level"
		if ($f == 0x0e);

	# b11100
	return "ynchronous parity error on translation table walk, 1st level"
		if ($f == 0x1c);

	# b11110
	return "Synchronous parity error on translation table walk, 2nd level"
		if ($f == 0x1e);

	# b00101
	return "Translation fault, 1st level"
		if ($f == 0x05);

	# b00111
	return "Translation fault, 2nd level"
		if ($f == 0x07);

	# b00011
	return "Access flag fault, 1st level"
		if ($f == 0x03);

	# b00110
	return "Access flag fault, 2nd level"
		if ($f == 0x06);

	# b01001
	return "Domain fault, 1st level"
		if ($f == 0x09);

	# b01011
	return " Domain fault, 2nd level"
		if ($f == 0x0b);

	# b01101
	return " Permission fault, 1st level"
		if ($f == 0x0d);

	# b01111
	return "Permission fault, 2nd level"
		if ($f == 0x0f);

	# b00010
	return "Debug event"
		if ($f == 0x02);

	# b01000
	return "Synchronous external abort, non-translation"
		if ($f == 0x08);

	# b11001
	return "Synchronous parity error on memory access"
		if ($f == 0x19);

	# b10110
	return "Asynchronous external abort"
		if ($f == 0x16);

	# b11000
	return "Asynchronous parity error on memory access"
		if ($f == 0x18);

	return "?????"
}


#  - - - - - - - - - - - - - - - - - - - - - - - - - -

sub decode_syndrome($)
{
	my $s = shift;
	my $pad  = "\t*";
	my $notepad  = "\t\t#";

	sep();

	printf("DFSR:\t0x%08x\n", $s);

	print "$pad Abort caused by a cache maintenance operation\n"
		if ($s & (0x01 << 13));

	print "$pad External abort marked as ";
	if ($s & (0x01 << 12)) {
		print "SLVERR\n";
	} else {
		print "DECERR (AXI Decode)\n";
	}

	print "$pad Abort caused by a ";
	if ($s & (0x01 << 11)) {
		print "WRITE access\n";
	} else {
		print "READ access\n";
	}
	print "$notepad NOTE: Abort will be set (WRITE) for faults on" .
		" CP15 cache\n";
	print "$notepad maintenance ops, including VA to PA translation" .
		" ops.\n";

	# [10]	FS[4]
	my $fault_status4 = ($s >> 10) & 0x1;

	# [9] RAZ

	# [8] Reserved UNK/SBZP

	# [7:4]
	my $domain = ($s & 0xF0) >> 4;
	printf("%s Domain: D%u\n", $pad, $domain);
	print "$notepad NOTE: ARMv7 deprecates any use of the domain field" .
		" in the DFSR.\n";
	print "$notepad For a Permission fault that generates a Data Abort" .
		" exception,\n";
	print "$notepad this field is unknown.\n";
	print "\n";

	# [10] | [3:0]
	my $fault_status =  ($s & 0x0F) | ($fault_status4 << 4);

	printf("%s Fault reason: %s. Code = b%05b.\n",
		$pad, fault2string($fault_status), $fault_status);
	sep();
}


# -----------------------------------------------------

sub init_src_dumper()
{
	my $f = $FAMILY;

	my $section = "ssbl";
	my $unasmfile = ".$section.asm";
	my $tchain_path = "";

	$f = "objs/" . $f . "/$section.o";

	if ( -e "toolpath" ) {
		print "# found toolpath file\n";
		my $t = do_shell("grep 'PATH:=' toolpath");
		$t =~ s/:\$\(PATH\)//;
		$t =~ s/PATH:=//;
		$t =~ s/\n//;
		$tchain_path = $t . "/";
	}

	if ( ! -e  "$f" ) {
		print "# $f not found\n";
		$super_source_dump_mode = 0;
		return;
	}

	my $cmd = $tchain_path . "arm-linux-objdump -Sd " .
		$f . " > " . $unasmfile;

	print "# Executing: $cmd\n";
	do_shell("$cmd");
	print "# Exec done\n";

	@unasm_text = split /\n/, read_textfile($unasmfile);
#	unlink $unasmfile;

	print "# Disassembly read in.\n";
}


sub dump_src_range($$$)
{
	my ($at, $msg, $delta) = @_;

	my $f = $at - $delta;
	my $t = $at + $delta;

	$f = 0
		if ($f < 0);
	$t = 0
		if ($t < 0);

	print "# $msg, line $at ($f to $t printed.)\n";
	print "#\n";

	for (my $i = $f; $t > $i ; $i++) {
		if ($i == $at) {
			printf("---->\t%s\n", $unasm_text[$i]);
		} else {
			printf("%05d\t%s\n", $i, $unasm_text[$i]);
		}
	}
}



sub __src_dumper($$$)
{
	my ($x, $msg, $delta) = @_;

	for (my $i = 0; $i < (@unasm_text); $i++) {
		if ($unasm_text[$i] =~ m/$x\:/) {
			dump_src_range($i, $msg, $delta);
			sep();
			return 1;
		}
	}
	return 0;
}

sub src_dumper($$)
{
	my ($addr, $msg) = @_;
	my $c = 1;

	return
		if ($super_source_dump_mode != 1);

	my $x = sprintf("%x", $addr);
	my $rc = __src_dumper($x, $msg, 10);
	return
		if ($rc == 1);

	print "# Exact address $x for $msg not found, ";

	if (0 == is_any_bolt_instruction_address_sane($addr)) {
		print "address $x is outside of FSBL & SSBL area.\n";
		return;
	}
	print "\n";

	# Try stepping back a bit
	while($c <= 16) {
		my $a = $addr - $c;
		$x = sprintf("%x", $a);
		$rc =__src_dumper($x, $msg, 10);
		if ($rc == 1) {
			print "# Nearest address for $msg is back $c to $x\n";
			sep();
			return;
		}
		$c = $c + 1;
	}
	print "# No addresses found for $msg\n";
	sep();
}


# -----------------------------------------------------

# Swiped from BOLT/scripts/BcmUtils.pm
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
	my $Debug = 0;

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
				$val = hex($val)
					if ($val =~ /^0x[0-9A-F]{1,8}$/i);
				$rh->{$key} = $val;
				push @{$ra}, [$key, $val];
			}
		}
		close($fh);
	}
	return ($rh, $ra);
}

# Swiped from BOLT/scripts/config.pl
sub get_bchp_info()
{
	my $family = $FAMILY;

	return $bchp_info{$family}
		if $bchp_info{$family};

	my $rdb_dir = "include/" . $family;
	return undef
		if (! -d $rdb_dir);

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
		'bchp_ddr??_phy_common_regs_?.h',
		'bchp_ebi.h',
		'bchp_shimphy_addr_cntl_?.h',
		'bchp_nand.h',
		'bchp_phy_control_regs_?.h',
		'bchp_sata_top_ctrl.h',
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
		'bchp_upg_main_irq.h',
		'bchp_upg_main_aon_irq.h',
		'bchp_upg_bsc_irq.h',
		'bchp_upg_bsc_aon_irq.h',
		'bchp_upg_spi_aon_irq.h',
		'bchp_mac.h',
		'bchp_gio.h',
		'bchp_gio_aon.h',
		'bchp_timer.h',
		'bchp_webhif_timer.h',
		'bchp_watchdog.h',
		);

	@incs = map { "$rdb_dir/$_" } @incs;

	my ($rh_defines, $ra_defines)
		= grok_defines_from_c_incl_files( @incs );

	my %h = (
		phys_offset => $rh_defines->{BCHP_PHYSICAL_OFFSET},
		rh_defines => $rh_defines,
		ra_defines => $ra_defines,
	);

	$bchp_info{$family} = \%h;

	return \%h;
}

sub init_bchp_info()
{
	my $rh = get_bchp_info();

	return
		if (! keys %bchp_info);

	$rdb_start = $rh->{phys_offset};
	my $bchp_defines = $rh->{rh_defines};
	$rdb_end = $rdb_start + $bchp_defines->{BCHP_REGISTER_END};
}

sub is_integer
{
	defined $_[0] && $_[0] =~ /^[+-]?\d+$/;
}

sub find_rdb_reg_by_addr($)
{
	my $addr = shift;
	my $rh = get_bchp_info();
	my $offs = $addr - $rh->{phys_offset};
	my $s = sprintf("%x", $offs);
	my $ra = $rh->{ra_defines};

	foreach my $x (@$ra) {
		my $key = $x->[0];
		my $val = $x->[1];

		if (is_integer($val)) {
			return $key
				if ($val == $offs);
		}
	}

	return undef;
}


# -----------------------------------------------------
# The main code
# -----------------------------------------------------


# -----------------------------------------------------
# Getops & check them

getopts("hf:g:kd", \%cmdflags)
	or usage_exit(-1);

usage_exit(0)
	if (defined $cmdflags{h});

$fname = $cmdflags{f}
	if (defined $cmdflags{f});

$gendir = $cmdflags{g}
	if (defined $cmdflags{g});

$keep_going = 1
	if (defined $cmdflags{k});

$super_source_dump_mode = 1
	if (defined $cmdflags{d} && defined $cmdflags{g});


# -----------------------------------------------------
# Setup

if (defined $gendir) {
	$FAMILY = $gendir;

	# get $FAMILY from gen
	$FAMILY =~ s/gen//;
	$FAMILY =~ s/\///;

	get_extra_defines();
	get_config_defines();
	init_find_stacktop();

	init_src_dumper()
		if ($super_source_dump_mode == 1);

	init_bchp_info();
}


# -----------------------------------------------------
# Slurp file, or stdin.
my @lines;

if (defined $fname) {
	open($fh, '<', $fname)
		or failed("could not open file '$fname'");
	@lines = <$fh>;
	close($fh);
} else {
	@lines = <STDIN>;
}


my $found_exception;
my $found_sysregs;
my $reg;
my @armreg;

my $cval;
my $ival;
my $dval;
my $fval;

sub reset_found() {
	$found_exception = 0;
	$found_sysregs = 0;
	$reg = 0;
	@armreg = ();

	$cval = -1;
	$ival = -1;
	$dval = -1;
	$fval = -1;
}


# -----------------------------------------------------
# main loop

reset_found();

for (my $i = 0; $i < (@lines); $i++) {

	$lines[$i] = strim($lines[$i]);

	next
		if (length $lines[$i] < 4);

	# no switch() for earlier Perl compat, in case you were wondering.
	if ($lines[$i] =~ m/CPU exception/) {
		print "\n";
		printf("# Found exception (line %u)\n", $i + 1);
		print "# ===============\n";
		print "# " . $lines[$i] . "\n";
		reset_found();
		$found_exception = 1;
		next;
	}

	if ($reg < 16) {
		if ($lines[$i] =~ m/^r$reg/) {
			my $regval = regtrim($lines[$i]);
			printf("R%s\t0x%08x", $reg, $regval);

			$armreg[scalar $reg] = $regval;

			report_is_ssbl_instruction_address_sane($regval)
				if ($reg ~~ [14, 15]);

			is_stack_address_sane($regval)
				if ($reg == 13);

			print "\n";
			$reg++;
			print "# All general purpose registers found" .
					" and in order\n"
				if ($reg == 16);
			next;
		}
	}

	failed(" !!! General purpose registers either NOT found" .
			" or bad ordering (count = $reg)\n")
		if ($reg != 16);

	next
		if ($found_exception != 1);

	if ($lines[$i] =~ m/cpsr/) {
		$cval = regtrim($lines[$i]);
		sep();
		decode_cpsr($cval);
		$found_sysregs++;
		next;
	}

	if ($lines[$i] =~ m/dfar/) {
		$dval = regtrim($lines[$i]);
		printf("DFAR:\t0x%08x\n", $dval);
		decode_data_fault_address($dval);
		sep();
		$found_sysregs++;
		next;
	}

	if ($lines[$i] =~ m/dfsr/) {
		$fval = regtrim($lines[$i]);
		decode_syndrome($fval);
		$found_sysregs++;
		next;
	}

	if ($lines[$i] =~ m/ifar/) {
		$ival = regtrim($lines[$i]);
		printf("IFAR:\t0x%08x\n", $ival);
		if ($ival != 0) {

			print("\t - Possibe non-NULL bad address!")
				if (! defined $gendir);

			report_is_ssbl_instruction_address_sane($ival);
			printf("\n");
			# Try to find out where it is anyway
			decode_data_fault_address($ival);

		} else {
			#TBD: Use exception type & fault status
			# to be smarter about this.
			print("\t - Not an instruction fault," .
				" or PC was NULL.\n")
		}

		sep();
		$found_sysregs++;

		failed(" !!! Not all system registers found " .
				"(count = $found_sysregs)\n")
			if ($found_sysregs != 4);

		src_dumper($ival, "IFAR")
			if (($ival != -1) && ($ival != 0));

		# might as well
		src_dumper($dval, "DFAR")
			if (($dval != -1) && ($dval >= 4096));

		src_dumper($armreg[14], "LR (R14)");
		src_dumper($armreg[15], "PC (R15)");

		# ready for next log, if multiple
		reset_found();

		next;
	}

}

# We are done
