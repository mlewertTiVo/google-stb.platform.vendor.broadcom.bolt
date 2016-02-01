#!/usr/bin/perl -w

use POSIX;
use strict;

# path to headers
my $rdb_path = "/projects/bseswev_nonos/nightly/rdb";

sub install_bchp($$$$)
{
	my($chip, $rev, $in, $out) = @_;

	local(*IN, *OUT);

	open(IN, "<$in") or die "can't open $in: $!";

	my $skip = 0;
	my(@outtxt, @outdefs);

	@outtxt = ( );
	my @outregs = ( );

	while(<IN>)
	{
		if($skip && m/\*{20,}/) {
			push(@outtxt, $_);
			$skip = 0;
		} elsif(m/\$brcm_/) {
			if(m/brcm_Log/)
			{
				push(@outtxt, $_);
			}
			$skip = 0;
		} else {
			if(! $skip)
			{
				push(@outtxt, $_);
				if($_ =~ /#define/) {
					push(@outregs, $_);
				}
			}
		}
	}
	close(IN);

	chmod(0644, $out);

	# Do not overwrite the output file unless one or more
	# register definitions (#define lines) has changed.  If the only
	# change is the md5/date/comments in the header, skip it.

	open(OUT, "<$out") or die "can't open $out: $!";

	my @oldregs;
	my $i = 0;
	my $changed = 0;

	# read in the old file and gather all register defs

	while(<OUT>) {
		if(m/#define/) {
			push(@oldregs, $_);
		}
	}
	close(OUT);

	# compare old and new register defs

	foreach my $x (@outregs) {

		# old version is incomplete/truncated
		if(!defined($oldregs[$i])) {
			$changed = 1;
			last;
		}

		my $y = $oldregs[$i];

		# old version has outdated content
		if($x ne $y) {
			$changed = 1;
			last;
		}
		$i++;
	}

	if($changed == 0) {
		#print "SKIP: $out\n";
		return;
	}
	print "UPDATE: $out\n";

	open(OUT, ">$out") or die "can't open $out: $!";
	foreach my $x (@outtxt) {
		print OUT $x;
	}
	close(OUT);
}

#
# MAIN
#

my $ret = 0;

my %chip_hash;
my $n_chips = 0;

while(defined($ARGV[0])) {
	my $a = $ARGV[0];
	$chip_hash{$a} = 1;
	$n_chips++;
	shift @ARGV;
}

if(-d "$rdb_path/vobs/magnum/basemodules/chp/") {
	$rdb_path = "$rdb_path/vobs/magnum/basemodules/chp";
}

if(! -f "$rdb_path/bchp.h") {
	die "can't find bchp.h in $rdb_path";
}

my $common = "include";
my $chip_pfx = "$common/";
my $hdr_pfx = "";
my $cfg_pfx = "CONFIG_BCM";

my @chips = glob("${chip_pfx}[0-9]*");
my @schips = ( );

foreach (@chips)
{
	if(m!((/brcm9)|/)([0-9]{4,5}[a-e]0)$!)
	{
		push(@schips, $3);

		# if a list was not provided on the command line, select
		# ALL chips for updating
		if($n_chips == 0) {
			$chip_hash{$3} = 1;
		}
	} else {
		die "bad chip: $_";
	}
}

@schips = sort { $a cmp $b } @schips;
my $first = 1;

foreach (@schips)
{
	m/([0-9]{4,5})([a-e]0)/ or die;
	my($chip, $rev) = ($1, $2);

	my @bchps = sort { $a cmp $b }
		glob("${chip_pfx}${chip}${rev}/bchp_*.h");
	
	my $caprev = $rev;
	$caprev =~ tr/[a-z]/[A-Z]/;
	my $if;

	if($first)
	{
		$if = "#if";
		$first = 0;
	} else {
		$if = "\n#elif";
	}

	foreach my $b (@bchps)
	{
		my $bbase = $b;
		$bbase =~ s|.*/||;

#		if(! -s $b)
#		{
#			next;
#		}

		my $f = "$rdb_path/$chip/rdb/$rev/$bbase";
		if(-f $f) {
			if (defined($chip_hash{"${chip}${rev}"})) {
				install_bchp($chip, $rev, $f, $b);
			}
		} else {
			print "ERROR: missing $f\n";
			$ret = 1;
		}
	}
}

exit $ret;
