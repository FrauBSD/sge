#ifndef __SGE_NICE_H
#define __SGE_NICE_H
/*___INFO__MARK_BEGIN__*/
/*************************************************************************
 * 
 *  The Contents of this file are made available subject to the terms of
 *  the Sun Industry Standards Source License Version 1.2
 * 
 *  Sun Microsystems Inc., March, 2001
 * 
 * 
 *  Sun Industry Standards Source License Version 1.2
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.2 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://gridengine.sunsource.net/Gridengine_SISSL_license.html
 * 
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 * 
 *   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 * 
 *   Copyright: 2001 by Sun Microsystems, Inc.
 * 
 *   All Rights Reserved.
 * 
 ************************************************************************/
/*___INFO__MARK_END__*/

#ifdef SOLARIS
#   include <sys/resource.h>
#endif

#ifdef __convex__
#   define SETPRIORITY(niceval) setpriority(PRIO_PROCESS,getpgrp(),niceval)
#else
#   if defined(_UNICOS) || defined(SINIX)
#      define SETPRIORITY(niceval) nice(niceval + 20)
#   elif defined(INTERIX) 
       /* On Interix the nice range goes from 0 to 2*NZERO-1 */
#      define SETPRIORITY(niceval) setpriority(PRIO_PROCESS, 0, niceval + NZERO)
#   else
#      define SETPRIORITY(niceval) setpriority(PRIO_PROCESS, 0, niceval)
#   endif
#endif    

#endif /* __SGE_NICE_H */

