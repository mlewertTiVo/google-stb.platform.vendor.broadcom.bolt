#!/usr/bin/perl -w
##############################################################################
# Copyright (c) 2013 Broadcom Corporation
# All Rights Reserved
# Confidential Property of Broadcom Corporation
# 
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# 
##############################################################################
use strict;
use warnings;
use File::Basename;
use File::Spec;
use Data::Dumper;
use Getopt::Long;

my $P = basename $0;

sub usage($)
{
	print "USAGE:\n  $P [-j<int>] [-d]\n\n";
	print "SYNOPSIS:\n  Invoke 'make' to make Bolt for all supported chips.\n\n";
	print "OPTIONS:\n";
	print "  -j<int> .... pass -j<int> to make.\n";
	print "  -d ......... dry run.\n";
	exit shift;
}

Getopt::Long::Configure(qw/no_ignore_case bundling/);

my $dry_run;
my $jobs = "";
my $help;

GetOptions(
	'd|dry'		=> \$dry_run,
	'h|help'	=> \$help,
	'j|jobs=i'	=> \$jobs
	);

usage(0) if ($help);

$jobs = "-j$jobs" if $jobs;

# chdir to root of Bolt source tree.
my $d_root = dirname( File::Spec->rel2abs(__FILE__))  . "/..";
chdir $d_root or die "$P: Error: could not chdir to '$d_root'.\n";

system("make toolfind");

my @a;
foreach (glob('config/family-*.cfg')) {
	s/^config\/family-//;
	s/\.cfg$//;
	next if !/^\d+[a-g]\d$/i;
	push @a, $_;
}

my %builds = ();
foreach my $chip (@a) {
	my $cmd = "make $chip $jobs";
	print "[$P: spawning '$cmd']\n";
	next if $dry_run;
	FORK: {
		if (my $pid = fork) {
			$builds{$chip}{'pid'} = $pid;
		} elsif (defined($pid)) {
			my $ecode = system("$cmd") >> 8;
			if ($ecode) {
				exit($ecode);
			} else {
				exit;
			}
		} elsif ($! == 'EAGAIN') {
			sleep 1;
			redo FORK;
		} else {
			die "Can't fork: $!\n";
		}
	}
}

my $errs = '';
foreach my $chip (@a) {
	waitpid($builds{$chip}{'pid'}, 0);
	my $ecode = $?;
	if ($ecode) {
		$errs .= "[$P: $chip build failed with exit code $ecode]\n";
	}
}

if ($errs) {
	print "$errs";
	exit(1);
} else {
	print "[$P: All builds successful!]\n"
		if (!$dry_run);
}
exit(0);

