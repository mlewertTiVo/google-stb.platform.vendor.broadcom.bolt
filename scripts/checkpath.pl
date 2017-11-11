#!/usr/bin/perl -w
use strict;
use warnings;
use File::Find;

# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

use Getopt::Long;

select(STDERR);
$| = 1;
select(STDOUT); # default
$| = 1;

# Print /path/to/toolchain:$PATH if a suitable toolchain can be found:
# bin/checkpath.pl -a
#
# Print errors/warnings if a suitable toolchain is not currently in $PATH:
# bin/checkpath.pl
#

my $makeheader = "
# #########################################################################
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# #########################################################################
#
# AUTOMATICALLY GENERATED - Feel free to edit PATH.
\n";

my $PATH_PREFIX = "stbgcc-";
my $TOOLNAME = "arm-linux-";
my $CC = "gcc";
my $ALT_TOOL_PATH = '';

my @searchpath = (
"/opt/toolchains", 
"/project_it/stbtc",
"/projects/stbtc",
"/projects/stbopt_p/toolchains_303",
);

my @othertools = ();
my $annoytime = 5;
my $add = 0;
my $path = $ENV{'PATH'};
my $gcc_version = "";
my $toolchain = '';


sub find_othertools {
	if ((-f $_)&&($_ =~/$TOOLNAME/)) {
		push(@othertools, $File::Find::name); 
	}
}

sub write_toolpath {
	my ($p,$cross) = @_;
	open(F, ">toolpath") or die "can't open toolpath file";
	print F $makeheader;
	print F "PATH:=" . $p . ":\$(PATH)\n";
	print F "CROSS:=" . $cross . "\n"
        if (defined $cross);
	close F;
	print "Created 'toolpath' file. (Re-)commence your build.\n";
}


# begin -----------------------------------

GetOptions('cross=s' => \$TOOLNAME, 'a' => \$add,
            'tool:s' => \$ALT_TOOL_PATH, 'cc:s' => \$CC);

# shortcut to just do as the developer wants
# and put in the toolchain as-is.
if (length($ALT_TOOL_PATH) and ($add == 1)) {
    write_toolpath($ALT_TOOL_PATH, $TOOLNAME);
    exit 0;
}

$TOOLNAME .= $CC;

open(F, "<config/toolchain") or die "can't open config/toolchain";
$toolchain = <F>;
close(F);
$toolchain =~ s/[\r\n]//g;

foreach my $x (split(/:/, $path)) {
	if(-e "$x/$TOOLNAME") {
		print "$x/$TOOLNAME is in your path.\n";
		if($x =~ m/$toolchain/) {
			# matches the recommended toolchain
			if($add) {
				print "\nYour toolchain is good\n";
				# print " PATH: $path\n\n";
			}
			exit 0;
		}
		if($add == 0) {
			if(($x !~ m/crosstools_hf-/) &&
			   ($x !~ m/$PATH_PREFIX/)) {
					# user has renamed the toolchain - assume
					# they know what they are doing
					print "\nPossible toolchain rename or hackery, presume its good.\n";
					# print " PATH: $path\n\n";
				   exit 0;
			}
			
			my $cmd = "$x/$TOOLNAME --version | grep " . $PATH_PREFIX . "[0-9].[0-9]";
			my $ver = `$cmd`;
			$ver =~s/[\(\)]//g;
			$ver =~s/[\t]/ /g;

			foreach my $v (split(/ /, $ver)) {
				if($v =~ m/$toolchain/) {
					print "\nUsing toolchain: $v, in path $x\n";
					exit 0;
				}
				if($v =~ m/$PATH_PREFIX/) {
					$gcc_version = $v;
				}
			}

			print "\n";
			print "WARNING: using toolchain ";
			if ($gcc_version ne "") {
				print $gcc_version;
			} else {
				print "binaries";
			}
			print " in:\n$x\n\n";
			print "The recommended toolchain for this release is:\n";
			print "$toolchain\n";
			print "\n";

			for(my $i=0; $i < $annoytime; $i++) {
				print $annoytime - $i . "..."; 
				sleep(1);
			}
			print "\n";
			exit 0;
		}

		# print "Your PATH: $path\n";
		exit 0;
	}
}

if($add == 1) {
	foreach my $x (@searchpath) {
		my $y = "$x/$toolchain/bin";
		if(-e "$y/$TOOLNAME") {
			print "\nFound a matching tool in $y\n";
			write_toolpath($y);
			print "\n";
			# print "Your PATH: $path\n";
			exit 0;
		}
	}
	print "\nDid not find a suitable toolchain '$toolchain' for tool '$TOOLNAME' at top level of @searchpath\n";
	print "\n";

	print "looking deeper for alternates...\n";
	foreach my $sp (@searchpath) {
	print "-> $sp\n";
		find(\&find_othertools, "$sp") if( -e "$sp");
	}

	my $ntools = @othertools;
	if ($ntools < 1) {
		print "\nDid not find any substitute toolchains :-(\n";
		exit 1;
	}

	print "\n Select an alternate toolchain (YMMV)\n";
	print   " -----------------------------\n";
	my $i = 1;
	foreach my $gcc (@othertools) {
		print "$i)\t$gcc\n";
		$i++;
	}

	while(1) {
		print "Selection: ";
		my $sel =  <STDIN>;
		chomp ($sel);
		$sel--;
		if (($sel >= 0)&&($sel < $ntools)) {
			$sel = $othertools[$sel];
			$sel =~ s/\/bin\/$TOOLNAME(.*)//g;
			print " You selected toolchain: $sel\n";
			write_toolpath("$sel/bin",undef);
			exit 0;
		}
	}
	exit 1;

} else {
	print STDERR "\n";
	print STDERR "ERROR: Toolchain '$toolchain' is not in \$PATH for tool '$TOOLNAME'\n";
	print STDERR "\n";
	exit 1;
}
