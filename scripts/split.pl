#!/usr/bin/perl
# ***************************************************************************
# *     Copyright (c) 2012-2013, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************
use strict;
use warnings;
use File::Basename;
my $P = basename $0;

die "\nUSAGE:  $P offset length infile outfile\n\n" .
	"\t:Note: offset and length must be in hex. " . 
	"If length is zero then outfile is from offset to EOF\n\n"
	if (!$ARGV[0] || !$ARGV[1] || $ARGV[0] !~ /^0x/i || $ARGV[1] !~ /^0x/i);
die "$P: Error: infile '$ARGV[2]' does not exist.\n"
	if (! -f $ARGV[2]);
die "$P: Error: outfile not specified.\n"
	if (!defined $ARGV[3] || !length $ARGV[3]);

my $offset = hex($ARGV[0]);
my $length = hex($ARGV[1]);
my $infile = $ARGV[2];
my $outfile = $ARGV[3];

open my $fin, "<", $infile or die "$P: Error opening $infile";
open my $fout, ">", $outfile or die "$P: Error opening $outfile";
binmode $fin;
binmode $fout;

undef $/;
my $x = <$fin>;
if  ($length == 0) {
	print $fout substr($x, $offset);
} else {
	print $fout substr($x, $offset, $length);
}

exit(0);
