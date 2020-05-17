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
#include <stdio.h>
#include <unistd.h>
#include "drmaa.h"

int main (int argc, char **argv) {
   char error[DRMAA_ERROR_STRING_BUFFER];
   int errnum = 0;
   drmaa_job_template_t *jt = NULL;

   errnum = drmaa_init (NULL, error, DRMAA_ERROR_STRING_BUFFER);

   if (errnum != DRMAA_ERRNO_SUCCESS) {
      fprintf (stderr, "Could not initialize the DRMAA library: %s\n", error);
      return 1;
   }

   errnum = drmaa_allocate_job_template (&jt, error, DRMAA_ERROR_STRING_BUFFER);

   if (errnum != DRMAA_ERRNO_SUCCESS) {
      fprintf (stderr, "Could not create job template: %s\n", error);
   }
   else {
      errnum = drmaa_set_attribute (jt, DRMAA_REMOTE_COMMAND, "sleeper.sh",
                                    error, DRMAA_ERROR_STRING_BUFFER);

      if (errnum != DRMAA_ERRNO_SUCCESS) {
         fprintf (stderr, "Could not set attribute \"%s\": %s\n",
                  DRMAA_REMOTE_COMMAND, error);
      }
      else {
         const char *args[2] = {"60", NULL};
         
         errnum = drmaa_set_vector_attribute (jt, DRMAA_V_ARGV, args, error,
                                              DRMAA_ERROR_STRING_BUFFER);
      }
      
      if (errnum != DRMAA_ERRNO_SUCCESS) {
         fprintf (stderr, "Could not set attribute \"%s\": %s\n",
                  DRMAA_REMOTE_COMMAND, error);
      }
      else {
         char jobid[DRMAA_JOBNAME_BUFFER];

         errnum = drmaa_run_job (jobid, DRMAA_JOBNAME_BUFFER, jt, error,
                                 DRMAA_ERROR_STRING_BUFFER);

         if (errnum != DRMAA_ERRNO_SUCCESS) {
            fprintf (stderr, "Could not submit job: %s\n", error);
         }
         else {
            int status = 0;
            
            printf ("Your job has been submitted with id %s\n", jobid);
            
            sleep (20);
            
            errnum = drmaa_job_ps (jobid, &status, error,
                                   DRMAA_ERROR_STRING_BUFFER);
            
            if (errnum != DRMAA_ERRNO_SUCCESS) {
               fprintf (stderr, "Could not get job' status: %s\n", error);
            }
            else {
               switch (status) {
                  case DRMAA_PS_UNDETERMINED:
                     printf ("Job status cannot be determined\n");
                     break;
                  case DRMAA_PS_QUEUED_ACTIVE:
                     printf ("Job is queued and active\n");
                     break;
                  case DRMAA_PS_SYSTEM_ON_HOLD:
                     printf ("Job is queued and in system hold\n");
                     break;
                  case DRMAA_PS_USER_ON_HOLD:
                     printf ("Job is queued and in user hold\n");
                     break;
                  case DRMAA_PS_USER_SYSTEM_ON_HOLD:
                     printf ("Job is queued and in user and system hold\n");
                     break;
                  case DRMAA_PS_RUNNING:
                     printf ("Job is running\n");
                     break;
                  case DRMAA_PS_SYSTEM_SUSPENDED:
                     printf ("Job is system suspended\n");
                     break;
                  case DRMAA_PS_USER_SUSPENDED:
                     printf ("Job is user suspended\n");
                     break;
                  case DRMAA_PS_USER_SYSTEM_SUSPENDED:
                     printf ("Job is user and system suspended\n");
                     break;
                  case DRMAA_PS_DONE:
                     printf ("Job finished normally\n");
                     break;
                  case DRMAA_PS_FAILED:
                     printf ("Job finished, but failed\n");
                     break;
               } /* switch */
            } /* else */
         } /* else */
      } /* else */

      errnum = drmaa_delete_job_template (jt, error, DRMAA_ERROR_STRING_BUFFER);

      if (errnum != DRMAA_ERRNO_SUCCESS) {
         fprintf (stderr, "Could not delete job template: %s\n", error);
      }
   } /* else */

   errnum = drmaa_exit (error, DRMAA_ERROR_STRING_BUFFER);

   if (errnum != DRMAA_ERRNO_SUCCESS) {
      fprintf (stderr, "Could not shut down the DRMAA library: %s\n", error);
      return 1;
   }

   return 0;
}
