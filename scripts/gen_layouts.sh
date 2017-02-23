#!/bin/bash
# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

#
#  This will need to be changed if we alter the
# 'section' cfg script command format.
#

# The current format inserted into scripts/bpatch.pl is
# an array of STRA,STRB,HEX as follows:
#
#my @configs = (
#	["zeus", "bbl", 0x000096F0],
#	["zeus", "bfw", 0x00020000],
#	["zeus42", "bbl", 0x000096F0],
#	["zeus42", "bfw", 0x00020000],
#);
#
# STRA is derived from the file name
# STRB is the section name
# HEX is the base/offset parameter
#
# section -name STRB -off HEX -size HEX2

#  The input file name must be of the form:
# config/layout-<ZEUSVER>.cfg and an exception
# is made for the old config/layout.cfg file.

if [ ! -d config ]; then
 echo "can't find config dir"
 exit -1
fi


function find_section() {
 while read -r line
 do
  IFS=$' \t' read -ra ARR <<< "$line"
  if [[ "${ARR[0]}" == "section" ]]; then
   if [[ "${ARR[2]}" == $2 ]]; then
    LC_SEC=$(echo $2 | tr '[:upper:]' '[:lower:]')

    if [[ "${ARR[3]}" == "-off" ]]; then
     echo "	[\"$3\", \"${LC_SEC}\", ${ARR[4]}], "
    else
      if [[ "${ARR[5]}" == "-off" ]]; then
       echo "	[\"$3\", \"${LC_SEC}\", ${ARR[6]}], "
      else
       exit 1
      fi
    fi

   fi
  fi
 done < "$1"
}


echo "#!/usr/bin/perl -w"
echo "my @configs = ("
for f in config/layout*.cfg
do
 Z=$(sed "s/config\/layout//g;s/.cfg//g;s/-//g;" <<< "$f")
 if [[ ${#Z} -le 0 ]]; then
  Z="zeus"
 fi
 find_section $f "KEY" $Z
 find_section $f "BBL" $Z
 find_section $f "BFW" $Z
 find_section $f "FSBL" $Z
 find_section $f "MEMSYS" $Z
 find_section $f "MEMSYS_ALT" $Z
 find_section $f "SHMOO" $Z
 find_section $f "BOARDS" $Z
 find_section $f "AVS" $Z
 find_section $f "SSBL" $Z
 find_section $f "FIRST_BFW" $Z
 find_section $f "FIRST_AVS" $Z
 find_section $f "FIRST_MEMSYS" $Z
done
echo ");"

cat ./scripts/bpatch.pl

exit 0

