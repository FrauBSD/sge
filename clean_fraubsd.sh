#!/bin/sh
############################################################ IDENT(1)
#
# $Title: Script to clean SGE $
# $Copyright: 2019-2020 Devin Teske. All rights reserved. $
# $FrauBSD: sge/clean_fraubsd.sh 2020-05-17 11:49:53 -0700 freebsdfrau $
#
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
for item in \
	install \
	source/*/*/*/*_dependencies \
	source/*/*/*_dependencies \
	source/*/*_dependencies \
	source/3rdparty/*/*/LINUXAMD64/ \
	source/3rdparty/*/*/autom4te.cache/ \
	source/3rdparty/plpa/*/src/libplpa/plpa_config.h.in~ \
	source/3rdparty/qmake/*/*.o \
	source/3rdparty/qmake/*/*/*.a \
	source/3rdparty/qmake/*/*/*.o \
	source/3rdparty/qmake/*/make \
	source/3rdparty/qmon/LINUXAMD64/ \
	source/3rdparty/qtcsh/*/*.*.h \
	source/3rdparty/qtcsh/*/*.defs.* \
	source/3rdparty/qtcsh/*/*.o \
	source/3rdparty/qtcsh/*/gethost \
	source/3rdparty/qtcsh/*/tcsh \
	source/3rdparty/remote/LINUXAMD64/ \
	source/3rdparty/sge_depend/LINUXAMD64/ \
	source/LINUXAMD64/ \
	source/MANSBUILD_ge/ \
	source/dist/qmon/Qmon \
;do
        eval2 rm -Rf "$item"
done
git checkout source/3rdparty/*/*/configure
git checkout source/3rdparty/plpa/*/src/libplpa/plpa_config.h.in

################################################################################
# END
################################################################################
