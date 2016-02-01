
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

my $in1, $out, $offset;
my $mcb_file_size, cfe_file_size;
my $mcb_path, $chip, $ddr_freq, $ddr_width, $ddr_dev_density, $ddr_dev_width, $endian, $profile_name; 

if ($#ARGV+1 == 3 && @ARGV[0] =~ /.*\.mcb$/)
{
    $in1 = @ARGV[0]; 
    $out = @ARGV[1]; 
    $offset = @ARGV[2];

    unless (-e $in1)
    {
        print "ERROR: $in1 does not exist!\n";
        system("rm cfe.bin");
        exit 1;
    }

    unless (-e $out)
    {
        print "ERROR: $out does not exist!\n";
        system("rm cfe.bin");
        exit 1;
    }

    $mcb_file_size = -s $in1;
    $cfe_file_size = -s $out;
    unless ($offset >= 0 && $offset + $mcb_file_size <= $cfe_file_size)
    {
        print "ERROR: incorrect MCB offset!\n";
        system("rm cfe.bin");
        exit 1; 
    }
}
elsif ($#ARGV+1 == 9 || $#ARGV+1 == 10) {
    $mcb_path = @ARGV[0];
    $chip = @ARGV[1];
    $ddr_freq = @ARGV[2];
    $ddr_width = @ARGV[3];
    $ddr_dev_density = @ARGV[4];
    $ddr_dev_width = @ARGV[5];
    $endian = @ARGV[6];
    $out = @ARGV[7];
    $offset = @ARGV[8];

    if ($#ARGV+1 == 10) {
        $profile_name = @ARGV[9];
        $in1 = $mcb_path."/".$chip."_".$ddr_freq."MHz_".$ddr_width."b_dev".$ddr_dev_density."x".$ddr_dev_width."_DDR3_".$profile_name."_".$endian.".mcb";
        print "MCB file: ".$in1."\n";
    }
    else {
        my @in_files = glob $mcb_path."/".$chip."_".$ddr_freq."MHz_".$ddr_width."b_dev".$ddr_dev_density."x".$ddr_dev_width."_DDR3_*_".$endian.".mcb";

        if (@in_files) {
            foreach my $mcb_file (@in_files) {
                $in1 = $mcb_file;
                print "MCB file: ".$in1."\n";
            }
        }
        else
        {
            print "ERROR: MCB file does not exist!\n";
            system("rm cfe.bin");
            exit 1;
        }
    }

    unless (-e $in1)
    {
        print "ERROR: $in1 does not exist!\n";
        system("rm cfe.bin");
        exit 1;
    }

    unless (-e $out)
    {
        print "ERROR: $out does not exist!\n";
        system("rm cfe.bin");
        exit 1;
    }

    $mcb_file_size = -s $in1;
    $cfe_file_size = -s $out;
    unless ($offset >= 0 && $offset + $mcb_file_size <= $cfe_file_size)
    {
        print "ERROR: incorrect MCB offset!\n";
        system("rm cfe.bin");
        exit 1; 
    }
}
else {
    print "\nUsage: if MCB file is specified, \"add_mcb mcb_file cfe_file mcb_offset\"\n";
	print "   or: if DDR configurations are specified, \"add_mcb MCB_file_path, chip_name, DDR_freq, DDR_interface_width, DDR_device_density, DDR_device_width, endian, [DDR_device_speed_grade]\"\n";
	print "This tool inserts MCB binary file to CFE binary file at the specified offset.\n\n";
    system("rm cfe.bin");
    exit 1;
}

$offset = int($offset);

printf("MCB offset = 0x%x\n", $offset);

$in_file_size = -s $in1;
printf("mcb file size = 0x%x bytes\n", $in_file_size);

$cfe_file_size = -s $out;

printf("cfe file_size = 0x%x bytes\n", $cfe_file_size);

if ( $offset >= $cfe_file_size )
{
   print "ERROR: mcb offset >= cfe file size!\n";
   system("rm cfe.bin");
   exit 1;
}

open(IN, "< $in1") or do {system("rm cfe.bin"); print "ERROR: can't open $in1: $!\n"; exit 1;};
binmode(IN);
read(IN, $buffer, $in_file_size) == $in_file_size or do {system("rm cfe.bin"); print "ERROR: couldn't read from $in1 : $!\n"; exit 1;};
close(IN);

open(OUT, "+< $out") or do {system("rm cfe.bin"); print "ERROR: can't open $out: $!\n"; exit 1;};
binmode(OUT);

seek(OUT, $offset, SEEK_SET) or do {system("rm cfe.bin"); print "ERROR: Couldn't seek to $offset: $!\n"; exit 1;};
$pos = tell(OUT);
#print "Move to $pos bytes from the start of $out. Start inserting...\n";
print OUT $buffer;
close(OUT);	
print "Done merging MCB file to cfe.bin.\n";
exit 0;
