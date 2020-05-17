#!/bin/ksh
#
# *********
# ATTENTION
# *********
# This file has been replaced by sge_mpirun. This file applies only to
# MPICH-GM versions prior to the 1.2.4..8a release.
#
#___INFO__MARK_BEGIN__
##########################################################################
#
#  The Contents of this file are made available subject to the terms of
#  the Sun Industry Standards Source License Version 1.2
#
#  Sun Microsystems Inc., March, 2001
#
#
#  Sun Industry Standards Source License Version 1.2
#  =================================================
#  The contents of this file are subject to the Sun Industry Standards
#  Source License Version 1.2 (the "License"); You may not use this file
#  except in compliance with the License. You may obtain a copy of the
#  License at http://gridengine.sunsource.net/Gridengine_SISSL_license.html
#
#  Software provided under this License is provided on an "AS IS" basis,
#  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#  See the License for the specific provisions governing your rights and
#  obligations concerning the Software.
#
#  The Initial Developer of the Original Code is: Shannon Davidson, Raytheon
#
#  Copyright: 2001 by Sun Microsystems, Inc.
#
#  All Rights Reserved.
#
##########################################################################
#___INFO__MARK_END__


set +u

if [ "$SGE_ROOT" = "" ]; then
   print -u2 SGE_ROOT is not set
   exit 1
fi
SGE_CELL=${SGE_CELL:-default}
. $SGE_ROOT/$SGE_CELL/common/settings.sh
SGE_CELL=${SGE_CELL:-default}

host=`hostname`

if [ "$PE" = "" ]; then
   print -u2 PE is not set, sge_mpirun must be issued from a Grid Engine parallel job
   exit 2
fi

if [ "$NSLOTS" = "" ]; then
   print -u2 NSLOTS is not set, sge_mpirun must be issued from a Grid Engine parallel job
   exit 3
fi

mpirun=$(qconf -sp $PE | grep "^start_proc_args" | awk '{ print $NF; }')

if [ ! -f $mpirun ]; then
   print -u2 $mpirun does not exist
   print -u2 There must be a problem with the $PE parallel environment
   exit 4
fi

if [ ! -f $TMPDIR/machines ]; then
   print -u2 $TMPDIR/machines does not exist
   print -u2 There must be a problem with the $PE parallel environment
   exit 5
fi

# Add TMPDIR to path in case there are wrappers
export PATH=$TMPDIR:$PATH

#print ===========DEBUG============
#print exec $mpirun --gm-f $TMPDIR/machines --gm-kill 15 -np $NSLOTS "$@"
#print ===========DEBUG============
exec $mpirun --gm-f $TMPDIR/machines --gm-kill 15 -np $NSLOTS "$@"
print -u2 exec of $mpirun failed
exit 6

