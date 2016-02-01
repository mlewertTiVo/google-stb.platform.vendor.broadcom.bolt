#!/usr/local/bin/perl -w

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
use Cwd;

my $P = basename $0;
my $TMPDIR = $ARGV[0];

# Just a wrapper to announce and do a shell command.
sub do_shell($)
{
	my $cmd = shift;
	print "    [$cmd]\n";
	my @a = `$cmd`;
	die "$P: Err: non-zero exit code.\n"
		if (($? >> 8) & 0xff);
	return wantarray ? @a : join("", @a);
}


# Just a wrapper to announce and do a chdir command.
sub do_chdir($)
{
	my $dir = shift;
	print "    [chdir $dir]\n";
	chdir $dir
		or die "$P: Err: could not chdir('$dir').\n";
}

my @excludes = (
"scripts/unuse.sh",
"scripts/bump.sh",
"scripts/fstrip.sh",
"scripts/release.pl",
"scripts/softload.pl",
"doc/release-checklist.txt",
"scripts/bchp_file_excluder.pl",
"scripts/smoketest.sh",
"scripts/update_bchp.pl",
"scripts/relnotes.pl");

# Filter out superfluous or sensitive files.
sub filter($)
{
	my $ra = shift;
	my (@a, $i);
	for ($i=0; $i<@$ra; $i++) {
		my $file = $ra->[$i];
		chomp $file;
		my $inlist = 0;
		# Beg filter here
		foreach (@excludes) {
			if ( $_ eq $file ) {
				$inlist = 1;
			} 
		}
		# filter out 'dot' files
		if (basename($file) =~ /^[.]/) {
			$inlist = 1;
		}
		# filter out 'gpg' files
		if (basename($file) =~ /\.gpg$/i) {
			$inlist = 1;
		}
		# End filter here
		if ( $inlist == 0 ) {
			push @a, $file;
		}
		else {
#			print "==> excluded $file\n";
		}
	}
	return @a;
}


#
sub main()
{
	die "$P: Err: not in top directory of a git repo!\n"
		if (!-d 'ssbl' || !-d 'fsbl' || !-d '.git');

	my $version = do_shell("git describe --dirty 2> /dev/null " .
		"|| git rev-parse --short HEAD 2> /dev/null");
	chomp $version;

	my $name = "bolt-$version";
	my @bolts = ();
	my $b = "";

	unlink "$name.tgz";
	unlink "$name.bin";
	unlink ".bolt-package-name";

	my @a = do_shell("git ls-files");
	@a = filter(\@a);
	@a = map { "$name/$_" } @a;
	push @a, "$name/version\n"; # synthetic file.
	
	my $root = cwd;
	my $tdir = $TMPDIR . "/bolt-" . time . "-$$";
	
	do_shell("mkdir -p $tdir");
	do_chdir($tdir);
	my $manifest = 'manifest.txt';
	open(my $fh, '>', $manifest)
		or die "$P: Err: could not open '$manifest'.\n";
	print $fh join("\n", @a);
	do_shell("git clone $root/.git $name");

	# add HTML documentation
	do_shell("make -C $name doc");
	my @htmldocs = glob("$name/doc/*.html");
	print $fh join("\n", @htmldocs) . "\n";
	close($fh);

	# finally add the version info and create the tarball
	do_shell("echo \"$version\" > \"$name/version\"");
	do_shell("tar czf $name.tgz -T $manifest");

	# Now make sure that the tarball 'makes' properly.
	do_shell("mkdir build");
	do_chdir("build");
	do_shell("tar xzf ../$name.tgz");
	do_chdir("$name");

	do_shell("make toolfind");
	do_shell("make bolts");

	opendir(BOLTS, "bolts") or die $!;
	while (my $f = readdir(BOLTS)) {
	        next if ($f =~ m/^\./);
		do_shell("sed -i 's/LOCAL BUILD/RELEASE    /g;' bolts/$f");
	}

	# Move the tarball back to the user's cwd.
	do_shell("mv ../../$name.tgz $root");
	do_shell("tar -czf $root/$name-binary.tgz bolts");

#	do_shell("cp release-notes.txt $root/$name-notes.txt");
	do_shell("echo \"$name\" > $root/.bolt-package-name");

	do_shell("rm -fr $tdir")
		if (length($tdir) > 5 && $tdir =~ /^\S{5}/);

	printf "\n$P: Info: tarball is '$name.tgz'.\n";
	printf   "$P: Info: binary  is '$name-binary.tgz'.\n";
	#printf "$P: Info: notes   is '$name-notes.txt'.\n";
	return 0;
}

exit main;


