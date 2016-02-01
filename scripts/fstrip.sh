#/bin/bash
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
#
# Strip out chips from a release package.

if [ "$#" == "0" ]; then
	echo "usage: scripts/fstrip.sh bolt-$RELEASE.tgz FAMILY FAMILY..."
	echo "   e.g scripts/fstrip.sh bolt-v0.53.tgz 74xxxa0 74xxxb0"
	echo
	exit 1
fi


TARFILE=$1
TARDIR="${TARFILE%.*}"

echo

mkdir -p $TARDIR
if [ ! -d $TARDIR ]; then
 echo "Could not make $TARDIR directory!"
 exit
fi

rm -rf bolts
tar -xzf $TARFILE

# bolt.bin package only bolts/bolt-v${VER}-${FAMILY}-*.bin
#
if [ -d bolts ]; then
  pushd bolts 2>&1 >/dev/null
  for var in "${@:2}"
  do
    echo "removing $var from $TARDIR"
    rm -f bolt-*-$var-*.bin
  done
  echo
  popd 2>&1 >/dev/null
  tar -czf $TARDIR-customer.tgz bolts/*.bin
  tar -tzf $TARDIR-customer.tgz
  echo
  rm -rf $TARDIR
  exit
fi

# bolt source files
#
pushd $TARDIR 2>&1 >/dev/null
pushd include 2>&1 >/dev/null
echo -n " RDBS: "
ls -d [0-9]*
popd 2>&1 >/dev/null

for var in "${@:2}"
do
    echo "removing include/$var"
	rm -rf "include/$var"

    echo "removing shmoo/memsys-$var.c"
	rm -rf "shmoo/memsys-$var.c"

	if [ -e "config/family-$var.cfg" ]; then
		echo "removing config/family-$var.cfg"
		rm -f "config/family-$var.cfg"
	fi
	if [ -e "config/family-$var.dts" ]; then
		echo "removing config/family-$var.dts"
		rm -f "config/family-$var.dts"
	fi
	if [ -e "config/family-$var.rts" ]; then
		echo "removing config/family-$var.rts"
		rm -f "config/family-$var.rts"
	fi

	echo "removing any config/family-$var-box*.rts files"
	rm -f config/family-$var-box*.rts

	if [ -e "config/clks-$var.plx" ]; then
		echo "removing config/clks-$var.plx"
		rm -f "config/clks-$var.plx"
	fi

	if [ -e "security/$var" ]; then
		echo "removing security/$var"
		rm -fr "security/$var"
	fi
	
	SH=`find shmoo -name mcb-$var.c`
	if [ -e "$SH" ]; then
		echo "removing SHMOO data $SH"
		rm -f "$SH"
	fi 

	ucvar=`echo $var | tr '[:lower:]' '[:upper:]'`
	for m in `find shmoo -name $ucvar*.mcb`;
        do
		echo "removing MCB $m"
		rm -f "$m"
        done    
        
	echo
done

popd 2>&1 >/dev/null

tar -czf $TARDIR-customer.tgz $TARDIR

rm -rf $TARDIR

