#!/usr/bin/perl -w
# ###########################################################################
# Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ###########################################################################

use POSIX;
use strict;
use warnings;
use Getopt::Long;

my $P = $0;
$P =~ s@.*/@@g;

my $ret = 0;
my $fields = "ULIMIT|BUSNUM|LLIMIT|UBUSCDBIT";
my $incdir = "include";
my $file;
my $opt_incdir;
my $opt_file;
my $opt_help = 0;

sub help($)
{
	my ($exitcode) = @_;

	print << "EOM";
Usage: $P [OPTIONS] [CHIP...]
Options:
    -d header-dir
    -f header-file
    -h
Examples:
    $P
    $P 73471a0
    $P 7250b0 7268b0 7271b0
    $P -d your-bolt-dir/include 7278b0
    $P -d your-bolt-dir/include 7439b0 7445e0
    $P -f somewhere/bchp_hif_cpubiuctrl.h
EOM

	exit($exitcode);
}

sub extract_ranges($$)
{
	my ($family, $fname) = @_;
	my %ranges;

	if (! defined($fname) or ! -r $fname) {
		# include/$family/bchp_hif_cpubiuctrl.h
		$fname = "$incdir/$family/bchp_hif_cpubiuctrl.h";
	}
	open(my $fh, "<", $fname) or die "cannot open file '$fname'";

	while (my $line = <$fh>) {
		# BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_[UL]LIMT[0-9]+_*_DEFAULT
		my $base = "BCHP_HIF_CPUBIUCTRL_CPU_BUS_RANGE_";
		if ($line =~ m/$base[UL]LIMT([0-9]{1,2})_($fields)_DEFAULT/) {
			# take value from "#define BCHP_*_DEFAULT value"
			my @chunks = split ' ', $line;
			$ranges{$1}{$2} = hex($chunks[2]);
		}
	}

	return %ranges;
}

sub print_ranges(\%)
{
	my (%ranges) = %{$_[0]};

	foreach my $set (sort { $a <=> $b } keys %ranges) {
		next if ! defined($ranges{$set}{BUSNUM});
		next if ! defined($ranges{$set}{LLIMIT});
		next if ! defined($ranges{$set}{ULIMIT});

		# { BUS, L_reg, U_reg } /* index [L..U] */
		printf("{ %d, 0x%07X, 0x%07X } /* %2d [0x%010X..0x%010X] */\n",
			$ranges{$set}{BUSNUM},
			$ranges{$set}{LLIMIT},
			$ranges{$set}{ULIMIT},
			$set,
			$ranges{$set}{LLIMIT} * 0x1000,
			$ranges{$set}{ULIMIT} * 0x1000 + 0xFFF);
	}
}

# main
# $P [{[-f bchp_hif_cpubiuctrl.h] | [[-d your-include-dir] [CHIP]...]}]
Getopt::Long::config("no_auto_abbrev", "no_ignore_case");
GetOptions(
	"d=s" => \$opt_incdir,
	"f=s" => \$opt_file,
	"h|?" => \$opt_help) or help(1);
help(0) if ($opt_help);

if (defined($opt_incdir) and defined($opt_file)) {
	print "Options -d and -f cannot co-exist\n";
	help(1);
}

if (defined($opt_file) and defined($ARGV[0])) {
	print "WARNING: ignoring @ARGV\n";
}

$incdir = $opt_incdir if defined($opt_incdir);
$file = $opt_file if defined($opt_file);

if (defined($file)) {
	my %ranges = extract_ranges("", $file);
	print_ranges(%ranges);
	exit $ret
}

if (defined($ARGV[0])) {
	# run through the specified chip family (or families)
	# include/$ARGV[0]/bchp_hif_cpubiuctrl.h
	while (defined($ARGV[0])) {
		my $family = lc $ARGV[0];
		my %ranges = extract_ranges($family, undef);

		print "$family:\n";
		print_ranges(%ranges);
		shift @ARGV;
	}
	exit $ret;
}

# run through all chip families available under include/
opendir(my $dh, $incdir) or die "cannot read '$incdir' directory";
for my $family (sort readdir $dh) {
	if ($family =~ m/^[0-9]+[a-z]0$/ and -d "$incdir/$family") {
		print "$family:\n";
		my %ranges = extract_ranges($family, undef);
		print_ranges(%ranges);
	}
}
closedir $dh;

exit $ret;
