#!/bin/bash

# ***************************************************************************
# *     Copyright (c) 2012-2014, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# * 
# ***************************************************************************

echo "Don't run this script!"
exit

#  Instead of releasing the whole RDB for every chip family
# and eventually suffering massive release package bloat,
# we can find those headers which do appear in the code and
# keep them. The rest get prefixed with a dot so they are
# not included in the release package (tar --exculde '.*')
# but can be un-dotted if we do need to bring them back in.
# If customers want specific headers then we can either 
# release on a per-customer basis, or they can go get them
# from NEXUS (magnum subdir.)


if [ $# -ne 1 ]
then
  echo "Usage: `basename $0` {FAMILY}"
  exit 22
fi

#make clean cfgclean 
#make FAMILY=$1 && make clean

# have to sort this grepping -v out later.

used=\
`find avs fsbl ssbl shmoo include -type f -name *.[chSs] | \
grep -v include/7445a0 | \
grep -v include/7445b0 | \
grep -v include/7145a0 | \
grep -v include/7366a0 | \
xargs sed -n /bchp_/p | sed "s/\"//g" | sed "s/>//g;" | sed "s/<//g;"  | \
awk '{print $2}' | sort -u`

files=`find include/$1 -type f -name *.h`

echo -n "moving..."
for f in $files
do
 mv include/$1/${f##*/} include/$1/.${f##*/}
 #echo mv include/$1/${f##*/} include/$1/.${f##*/}
done
echo "done"

count=0

for f in $files
do
 for u in $used
 do
  ug=include/$1/$u
  if [ $f = $ug ]; 
  then
   ((count++))
   echo "$count match " $ug
   mv include/$1/.${f##*/} include/$1/${f##*/}
   #echo mv include/$1/.${f##*/} include/$1/${f##*/}
  fi
 done
done

ls -l include/$1/bchp_* | wc -l

sleep 4

make clean cfgclean 
make FAMILY=$1

