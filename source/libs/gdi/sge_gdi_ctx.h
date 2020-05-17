#ifndef __SGE_GDI_CTX_H
#define __SGE_GDI_CTX_H
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

#include "sge_env.h"
#include "sge_prog.h"
#include "setup_path.h"
#include "sge_bootstrap.h"
#include "sge_error_class.h"
#include "commlib.h"

typedef struct sge_gdi_ctx_class_str sge_gdi_ctx_class_t; 

/*
** TODO: this maps single thread clients, replace it with a better solution
*/
#include "sge.h"
#include "sge_gdi.h"
#include "sge_conf.h"
#include "sge_gdi2.h"
#include "sge_profiling.h"
#include "sge_uidgid.h"
#include "sge_gdi_packet.h"

int 
sge_gdi2_setup(sge_gdi_ctx_class_t **context, u_long32 progid, u_long32 thread_id, lList **alpp);

int 
sge_setup2(sge_gdi_ctx_class_t **context, u_long32 progid, u_long32 thread_id,
           lList **alpp, bool is_qmaster_intern_client);

struct sge_gdi_ctx_class_str {
   void *sge_gdi_ctx_handle;

   bool (*sge_gdi_packet_execute)        (sge_gdi_ctx_class_t* ctx, lList **answer_list,
                                          sge_gdi_packet_class_t *packet);
   bool (*sge_gdi_packet_wait_for_result)(sge_gdi_ctx_class_t* ctx, lList **answer_list,
                                          sge_gdi_packet_class_t **packet_handle, lList **malpp);
   bool (*gdi_wait)                      (sge_gdi_ctx_class_t* ctx, lList **alpp, lList **malpp,
                                          state_gdi_multi *state);
   lList* (*gdi)                         (sge_gdi_ctx_class_t *thiz, u_long32 target, 
                                          u_long32 cmd, lList **lp, lCondition *where, 
                                          lEnumeration *what);
   int (*gdi_multi)                      (sge_gdi_ctx_class_t* ctx, lList **alpp, int mode, 
                                          u_long32 target, u_long32 cmd, lList **lp, 
                                          lCondition *cp, lEnumeration *enp, 
                                          state_gdi_multi *state, bool do_copy);
  
   sge_env_state_class_t* (*get_sge_env_state)(sge_gdi_ctx_class_t *thiz);
   sge_prog_state_class_t* (*get_sge_prog_state)(sge_gdi_ctx_class_t *thiz);
   sge_path_state_class_t* (*get_sge_path_state)(sge_gdi_ctx_class_t *thiz);
   sge_bootstrap_state_class_t* (*get_sge_bootstrap_state)(sge_gdi_ctx_class_t *thiz);
   int (*reresolve_qualified_hostname)(sge_gdi_ctx_class_t *thiz);
   void (*get_errors)(sge_gdi_ctx_class_t *thiz, lList **alpp, bool clear_errors);
   
   int (*prepare_enroll)(sge_gdi_ctx_class_t *thiz);
   int (*connect)(sge_gdi_ctx_class_t *thiz);
   int (*is_alive)(sge_gdi_ctx_class_t *thiz);
   lList* (*tsm)(sge_gdi_ctx_class_t *thiz, const char *schedd_name, const char *cell);
   lList* (*kill)(sge_gdi_ctx_class_t *thiz, lList *id_list, const char *cell, u_long32 option_flags, u_long32 action_flag);
   bool (*gdi_check_permission)(sge_gdi_ctx_class_t *thiz, lList **alpp, int option);
   bool (*gdi_get_mapping_name)(sge_gdi_ctx_class_t *thiz, const char *requestedHost, char *buf, int buflen);
   const char* (*get_master)(sge_gdi_ctx_class_t *thiz, bool reread);
   u_long32 (*get_sge_qmaster_port)(sge_gdi_ctx_class_t *thiz);
   u_long32 (*get_sge_execd_port)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_component_name)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_thread_name)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_progname)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_qualified_hostname)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_unqualified_hostname)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_default_cell)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_admin_user)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_binary_path)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_qmaster_spool_dir)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_bootstrap_file)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_act_qmaster_file)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_shadow_master_file)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_acct_file)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_reporting_file)(sge_gdi_ctx_class_t *thiz);
   u_long32 (*get_who)(sge_gdi_ctx_class_t *thiz);
   bool (*is_daemonized)(sge_gdi_ctx_class_t *thiz);
   void (*set_daemonized)(sge_gdi_ctx_class_t *thiz, bool daemonized);
   sge_exit_func_t (*get_exit_func)(sge_gdi_ctx_class_t *thiz);
   void (*set_exit_func)(sge_gdi_ctx_class_t *thiz, sge_exit_func_t exit_func);
   const char* (*get_cell_root)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_sge_root)(sge_gdi_ctx_class_t *thiz);
   bool (*get_job_spooling)(sge_gdi_ctx_class_t *thiz);
   void (*set_job_spooling)(sge_gdi_ctx_class_t *thiz, bool job_spooling);
   u_long32 (*get_listener_thread_count)(sge_gdi_ctx_class_t *thiz);
   u_long32 (*get_worker_thread_count)(sge_gdi_ctx_class_t *thiz);
   u_long32 (*get_scheduler_thread_count)(sge_gdi_ctx_class_t *thiz);
   u_long32 (*get_jvm_thread_count)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_spooling_method)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_spooling_lib)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_spooling_params)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_username)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_groupname)(sge_gdi_ctx_class_t *thiz);
   uid_t (*get_uid)(sge_gdi_ctx_class_t *thiz);
   gid_t (*get_gid)(sge_gdi_ctx_class_t *thiz);
   bool (*is_qmaster_internal_client)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_ca_root)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_ca_local_root)(sge_gdi_ctx_class_t *thiz);

   /* credentials */
   void (*set_private_key)(sge_gdi_ctx_class_t *thiz, const char* pkey);
   void (*set_certificate)(sge_gdi_ctx_class_t *thiz, const char* cert);
   const char* (*get_private_key)(sge_gdi_ctx_class_t *thiz);
   const char* (*get_certificate)(sge_gdi_ctx_class_t *thiz);

   /* commlib */
   cl_com_handle_t* (*get_com_handle)(sge_gdi_ctx_class_t *thiz);
   
   /* dump current settings */
   void (*dprintf)(sge_gdi_ctx_class_t *thiz);
};

sge_gdi_ctx_class_t *
sge_gdi_ctx_class_create(int prog_number, const char *component_name,
                         int thread_number, const char *thread_name,
                         const char* username, const char* groupname, 
                         const char* sge_root, const char* sge_cell, 
                         int sge_qmaster_port, int sge_execd_port,
                         bool from_services, bool is_qmaster_intern_client,
                         lList **alpp);



/*
   create an instance of sge_gdi_ctx_class_t and initialize it
   with the information of the bootstrap file

   @param prog_number  number of the program which uses the context
   @param url          url to the bootstrap file
   @param username     name of the user which uses the context
   @param alpp         answer list reference
   @return the gdi context

   TODO we need a answer list to transport error messages to the caller
*/
sge_gdi_ctx_class_t *
sge_gdi_ctx_class_create_from_bootstrap(int prog_number, const char* component_name,
                                        int thread_number, const char *thread_name,
                                        const char* url, const char* username, lList **alpp);

void sge_gdi_ctx_class_destroy(sge_gdi_ctx_class_t **pst);

sge_gdi_ctx_class_t* sge_gdi_get_thread_local_ctx(void);
void sge_gdi_set_thread_local_ctx(sge_gdi_ctx_class_t* ctx);

void gdi_mt_init(void);

#endif /* __SGE_GDI_CTX_H */


