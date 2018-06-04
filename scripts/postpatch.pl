#!/usr/bin/perl -w
# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
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
my $arg_gdir = undef;
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
	print "-G <generated files base dir>\n";
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

	die "$P: FAIL: non-zero exit code for command:\n[$cmd]\n"
		if (($? >> 8) & 0xff);

	return wantarray ? @a : join("", @a);
}

# getopts ------------------------------------------------------------------

my %cmdflags=();
getopts("sz:p:l:w:f:v:dokD:G:T:", \%cmdflags) or usage_exit();

$arg_zeusver = $cmdflags{z} if (defined $cmdflags{z});
$arg_pfx    = $cmdflags{p} if (defined $cmdflags{p});
$arg_bbl    = $cmdflags{l} if (defined $cmdflags{l});
$arg_bfw    = $cmdflags{w} if (defined $cmdflags{w});
$arg_family = $cmdflags{f} if (defined $cmdflags{f});
$arg_ver    = $cmdflags{v} if (defined $cmdflags{v});
$arg_debug  = $cmdflags{d} if (defined $cmdflags{d});
$arg_odir  = $cmdflags{D} if (defined $cmdflags{D});
$arg_gdir  = $cmdflags{G} if (defined $cmdflags{G});
$arg_tool  = $cmdflags{T} if (defined $cmdflags{T});

$arg_bbl_ver = 0 if (defined $cmdflags{o});
$arg_symlinks = 1 if (defined $cmdflags{s});
$arg_keep = 1 if (defined $cmdflags{k});


# cmdflags checks ----------------------------------------------------------

failed("-z missing zeus version")
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

$arg_gdir = "gen/$arg_family"
	if (!defined $arg_gdir);

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


# security ------------------------------------------------------------------

my %extra_defines = ();

# BOLT binary image
my $image = undef;

# Upgradable Firmware Set (UFS)
my @sections_of_interest = ("FIRST_AVS", "FIRST_MEMSYS", "FIRST_BFW");
my %ufs_list = ();
my $ufs_base_offset = 0xffffffff;
my $ufs_max_offset = 0;
my $ufs_image = undef;
my $ufs_avs_base = 0;
my $ufs_bfw_base = 0;
my $ufs_memsys_base = 0;
my $ufs_max_size = 0;
my $ufs_metadata = 0;
my $bfw_text_offs = 0xffffffff;

# security: helpers ----------------------------------------------------------

sub check_file_is_good($)
{
	my $f = shift;
	die "*** File $f is missing or empty!"
	    if (!(-f $f && -s $f));
}


sub read_textfile($) {
	my $f = shift;
	open(FH, '<', $f)
		or die "Failed to read text '$f'";
	return join('',<FH>);
}


sub read_binfile($) {
	my $f = shift;
	open(my $bh, '<', $f)
		or die "Failed to read binary '$f'";
	binmode $bh;
	undef $/;
	my $in = <$bh>;
	close $bh;
	return $in;
}


sub write_binfile($$) {
	my ($f, $data) = @_;
	open my $fout, ">", $f
		or die "Failed to write binary '$f'";
	binmode $fout;
	print $fout $data;
	close $fout;
}


sub strim {
	my $s = shift;
	$s =~ s/^\s+|\s+$//g;
	return $s
}


# for get_extra_defines
sub left_def_trim {
	my $s = strim(shift);
	$s =~ s/^-D//g;
	$s =~ s/\=.*//g;
	return strim($s)
}


sub right_def_trim {
	my $s = strim(shift);
	$s =~ s/^-D//g;
	$s =~ s/.*\=//g;
	return strim($s)
}


# for get_config_defines
sub left_h_trim {
	my $s = strim(shift);
	$s =~ s/^\#define//g;
	$s = strim($s);
	$s =~ s/(\s+).*//g;
	return strim($s)
}


sub right_h_trim {
	my $s = shift;
	$s =~ s/^\#define//g;
	$s =~ s/.*(\s+)//g;
	return strim($s)
}


sub cfg_is($)
{
	my $ref = shift;

	return 0
		if (! defined $extra_defines{$ref});

	return 0
		if (0 == $extra_defines{$ref});

	return 1;
}


sub cfg_value($)
{
	my $ref = shift;

	return undef
		if (! defined $extra_defines{$ref});

	return scalar($extra_defines{$ref});
}


sub section_top($)
{
	my $section_name = shift;

	my $base = cfg_value($section_name . "_TEXT_OFFS");
	my $size = cfg_value($section_name . "_SIZE");

	return undef
		if ((! defined $base) || (! defined $size));

	return $base + $size;
}


# security: functional blocks ------------------------------------------------

#Exists:
# ENABLE_AVS_INIT
# ENABLE_AVS_FIRMWARE
#
#Enabled:
# BFW_LOAD		0|1
sub get_extra_defines()
{
	my $f = $arg_gdir . "/.common_flags";

	failed( $f . " does not exist")
		if (! -f $f);

	my @lines = split / /, read_textfile($f);

	foreach my $i (@lines)
	{
		$i = strim($i);
		if ($i =~ /^-D/) {
			my $l = left_def_trim($i);
			my $r =  right_def_trim($i);
			if ($l ne $r) {
				$extra_defines{$l} = $r;
			} else {
				# mark as enabled
				$extra_defines{$l} = 1;
			}
		}
	}
}


sub get_config_defines()
{
	my $f = $arg_gdir . "/config.h";

	failed( $f . " does not exist")
		if (! -f $f);

	my @lines = split /\n/, read_textfile($f);

	foreach my $i (@lines)
	{
		$i = strim($i);
		if ($i =~ /#define/) {
			my $l = left_h_trim($i);
			my $r = right_h_trim($i);
			if ($l ne $r) {
				if ($r =~ /^0[xX][0-9a-fA-F]+$/) {
					$extra_defines{$l} = hex $r;
				} else {
					$extra_defines{$l} = $r;
				}
			} else {
				# mark as enabled
				$extra_defines{$l} = 1;
			}
		}
	}
}


sub patch_u32($$$)
{
	my ($img, $hexoffset, $word) = @_;

	my $u32 = pack("V", scalar($word));

	die "bad length " . length($u32) . " for a u32"
		if (length($u32) != 4);

	substr($$img, $hexoffset, 4) = $u32;
}


sub patch_zeus_ver($$)
{
	my ($img, $hexoffset) = @_;

	patch_u32($img, $hexoffset, 0x00040100)
		if (cfg_is("CFG_ZEUS4_1"));

	patch_u32($img, $hexoffset, 0x00040200)
		if (cfg_is("CFG_ZEUS4_2"));

	patch_u32($img, $hexoffset, 0x00040201)
		if (cfg_is("CFG_ZEUS4_2_1"));
}


sub patch_presence($$)
{
	my ($img, $hexoffset) = @_;

	my $second_external = 0x00000000;

	$second_external |= 0x4
		if (!cfg_is("CFG_NOSHMOO"));

	$second_external |= 0x2
		if (cfg_is("ENABLE_AVS_FIRMWARE"));

	# This should only be called if are in the
	# $patched_bolts loop as we know we have a BBL
	# and BFW somewhere.
	$second_external |= 0x1;

	patch_u32($img, $hexoffset, $second_external << 8);
}


sub calc_ufs()
{
	$bfw_text_offs = cfg_value("FIRST_BFW_TEXT_OFFS");

	# hard wired ordering of BFW, AVS & MEMSYS binary blobs
	$ufs_bfw_base = 0x00000;

	$ufs_avs_base = cfg_value("FIRST_AVS_TEXT_OFFS") -
		$bfw_text_offs;

	$ufs_metadata =  cfg_value("UFS_PARAMS");

	$ufs_memsys_base = cfg_value("FIRST_MEMSYS_TEXT_OFFS") -
		$bfw_text_offs;

	$ufs_max_size = (cfg_value("FIRST_MEMSYS_TEXT_OFFS") +
		cfg_value("FIRST_MEMSYS_SIZE")) - $bfw_text_offs
			if ($ufs_max_size == 0);

	if ($arg_debug) {
		printf("   BFW base is %x\n", $bfw_text_offs);
		printf("    BFW offset: %x\n", $ufs_bfw_base);
		printf(" PARAMS offset: %x\n", $ufs_metadata);
		printf("    AVS offset: %x\n", $ufs_avs_base);
		printf(" MEMSYS offset: %x\n", $ufs_memsys_base);
		printf("Max image size: %x\n", $ufs_max_size);
	}

}

sub scan_for_sections_of_interest()
{
	# Only create UFS if USE_FIRST_IMAGES=1
	my $ufs_enable = cfg_value("USE_FIRST_IMAGES");

	print "  SEC     USE_FIRST_IMAGES: " . $ufs_enable . "\n"
		if ($arg_debug);

	return
		if ($ufs_enable == 0);

	# Get a list of thing to be added to UFS
	for my $s (@sections_of_interest) {
		my $base = cfg_value($s . "_TEXT_OFFS");
		next
			if (! defined $base);
		# Add to UFS list
		$ufs_list{$s} = $base;
	}

	# Nothing is above SSBL
	return
		if (!keys %ufs_list);

	# Now get the extent of the external blob we
	# are going to make.
	for my $section (keys %ufs_list) {
		my $base = $ufs_list{$section};
		my $top = section_top($section);

		$ufs_base_offset = $base
			if ($base <= $ufs_base_offset);
		$ufs_max_offset = $top
			if ($ufs_max_offset <= $top);
	}

	# Check that the Zeus suggested max img size is big enough or not.
	my $from_cfg_max = $ufs_max_offset - $ufs_base_offset;

	$ufs_max_size = $from_cfg_max
		if ($from_cfg_max > $ufs_max_size);

	calc_ufs();
}


sub patch_blob($$$)
{
	my ($img, $hexoffset, $blob) = @_;

	substr($$img, $hexoffset, length($blob)) = $blob;
}


sub get_avs_file($)
{
	my ($should_die) = @_;

	if (! cfg_is("ENABLE_AVS_FIRMWARE")) {
		die "AVS firmware not enabled!"
			if ($should_die);

		return undef;
	}

	if (! $extra_defines{AVS_CLASS}) {
		die "AVS class not defined!"
			if ($should_die);

		return undef;
	}

	my $avs = $extra_defines{AVS_CLASS};

	$avs .= "/avs_" . $arg_family . "_fw.bin";

	return $avs
		if (-f $avs && -s $avs);

	die "AVS file $avs - not a file or empty!"
		if ($should_die);

	return undef;
}


sub get_file_size($)
{
	my ($f) = @_;

	my $file_sz = (stat($f))[7];

	if ($arg_debug) {
		printf("  INFO: file '%s' size is 0x%x\n", $f, $file_sz);
	}

	return $file_sz;
}


sub patch_ufs_metadata($$$$$$)
{
	my ($img, $offset, $has_avs, $has_bfw, $has_memsys, $favs) = @_;

	my $ufs_metabase = $offset + $ufs_metadata;

	if ($arg_debug) {
		printf("\n  INFO: patch_ufs_metadata @ %x\n", $ufs_metabase);
	}

	my $content = 0x00000000;
	$content |= 0x4
		if ($has_memsys);

	$content |= 0x2
		if ($has_avs);

	$content |= 0x1
		if ($has_bfw);

	patch_zeus_ver($img, $ufs_metabase);

	patch_u32($img,
		$ufs_metabase + 0x04, $content << 8);

	# blank
	patch_u32($img,
		$ufs_metabase + 0x08, 0x0);

	# blank
	patch_u32($img,
		$ufs_metabase + 0x0c, 0x0);

	if ($has_avs) {
		patch_u32($img,
			$ufs_metabase  + 0x10, $ufs_avs_base);

		my $actual_avs_size = cfg_value("AVS_CODE_SIZE") +
			cfg_value("AVS_DATA_SIZE");

		patch_u32($img,
			$ufs_metabase  + 0x14, scalar $actual_avs_size);
	} else {
		patch_u32($img,
			$ufs_metabase + 0x10, 0x0);

		patch_u32($img,
			$ufs_metabase + 0x14, 0x0);
	}

	# AVS padding
	if (cfg_is("CFG_ZEUS4_1")) {
		patch_u32($img,
			$ufs_metabase + 0x18, 0x0); # FIXME!!!!
	} elsif (cfg_is("CFG_ZEUS4_2_1")) {
		patch_u32($img,
			$ufs_metabase + 0x18, 0x0);
	} elsif (cfg_is("CFG_ZEUS4_2")) {
		patch_u32($img,
			$ufs_metabase + 0x18, 0x100);
	}

	# blank
	patch_u32($img,
		$ufs_metabase + 0x1c, 0x0);

	if ($has_memsys) {
		patch_u32($img,
			$ufs_metabase + 0x20, $ufs_memsys_base);

		my $cfg_memsys_size = cfg_value("FIRST_MEMSYS_SIZE");
		patch_u32($img,
			$ufs_metabase + 0x24, scalar $cfg_memsys_size);
	} else {
		patch_u32($img,
			$ufs_metabase + 0x20, 0x0);

		patch_u32($img,
			$ufs_metabase + 0x24, 0x0);
	}
}


sub assemble_sections_of_interest($$$)
{
	my ($odir, $bfw, $bfw_ver) = @_;

	my $has_avs = 0;
	my $has_bfw = 0;
	my $has_memsys = 0;
	my $avs = undef;

	return
		if (!keys %ufs_list);

	my $memsys = "$odir/memsys.bin";

	for my $section (keys %ufs_list) {
		# Perl 5.8 compat
		if ($section eq "FIRST_AVS") {
			$avs = get_avs_file(0);
			$has_avs = 1
				if (defined $avs);
		} elsif ($section eq "FIRST_BFW") {
			check_file_is_good($bfw);
			$has_bfw = 1;
		} elsif ($section eq "FIRST_MEMSYS") {
			if (! cfg_is("CFG_NOSHMOO")) {
				# A BOLT build should always generate
				# a memsys file.
				check_file_is_good($memsys);
				$has_memsys = 1;
			}
		} else {
			die "Unknown section $section!\n";
		}
	}

	$bfw = "nobfw"
		if (! $has_bfw);

	my $fimage = "$odir/external";
	$fimage .= "_bfw" . $bfw_ver
		if ($has_bfw);
	$fimage .= "_avs"
		if ($has_avs);
	$fimage .= "_memsys"
		if ($has_memsys);
	$fimage .= ".bin";

	print "  EXTERN  $fimage...";

	$ufs_image = chr(0) x ($ufs_max_size);

	if ($has_bfw) {
		my $bimage = read_binfile($bfw);
		patch_blob(\$ufs_image,
			$ufs_bfw_base, $bimage);
	}

	if ($has_avs) {
		my $aimage = read_binfile($avs);
		patch_blob(\$ufs_image,
			$ufs_avs_base, $aimage);
	}

	if ($has_memsys) {
		my $mimage = read_binfile($memsys);
		patch_blob(\$ufs_image,
			$ufs_memsys_base, $mimage);
	}

	# offset into external image for metadata is always UFS_PARAMS
	# with no added offset.
	patch_ufs_metadata(\$ufs_image, 0x00000000,
		$has_avs, $has_bfw, $has_memsys, $avs);

	unlink $fimage
		if (-f $fimage);

	write_binfile($fimage, $ufs_image);
	print "OK\n";
}


# security: main -------------------------------------------------------------

sub do_secpatch($$$$)
{
	my ($odir, $f, $bfw, $bfw_ver) = @_;

	my $avs = undef;
	my $has_avs = 0;
	my $has_memsys = 0;
	my $fname = "$odir/$f";

	print "  SECPATCH " . $f . "\n";
	$image = read_binfile($fname);

	assemble_sections_of_interest($odir, $bfw, $bfw_ver);

	# --- Area reserved in FSBL for security ---
	patch_zeus_ver(\$image, 0x900)
		if (!cfg_is("CFG_ZEUS5_1"));

	patch_presence(\$image, 0x904);
	# make sure signed & enc status are cleaned out
	patch_u32(\$image, 0x908, 0x00000000);
	patch_u32(\$image, 0x90C, 0x00000000);

	# --- Patch metadata into main BOLT image ---

	# test for BFW existence done in @list_pfxs loop
	my $has_bfw = 1;

	$avs = get_avs_file(0);
	$has_avs = 1
		if (defined $avs);

	$has_memsys = 1
		if (! cfg_is("CFG_NOSHMOO"));

	write_binfile($fname, $image)
		if (! cfg_is("CFG_ZEUS5_1"));
}


# doit ---------------------------------------------------------------------

my $patched_bolts = 0;

# init security things
get_extra_defines();
get_config_defines();
scan_for_sections_of_interest();


for (my $i = 0; $i < @list_pfxs; $i++) {

	my $r;
	my $meta = "BOLT-META:";
	my $file_bbl = "security/" . $arg_family . "/bbl-" . $list_bbls[$i] . ".bin";
	my $file_bfw = "security/" . $arg_family . "/bfw-" . $list_bfws[$i] . ".bin";

	# do as a matched/compatible set
	if ( (! -f $file_bbl) || (! -f $file_bfw)) {
		if ($arg_debug) {
			print "  INFO: skipping as ";
			print "$file_bbl or $file_bfw does not exist\n";
		}
		next;
	}

	my $bname = "bolt-" . $arg_ver . "-";
	if (cfg_is("CFG_ZEUS5_1")) {
		$bname .= substr($arg_family,0,4) . $list_pfxs[$i] . "-ba";
	} else {
		$bname .= $arg_family . "-" . $list_pfxs[$i];
	}

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

	if (cfg_is("CFG_ZEUS5_1")) {
		my $file_reserveddata = "security/" . $arg_family . "/reserveddata-" . $list_pfxs[$i] . ".bin";
		my $file_zeus51_mark = "security/" . "/zeus51-mark.bin";
		my $sub_reserveddata = "-t reserveddata -p $file_reserveddata -i $arg_odir/$bname";
		my $sub_zeus51_mark = "-t zeus51mark -p $file_zeus51_mark -i $arg_odir/$bname";

		if (-f $file_reserveddata) {
			$r = do_shell("$cmd $sub_reserveddata");
			print "\n$r"
				if ($arg_debug);
		}

		if (-f $file_zeus51_mark) {
			$r = do_shell("$cmd $sub_zeus51_mark");
			print "\n$r"
				if ($arg_debug);
		}
	}
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

	# note we're doing the final products, not bolt.bin
	# This is so we may in the future do different things
	# for different BBL & BFW versions.
	do_secpatch($arg_odir, $bname, $file_bfw, $list_bfws[$i])
		if ((defined $ufs_metadata) && ($ufs_metadata != 0));

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

