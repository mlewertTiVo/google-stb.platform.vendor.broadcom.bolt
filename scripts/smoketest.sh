#!/bin/bash
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

XCHIP=7445a0
OBJDIR=objs

tstart=`date +%s`

if [ -e bolts ]; then
  rm -rf bolts/
fi
mkdir bolts

echo "============== standard bolts ================="
make distclean
make toolfind
perl scripts/make_all.pl

for fam in `make show`
do
 if [ ! -e ${OBJDIR}/${fam}/bolt.bin ]; then
  echo "FAILED ${OBJDIR}/${fam}/bolt.bin";
  exit
 fi
 cp ${OBJDIR}/${fam}/bolt.bin bolts/bolt-${fam}.bin
done

echo "============== ddrtests ================="
make clean FAMILY=${XCHIP}
make toolfind
make ${XCHIP} CFG=config/testing/ddrtests-${XCHIP}.cfg
if [ ! -e ${OBJDIR}/${XCHIP}/bolt.bin ]; then
 echo "FAILED config/testing/ddrtests-${XCHIP}.cfg";
 exit
fi
cp ${OBJDIR}/${XCHIP}/bolt.bin bolts/ddrtests-bolt-${XCHIP}.bin 

echo "============== oneboard ================="
make clean FAMILY=${XCHIP}
make toolfind
make ${XCHIP} CFG=config/testing/family-${XCHIP}-oneboard.cfg
if [ ! -e ${OBJDIR}/${XCHIP}/bolt.bin ]; then
 echo "FAILED config/testing/family-${XCHIP}-oneboard.cfg";
 exit
fi
cp ${OBJDIR}/${XCHIP}/bolt.bin bolts/oneboard-bolt-${XCHIP}.bin 


echo "============== emu ================="
make clean FAMILY=${XCHIP}
make toolfind
make emu FAMILY=${XCHIP}
if [ ! -e ${OBJDIR}/${XCHIP}/fsbl.bin ]; then
 echo "FAILED emu 1";
 exit
fi
cp ${OBJDIR}/${XCHIP}/fsbl.bin bolts/emu-fsbl-${XCHIP}.bin 

if [ ! -e ${OBJDIR}/${XCHIP}/fsbl_flash.srec ]; then
 echo "FAILED emu 2";
 exit
fi
cp ${OBJDIR}/${XCHIP}/fsbl_flash.srec bolts/emu-fsbl-${XCHIP}.srec 


echo "============== fullemu ================="
make clean FAMILY=${XCHIP}
make toolfind
make fullemu FAMILY=${XCHIP}
if [ ! -e ${OBJDIR}/${XCHIP}/bolt.bin ]; then
 echo "FAILED emu 1";
 exit
fi
cp ${OBJDIR}/${XCHIP}/bolt.bin bolts/fullemu-bolt-${XCHIP}.bin 


echo "============== completed ================="
echo done in $(expr `date +%s` - $tstart) Seconds
echo 
ls -l bolts

