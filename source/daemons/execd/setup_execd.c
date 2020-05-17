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
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "sge_bootstrap.h"

#include "sgermon.h"
#include "sge.h"
#include "sge_conf.h"
#include "sge_log.h"
#include "sge_ja_task.h"
#include "sge_pe_task.h"
#include "sge_str.h"
#include "job_report_execd.h"
#include "execd_ck_to_do.h"
#include "setup_execd.h"
#include "sge_load_sensor.h"
#include "cull_file.h"
#include "sge_prog.h"
#include "sge_string.h"
#include "reaper_execd.h"
#include "execution_states.h"
#include "sge_feature.h"
#include "spool/classic/read_write_job.h"
#include "sge_unistd.h"
#include "sge_uidgid.h"
#include "sge_io.h"
#include "sge_os.h"
#include "sge_job.h"
#include "uti/sge_binding_hlp.h"
#include "sge_binding.h"

#include "msg_common.h"
#include "msg_daemons_common.h"
#include "msg_execd.h"

extern char execd_spool_dir[SGE_PATH_MAX];
extern lList *jr_list;

static char execd_messages_file[SGE_PATH_MAX];

/*-------------------------------------------------------------------*/
void sge_setup_sge_execd(sge_gdi_ctx_class_t *ctx, const char* tmp_err_file_name)
{
   char err_str[1024];
   int allowed_get_conf_errors     = 5;
   char* spool_dir = NULL;
   const char *unqualified_hostname = ctx->get_unqualified_hostname(ctx);
   const char *admin_user = ctx->get_admin_user(ctx);

   DENTER(TOP_LAYER, "sge_setup_sge_execd");

   /* TODO: is this the right place to switch the user ?
            ports below 1024 ok */
   /*
   ** switch to admin user
   */
   if (sge_set_admin_username(admin_user, err_str)) {
      CRITICAL((SGE_EVENT, err_str));
      /* TODO: remove */
      SGE_EXIT(NULL, 1);
   }

   if (sge_switch2admin_user()) {
      CRITICAL((SGE_EVENT, MSG_ERROR_CANTSWITCHTOADMINUSER));
      /* TODO: remove */
      SGE_EXIT(NULL, 1);
   }

   while (gdi2_wait_for_conf(ctx, &Execd_Config_List)) {
      if (allowed_get_conf_errors-- <= 0) {
         CRITICAL((SGE_EVENT, MSG_EXECD_CANT_GET_CONFIGURATION_EXIT));
         /* TODO: remove */
         SGE_EXIT(NULL, 1);
      }
      sleep(1);
      ctx->get_master(ctx, true);
   }
   sge_show_conf();         


   /* get aliased hostname */
   /* TODO: is this call needed ? */
   ctx->reresolve_qualified_hostname(ctx);
   spool_dir = mconf_get_execd_spool_dir();

   DPRINTF(("chdir(\"/\")----------------------------\n"));
   sge_chdir_exit("/",1);
   DPRINTF(("Making directories----------------------------\n"));
   sge_mkdir(spool_dir, 0755, 1, 0);
   DPRINTF(("chdir(\"%s\")----------------------------\n", spool_dir));
   sge_chdir_exit(spool_dir,1);
   sge_mkdir(unqualified_hostname, 0755, 1, 0);
   DPRINTF(("chdir(\"%s\",me.unqualified_hostname)--------------------------\n",
            unqualified_hostname));
   sge_chdir_exit(unqualified_hostname, 1); 
   /* having passed the  previous statement we may 
      log messages into the ERR_FILE  */
   if ( tmp_err_file_name != NULL) {
      sge_copy_append((char*)tmp_err_file_name, ERR_FILE, SGE_MODE_APPEND);
   }
   sge_switch2start_user();
   if ( tmp_err_file_name != NULL) {
      unlink(tmp_err_file_name);
   }
   sge_switch2admin_user();
   log_state_set_log_as_admin_user(1);
   sprintf(execd_messages_file, "%s/%s/%s", spool_dir, 
           unqualified_hostname, ERR_FILE);
   log_state_set_log_file(execd_messages_file);

   sprintf(execd_spool_dir, "%s/%s", spool_dir, 
           unqualified_hostname);
   
   DPRINTF(("Making directories----------------------------\n"));
   sge_mkdir(EXEC_DIR, 0775, 1, 0);
   sge_mkdir(JOB_DIR, 0775, 1, 0);
   sge_mkdir(ACTIVE_DIR,  0775, 1, 0);

#if defined(PLPA_LINUX) || defined(SOLARIS86) || defined(SOLARISAMD64)
   /* initialize processor topology */
   if (initialize_topology() != true) {
      DPRINTF(("Couldn't initizalize topology-----------------------\n"));
   }
#endif

   FREE(spool_dir);
   DRETURN_VOID;
}

int job_initialize_job(lListElem *job)
{
   u_long32 job_id;
   lListElem *ja_task;
   lListElem *pe_task;
   DENTER(TOP_LAYER, "job_initialize_job");

   job_id = lGetUlong(job, JB_job_number); 
   for_each (ja_task, lGetList(job, JB_ja_tasks)) {
      u_long32 ja_task_id;

      ja_task_id = lGetUlong(ja_task, JAT_task_number);

      add_job_report(job_id, ja_task_id, NULL, job);
                                                                                      /* add also job reports for tasks */
      for_each (pe_task, lGetList(ja_task, JAT_task_list)) {
         add_job_report(job_id, ja_task_id, lGetString(pe_task, PET_id), job);
      }

      if (mconf_get_simulate_jobs()) {
         /* nothing to do for simulated jobs */
         continue;
      }

      /* does active dir exist ? */
      if (lGetUlong(ja_task, JAT_status) == JRUNNING ||
          lGetUlong(ja_task, JAT_status) == JWAITING4OSJID) {
         SGE_STRUCT_STAT stat_buffer;
         stringT active_dir = "";

         sge_get_file_path(active_dir, JOB_ACTIVE_DIR, FORMAT_DEFAULT,
                           SPOOL_WITHIN_EXECD, job_id, ja_task_id, NULL);
         if (SGE_STAT(active_dir, &stat_buffer)) {
            /* lost active directory - initiate cleanup for job */
            execd_job_run_failure(job, ja_task, NULL, "lost active dir of running "
                                  "job", GFSTATE_HOST);
            continue;
         }
      }

#ifdef COMPILE_DC
      {
         int ret;
         /* register still running jobs at ptf */
         if (lGetUlong(ja_task, JAT_status) == JRUNNING) {
            ret = register_at_ptf(job, ja_task, NULL);
            if (ret) {
               ERROR((SGE_EVENT, MSG_JOB_XREGISTERINGJOBYATPTFDURINGSTARTUP_SU,
                     (ret == 1 ? MSG_DELAYED : MSG_FAILED), sge_u32c(job_id)));
            }
         }
         for_each(pe_task, lGetList(ja_task, JAT_task_list)) {
            if (lGetUlong(pe_task, PET_status) == JRUNNING) {
               ret=register_at_ptf(job, ja_task, pe_task);
               if (ret) {
                  ERROR((SGE_EVENT, MSG_JOB_XREGISTERINGJOBYTASKZATPTFDURINGSTARTUP_SUS,
                     (ret == 1 ? MSG_DELAYED : MSG_FAILED),
                     sge_u32c(job_id), lGetString(pe_task, PET_id)));
               }
            }
         }
      }
#endif
   }     
   DEXIT;
   return 0;           
}
