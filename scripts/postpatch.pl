#!/usr/bin/perl -w
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

use strict;
use warnings FATAL=>q(all);
use File::Basename;
use Getopt::Std;
use Data::Dumper;
use Digest::MD5 qw(md5_hex);
my $P = basename $0;

# Note: had fun trying exists & defined for this.
use vars qw(@configs);
my $hascfg = scalar @configs;


# args ---------------------------------------------------------------------

my $arg_zeusver = undef;
my $arg_bbl = undef;
my $arg_bfw = undef;
my $arg_pfx = undef;
my $arg_family = undef;
my $arg_ver = undef;
my $arg_debug = 0;
my $arg_bbl_ver = 1;
my $arg_symlinks = 0;
my $arg_keep = 0;
my $arg_odir = undef;
my $arg_tool = undef;


# helpers ------------------------------------------------------------------

sub usage_exit()
{
	print "usage: $P -z <zeus type> -p <list names> ";
	print "-l <list BBL versions> -l <list BFW versions> ";
	print "-f <FAMILY> -v <VERSION> ";
	print "-d (debug) -o (omit BBL version) ";
	print "-k (keep original, as 'last.bin')\n";
	print "-D <build object base dir>\n";
	print "-T <patching tool name and path>\n";
	print " Note: <list N...> is a comma separated list with no spaces, ";
	print "-d, -k -o and -D are optional\n";
	exit(-1);
}

sub failed($)
{
	my $x = shift @_;
	print " *** fail: " . $x . "\n";
	usage_exit();
}

sub do_shell($)
{
	my $cmd = shift;

	print "\n    [$cmd]\n"
		if ($arg_debug);
	my @a = `$cmd`;
	print "\n" . Dumper @a
		if ($arg_debug);

	die "$P: FAIL: non-zero exit code.\n"
		if (($? >> 8) & 0xff);

	return wantarray ? @a : join("", @a);
}

# getopts ------------------------------------------------------------------

my %cmdflags=();
getopts("sz:p:l:w:f:v:dokD:T:", \%cmdflags) or usage_exit();

$arg_zeusver = $cmdflags{z} if (defined $cmdflags{z});
$arg_pfx    = $cmdflags{p} if (defined $cmdflags{p});
$arg_bbl    = $cmdflags{l} if (defined $cmdflags{l});
$arg_bfw    = $cmdflags{w} if (defined $cmdflags{w});
$arg_family = $cmdflags{f} if (defined $cmdflags{f});
$arg_ver    = $cmdflags{v} if (defined $cmdflags{v});
$arg_debug  = $cmdflags{d} if (defined $cmdflags{d});
$arg_odir  = $cmdflags{D} if (defined $cmdflags{D});
$arg_tool  = $cmdflags{T} if (defined $cmdflags{T});

$arg_bbl_ver = 0 if (defined $cmdflags{o});
$arg_symlinks = 1 if (defined $cmdflags{s});
$arg_keep = 1 if (defined $cmdflags{k});


# cmdflags checks ----------------------------------------------------------

failed("-z missing param value: 0 (zeus) 1 (zeus 4.2) 2 (zeus 4.1)")
	if (!defined $arg_zeusver);
failed("-l missing param list for BBLs")
	if (!defined $arg_bbl);
failed("-w missing param list for BFWs")
	if (!defined $arg_bfw);
failed("-p  missing param list for image identifiers")
	if (!defined $arg_pfx);
failed("-f missing chip family")
	if (!defined $arg_family);
failed("-v missing bolt version")
	if (!defined $arg_ver);

failed("-T missing patching tool")
	if (!defined $arg_tool);

$arg_odir = "objs/$arg_family"
	if (!defined $arg_odir);

my $bolt = "$arg_odir/bolt.bin";

# keep the file name format the same if we have no BBL or BFW.
my $no_sec_bolt = "bolt-$arg_ver-$arg_family-xx-";

$no_sec_bolt .= "bbl-x.x.x-"
	if ($arg_bbl_ver);

$no_sec_bolt .= "bfw-x.x.x.bin";


failed( $bolt . " does not exist")
	if ( ! -f $bolt);

# get values ----------------------------------------------------------

my @list_bbls = split(',', $arg_bbl);
my @list_bfws = split(',', $arg_bfw);
my @list_pfxs = split(',', $arg_pfx);

# support ------------------------------------------------------------------

sub do_symlink($$)
{
	return if (!$arg_symlinks);
	my ($file, $linky) = @_;

	unlink($linky);
	print "  SYMLINK $file -> $linky\n";
	symlink($file, $linky);

}

sub md5file($)
{
	my($fin) = @_;
	open my $fh, "<", $fin or failed("Can't open $fin");
	my @array = <$fh>;
	close $fh;
	my $digest = md5_hex(@array);
	return $digest;
}

sub fappend($$)
{
	my($fap, $text) = @_;
	open my $fh, ">>", $fap or failed("Can't append $fap");
	print $fh $text;
	close $fh;
}

# doit ---------------------------------------------------------------------

my $patched_bolts = 0;

for (my $i = 0; $i < @list_pfxs; $i++) {

	my $r;
	my $meta = "BOLT-META:";
	my $file_bbl = "security/" . $arg_family . "/bbl-" . $list_bbls[$i] . ".bin";
	my $file_bfw = "security/" . $arg_family . "/bfw-" . $list_bfws[$i] . ".bin";

	# do as a matched/compatible set
	if ( (! -f $file_bbl) || (! -f $file_bfw)) {
		print "skipping as one or more files $file_bbl $file_bfw don't exist\n"
			if ($arg_debug);
		next;
	}

	my $bname = "bolt-" . $arg_ver . "-" . $arg_family;
	$bname .=     "-" . $list_pfxs[$i];

	$bname .= "-bbl-" . $list_bbls[$i]
		if ($arg_bbl_ver);

	$bname .= "-bfw-" . $list_bfws[$i] . ".bin";

	print "  PATCH   $bname...";

	my $cmd = "$arg_tool -z $arg_zeusver -i $bolt -o $arg_odir/$bname";
	my $sub_bbl = "-t bbl -p $file_bbl -i $bolt";
	my $sub_bfw = "-t bfw -p $file_bfw -i $arg_odir/$bname";

	$r = do_shell("$cmd $sub_bbl");
	print "\n$r"
		if ($arg_debug);

	$r = do_shell("$cmd $sub_bfw");
	print "\n$r"
		if ($arg_debug);

	if ($arg_keep == 1) {
		$r = do_shell("cp $arg_odir/$bname $arg_odir/last.bin");
		print "\n$r"
			if ($arg_debug);
	}

	#now build the metadata
	$meta .= "name=" . $bname . ",";
	$meta .= "family=" . $arg_family . ",";
	$meta .= "version=" .$arg_ver . ",";
	$meta .= "md5sum=" . md5file("$arg_odir/$bname") .",";

	#pad string to a uniform size
	$meta = sprintf("%-512s", $meta);

	fappend("$arg_odir/$bname", $meta);
	print " OK\n";

	# NB: We may have to append "-bfw-$list_bbls[$i]" later if
	# we get multiple bfw versions per bbl.
	do_symlink($bname, "$arg_odir/bolt-$list_pfxs[$i].bin");

	$patched_bolts++;
}

if ($patched_bolts) {
	#  If we have at least one BBL & BFW set then 
	# security is setup and we don't want this to
	# be flashed as it WILL FAIL TO BOOT on verifed
	# general systems that are distributed
	# internally, and to customers.
	do_shell("rm $bolt")
		if (!$arg_debug);
} else {
	do_shell("mv $bolt $arg_odir/$no_sec_bolt");
	do_symlink($no_sec_bolt, "$arg_odir/bolt-xx.bin");
	print "  PATCH   *unpatched*\n";
}

exit(0);

