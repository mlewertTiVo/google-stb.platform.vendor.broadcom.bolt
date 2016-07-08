#!/usr/bin/perl -w
# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

# Example:
# ./scripts/bpatch.pl -a 0x000096F0 -p security/7445d0/bbl.bin \
#	-i objs/7445d0/bolt.bin -o objs/7445d0/bolt-new.bin
#
# ./gen/scripts/patcher.pl -z zeus42 -t bbl -p security/7445d0/bbl.bin \
#	-i objs/7445d0/bolt.bin -o objs/7445d0/bolt-new.bin

use strict;
use warnings FATAL=>q(all);
use File::Basename;
use Getopt::Std;
my $P = basename $0;

# Note: had fun trying exists & defined for this.
use vars qw(@configs);
my $hascfg = scalar @configs;


sub usage()
{
	print " help: $P -h for this help, or -c for current configuration\n";
	print " info: patch (overlay) one binary file into another at a specfic offset/address\n";
	print " usage: $P -a hexoffset -p patchfile -i infile -o outfile";
	if ($hascfg) {
		print " -z zeus (version) -t type (bbl, bfw etc.)\n";
		print " Note that -z and -t flags MUST be used together,\n";
		print " and the -a flag can then be omitted as it will\n";
		print " be overwritten by the combination of -z and -t flags.";
	}
	print "\n";
}

sub usage_exit($) 
{
	my $x = shift @_;
	usage();
	exit $x;
}

sub dumpconfig_exit()
{
	if ($hascfg) {
		print "configuration flags -t(ype) for -z(eus):\n";
		foreach my $c (@configs) {
			print "@$c[1] for @$c[0]\n";
		}
	} else {
		print "no configuration info available.\n";
	}
	exit 0;
}

sub failed($)
{
	my $x = shift @_;
	print " *** fail: " . $x . "\n";
	usage_exit(-1);
}

sub get_hexoffset($$)
{
	my ($z, $t) = @_;

	foreach my $c (@configs) {
		return @$c[2] 
			if ((@$c[0] eq $z)&&(@$c[1] eq $t)) 
	}
	return undef;
}


# args ---------------------------------------------------------------------

my $hexoffset = undef;
my $patchfile = undef;
my $infile = undef;
my $outfile = undef;
my $zeus = undef;
my $type = undef;


# getopts ------------------------------------------------------------------

my %cmdflags=();
getopts("ha:p:i:o:z:t:c", \%cmdflags) or usage_exit(-1);

usage_exit(0) if (defined $cmdflags{h});
dumpconfig_exit() if (defined $cmdflags{c});


$patchfile = $cmdflags{p} if (defined $cmdflags{p});
$infile = $cmdflags{i} if (defined $cmdflags{i});
$outfile = $cmdflags{o} if (defined $cmdflags{o});

$hexoffset = scalar hex $cmdflags{a} if ((defined $cmdflags{a}) &&
				!($cmdflags{a} !~ /^0x/i));

$zeus = $cmdflags{z} if (defined $cmdflags{z});
$type = $cmdflags{t} if (defined $cmdflags{t});

# cmdflags checks ----------------------------------------------------------

failed("-z -t unsupported flags, use patcher.pl instead.")
	if (!$hascfg && (exists $cmdflags{z} || exists $cmdflags{t}));

$hexoffset = get_hexoffset($zeus, $type)
	if ($hascfg && defined $zeus && defined $type);

failed("-a Address offset missing or not a hex number")
	if (!defined $hexoffset);

failed("-p (patch file) param missing or file does not exist")
	if (!defined $patchfile || !length $patchfile || ! -f $patchfile);

failed("-i (input file) param missing or file does not exist")
	if (!defined $infile || !length $infile || ! -f $infile);

failed("-o (output file) param missing")
	if (!defined $outfile);


# doit ---------------------------------------------------------------------

open my $fpat, "<", $patchfile or failed("opening $patchfile");
open my $fin, "<", $infile or failed("opening $infile");

binmode $fpat;
binmode $fin;

undef $/;
my $pat = <$fpat>;
close $fpat;
failed("'$patchfile' is an empty file")
		if ( length($pat) <= 0);
undef $/;
my $in = <$fin>;
close $fin;
failed("'$infile' is an empty file")
		if (length($in) <= 0);

my $top_addr = $hexoffset + length($pat);

if ($top_addr > length($in)) {
	printf(" ==> lengthen image from 0x%x to 0x%x\n",
		length($in),
		$top_addr);

	$in .= " " x ($top_addr - length($in));
}

printf(" ==> patching %s (0x%x) into %s (0x%x) at offset 0x%x\n",
	$patchfile, length($pat), $infile, length($in) , $hexoffset);

substr($in, $hexoffset, length($pat)) = $pat;

open my $fout, ">", $outfile or failed("open $outfile for write");
binmode $fout;
print $fout $in;
close $fout;

exit 0;

