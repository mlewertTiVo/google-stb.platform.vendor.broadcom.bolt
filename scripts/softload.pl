#!/usr/bin/perl

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

use Getopt::Long;
use lib '/projects/stbgit/scripts/bin/.plib';
use Broadcom::RdbLive;
local $| = 1;
use strict; use warnings;

# --------------------------

my $IPADDR = "0.0.0.0";
my $ssbl = "softload.bin";
my $wipeonly = 0;
my $erase = 0;
my $loadaddr = 0x0;

# --------------------------

GetOptions(
 'file=s' => \$ssbl,
 'ip=s'   => \$IPADDR,
 'wipeonly'   => \$wipeonly,
 'erase'   => \$erase,
 'addr=o' => \$loadaddr);

# --------------------------

my $s = Broadcom::RdbLive->new($IPADDR);

$s->grok('bchp_sun_top_ctrl.h');

printf "Chip family:  %08x\n", $s->rreg('SUN_TOP_CTRL_CHIP_FAMILY_ID');

# --------------------------

my $soap_lump = 4096;
my $filesize = -s $ssbl;

$filesize = 1024*512 if (! defined $filesize || $filesize < 1);

# --------------------------

my $wordsize = $filesize / 4;
my $lumpsize = ($wordsize + $soap_lump) & ~($soap_lump -1);
my $loadtop =  $loadaddr + $filesize - 4;
my $lumpbytes = $lumpsize * 4;
my @data = (0..$lumpsize * 2);
my $i;

# --------------------------

sub readfile() {
    open my $fh, "<", $ssbl or die "Error opening $ssbl";
    binmode $fh;

    printf("load 0x%08x to 0x%08x, %d bytes, %d words, %s lump\n", 
        $loadaddr, $loadtop, $filesize, $wordsize, $lumpsize);

    for($i = 0; $i < $wordsize; $i++) {
        my $word;
        if (read($fh, $word, 4) != 4) {
            printf("done reading. 0x%08x / %d 0x%08x\n", $i,  $i);   
            last;
        }
        my $d = unpack('V', $word);
        $data[$i] = $d;
    }

    close $fh;

    printf(" 0x%08x\n 0x%08x \n", $data[0], $data[1]);
}


# --------------------------

sub clearmem() {
    my @blank = 0xdeadbeef x $soap_lump;

    printf("clear... 0x%08x           ", $loadaddr );
    for($i = 0; $i < $lumpbytes; $i = $i + $soap_lump) {
        $s->wmem($loadaddr + $i, @blank);
        printf("\b\b\b\b\b\b\b\b\b\b0x%08x", $loadaddr + $i);
    }
    printf(" ok\n");
}

# --------------------------

sub programmem() {
    my $count = 0;
    my $ab = 0;
    my $ae = 0;

    printf("LOAD.... 0x%08x           ", $loadaddr );
    for($i = 0; $i < $lumpbytes; $i = $i + $soap_lump) {
        $ab = $i/4;
        $ae = ($i + $soap_lump)/4;
        my @d = @data[$ab..$ae];
        $s->wmem($loadaddr + $i, @d);
        printf("\b\b\b\b\b\b\b\b\b\b0x%08x", $loadaddr + $i);
    }
    printf("\nok\n");
}


# --------------------------

sub demoprint() {
    printf("data \t0x%08x\t0x%08x\n", $data[0], $data[1]);
    my $r0 = $s->rmem( $loadaddr+0, 1);
    my $r1 = $s->rmem( $loadaddr+4, 1);
    printf("actual\t0x%08x\t0x%08x\n", $r0, $r1);
}

# --------------------------
# main

clearmem() if ($erase != 0 || $wipeonly != 0);
exit if ($wipeonly != 0);

readfile();

programmem();

demoprint();

