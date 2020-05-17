#!/bin/csh -f
# 
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
#  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#  Copyright: 2001 by Sun Microsystems, Inc.
#
#  All Rights Reserved.
#
##########################################################################
#___INFO__MARK_END__

# 
# sample pvm job
# this script starts the pvm sample spmd
# note that this job uses the pvm group communication
# 
# our name 
#$ -N PVM_Job
# pe request
#$ -pe pvm 16,8,4-1
#$ -v SGE_QMASTER_PORT,DISPLAY
#$ -S /bin/csh
# ---------------------------

echo "Got $NSLOTS slots."

/bin/echo Here I am on a $ARC called `hostname`.

# spmd requests $NSLOTS on __different__ hosts
$SGE_ROOT/pvm/bin/$ARC/spmd $NSLOTS
