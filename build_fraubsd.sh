#!/bin/sh
#-
############################################################ IDENT(1)
#
# $Title: Script to build SGE and install it to package sandbox $
# $Copyright: 2019-2020 Devin Teske. All rights reserved. $
# $FrauBSD: sge/build_fraubsd.sh 2020-05-17 12:57:43 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=install/opt/ge6.2
case "${UNAME_r:=$( uname -r )}" in
2.*)
	JAVA_HOME=/usr/lib/jvm/java-1.7.0-openjdk-1.7.0.261.x86_64
	JAVA_PKG=java-1.7.0-openjdk-devel
	DB4=db4
	MOTIF=openmotif
	;;
*)
	JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.252.b09-2.el7_8.x86_64
	JAVA_PKG=java-1.8.0-openjdk-devel
	DB4=libdb4
	MOTIF=motif
esac

############################################################ GLOBALS

#
# ANSI
#
ESC=$( :| awk 'BEGIN { printf "%c", 27 }' )
ANSI_BLD_ON="$ESC[1m"
ANSI_BLD_OFF="$ESC[22m"
ANSI_GRN_ON="$ESC[32m"
ANSI_FGC_OFF="$ESC[39m"

############################################################ FUNCTIONS

eval2()
{
	echo "$ANSI_BLD_ON$ANSI_GRN_ON==>$ANSI_FGC_OFF $*$ANSI_BLD_OFF"
	eval "$@"
}

############################################################ MAIN

set -e

#
# Install dependencies
#
items_needed=
#	bin=someprog:pkg=somepkg \
#	file=/path/to/some_file:pkg=somepkg \
#	lib=somelib.so:pkg=somepkg \
for entry in \
	bin=csh:pkg=tcsh \
	file=$JAVA_HOME/include/jni.h:pkg=$JAVA_PKG \
	file=/usr/include/$DB4/db.h:pkg=$DB4-devel \
	file=/usr/lib64/libssl.a:pkg=openssl-static \
	file=/usr/include/security/_pam_types.h:pkg=pam-devel \
	file=/usr/include/Xm/BulletinB.h:pkg=$MOTIF-devel \
	file=/usr/include/freetype2/freetype/freetype.h:pkg=freetype-devel \
; do
	check="${entry%%:*}"
	item="${check#*=}"
	case "$check" in
	 bin=*) type "$item" > /dev/null 2>&1 && continue ;;
	file=*) [ -e "$item" ] && continue ;;
	 lib=*) ldconfig -p | awk -v lib="$item" \
		'$1==lib{exit f++}END{exit !f}' && continue ;;
	     *) continue
	esac
	pkg="${entry#*:}"
	pkgname="${pkg#*=}"
	items_needed="$items_needed $pkgname"
done
[ "$items_needed" ] && eval2 sudo yum install $items_needed

#
# Build software
#
eval2 cd source
eval2 export JAVA_HOME=$JAVA_HOME
case "$UNAME_r" in
2.*)
	eval2 export SGE_INPUT_LDFLAGS=
	eval2 export SGE_INPUT_CFLAGS=\""$( echo \
		-Wno-address \
		-Wno-deprecated-declarations \
		-Wno-format \
		-Wno-unused \
		-I/usr/include/$DB4 \
		-I/usr/include/freetype2 \
	)"\"
	;;
*)
	eval2 export SGE_INPUT_LDFLAGS=-L/usr/lib64/$DB4
	eval2 export SGE_INPUT_CFLAGS=\""$( echo \
		-Wno-address \
		-Wno-deprecated-declarations \
		-Wno-format \
		-Wno-maybe-uninitialized \
		-Wno-unused \
		-I/usr/include/$DB4 \
		-I/usr/include/freetype2 \
	)"\"
esac
set -- -no-dump -no-java
eval2 ./aimk -only-depend "$@"
eval2 ./scripts/zerodepend
eval2 ./aimk "$@" depend
eval2 ./aimk "$@"
eval2 ./aimk -man "$@"

#
# Install software to package sandbox
#
[ -e "../$DESTDIR" ] || eval2 mkdir -p "../$DESTDIR"
yes | eval2 SGE_ROOT="\"$(pwd)/../$DESTDIR\"" ./scripts/distinst -local -allall
eval2 : SUCCESS

################################################################################
# END
################################################################################
