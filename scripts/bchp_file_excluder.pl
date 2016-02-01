#!/usr/bin/perl -w
###########################################################################
#     Copyright (c) 2013, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
###########################################################################
use strict;
use warnings;
use File::Spec;
use File::Basename;
use Data::Dumper;

# This script lists the bchp_xxx.h files that we could remove from the repo.
# It's analysis is conservative rather than precise -- it works by finding
# all of the "#include bchp_xxx.h" statements and correlating them to our
# current list of bchp_xxx.h files.

my $P = basename $0;
my ($f,$dh,@a,%h,@to_delete);

my $scripts_dir = dirname(File::Spec->rel2abs(__FILE__));
chdir("$scripts_dir/..") or die "$P: cannot find Bolt root dir.\n";

# Get list of all source files.
opendir($dh, ".") or die;
@a = grep { !/^(objs|dtc|\.+)$/ } readdir($dh);
closedir($dh);
my $cmd = "find " . join(' ',@a) . " -name '*.[chshS]' -type f";
@a = map { chomp; $_ } `$cmd`;

# %h will soon contain the basenames of all the bchp_files we want.
foreach my $f (@a) {
	next if (basename($f) =~ /^bchp_.*\.h$/);
	open(my $fh, $f) or next;
	while (<$fh>) {
		$h{$1} = 1 if (/^\s*\#\s*include\s+["<](bchp_.*\.h)[>"]/);
	}
	close($fh);
}

# Get the list of bchp_files we have.
$cmd = "find include -name 'bchp_*.h'";
@a = map { chomp; $_ } `$cmd`;


foreach $f (@a) {
	my $bn = basename $f;
	push @to_delete, $f if (!$h{$bn});
}

print "\n";
printf "$P: Total include files: %d.\n", scalar(@a);
printf "$P: Total files that could be deleted: %d.\n", scalar(@to_delete);
print "\n\n";
print map { "\trm $_\n" } @to_delete;
