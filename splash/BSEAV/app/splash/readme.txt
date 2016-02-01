         ********************************************************
         * How to use splash tool on broadcom's reference board *
         ********************************************************

---------------------------------
Features supported : 
---------------------------------
	1. RGB565 and ARGB8888 surfaces.
	2. HD Output support
	3. 480p+NTSC support
	4. 576p + PAL supprot

---------------------------------
What does the package contain ?
---------------------------------
	1. Two linux usermode applications
		a) splashgen - generates the register and rul dumps
		b) splashrun - Simple utility to download and run the VDC 
		   scripts. This loader splash_script_load.c has also been 
		   ported to the CFE
	   Both have portable libary components with simple apps wrapping 
	   them for the linux usermode environment. 
	2. Magnum basemodules/commonutils/portinginterace used by the 
	   splashgen and splashrun. Some of the files may be different from 
	   the regular refsw files.
	3. The nexus build framework and user mode board support package 
	   for the 97xxx board.

---------------------------------
Requires : 
---------------------------------
	1. The usermode driver compiled for linux 2.6.x-y.z (as bcmdriver.ko)


---------------------------------
Build and Execute Instructions : 
---------------------------------
1. Splashgen make Instructions
    - > Make sure you have the instrumented version of breg_mem.c on SPLASH_Devel branch
    - > cd BSEAV\app\splash\splashgen
    - > Edit BSEAV\app\splash\splashgen\ChipName\bsplash_board.h to your required confiuration.
    - > plat 97405 B0
    - > make
    - With above instructions, the binary will be generated in nexus\bin directory.
2. Running Splashgen
    - When splashgen run in this directory as the bmp file will display the bmp after the dump files are created.
    - Copy the bmp file into nexus/bin directory.
    - > cd nexus\bin
    - > nexus splashgen
    - Make sure HDMI is connected.
    - Check and make sure the logo is displayed on all outputs of interest.
    - Look for these 2 files in the directory after exiting splashgen.
        - splash_vdc_reg.h
        - splash_vdc_rul.h
3. Splashrun make Instructions
    - The splashrun utlity is a simple loader function that loads the C register dumps and the RUL dumps and triggers the BVN.
    - Copy the splash script files splash_vdc_reg.h and splash_vdc_rul.h created by splashgen to the 
       BSEAV\app\splash\splashrun directory.
    - > cd BSEAV\app\splash\splashrun
    - > plat 97405 B0
    - > make
    - This above instructions will generate the splashrun binary in nexus/bin directory.
4. Running Splashrun
    - cd nexus\bin
    - > splashrun
    - Check and make sure the logo is displayed on all outputs of interest.

         ****************************************************
         * How to customize splash tool on customer's board *
         ****************************************************
Two file that need to be changed 
	BSEAV\app\splash\splashgen\93549\bsplash_board.h, 
	BSEAV\app\splash\splashgen\93549\platformconfig.c, 
in this file, customer can add or remove registers that need to be excluded from the dump in the splash_vdc_reg.h based on their own 
board.
Generally, PWM, PIN MUX, GPIO, LVDS, DVPO will be involved.

---------------------------------
  File description
---------------------------------
       splash_vdc_reg.h - Register script for getting the splash screen output by splashgen
       splash_vdc_rul.h - Prebuilt RULs for getting the splash screen output by splashgen
       splash_vdc_setup.c - Key file that does all the loading of the REG and 
           RUL scripts.
           This file is has portable functions for loading the register script 
           and the RULs into appropriate memory location and triggering the 
	   Video backend
           It uses the magnum functions 
	BREG_Write32
	BMEM_AllocAligned
	BKNI_Memcpy
	BMEM_ConvertAddressToOffset
           Each of these magnum functions can have a cheaper alternate 
	   definition for the environment in question. This is implemented 
	   in splash_magnum.h in the BSEAV\app\splash\os\cfe
	BRDC_OP_IMM_TO_REG - magnum/commonutils/rdc/7038/brdc.h
	BRDC_REGISTER - magnum/commonutils/rdc/7038/brdc.h
	BCHP_FIELD_DATA - magnum/basemodules/chp/bchp.h
       splash_vdc_run.c - Top level file demonstrates calls to the 
           splash_vdc_setup.c
       bsplash_board.h - Files implements 
           1. marcos for controlling the output configration
           2. Exclude marco for excluding registers from the dump
	   3. DACs on the board used for various outputs
	   4. Surface type
	   etc.
       platformconfig.c - This configures the top level pin muxes to get the desired output on 
           on the custom board.
       splash_bmp.c - Implements graphics operations like rendering BMP files with RGB565/ARGB8888.
