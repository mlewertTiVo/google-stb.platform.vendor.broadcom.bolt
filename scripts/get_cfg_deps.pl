#!/usr/bin/perl -w
# ***************************************************************************
# *     Copyright (c) 2013, Broadcom Corporation
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

my %deps;

#
# Scans a config file and follows its "include" statements and note its
# "rts_config" statements.  Recursively collects the set of all dependent 
# files.
#

sub scan_file($)
{
	my ($f) = @_;
	my $fh;
	die "$P: file '$f' does not exist.\n"
		if (! -f $f);
	open($fh, '<', $f)
		or die "$P: could not open file '$f'.\n";
	my @a;
	while (<$fh>) {
		if (/^\s*include\s+(\S+)/) {
			if (! $deps{$1}) {
				$deps{$1} = 1;
				push @a, $1;
			}
		} elsif (/^\s*rtsconfig\s+\-id\s+\w\s+\-file\s+(\S+)/) {
			my @a = split /\s+/, $_;
			$deps{$a[5]} = 1;
		} elsif (/^\s*dts (\S+)/) {
			$deps{$1} = 1;
		}
	}
	close($fh);
	map { scan_file($_) } @a;
}

sub main()
{
	die "\nUSAGE: $P a.cfg b.cfg ...\n\n"
		if (grep /^--?h/i, @ARGV);

	while (@ARGV) {
		my $f = shift @ARGV;
		scan_file($f);
		$deps{$f} = 1;
	}
	
	print join(' ', keys %deps);
	return 0;
}

exit main();
