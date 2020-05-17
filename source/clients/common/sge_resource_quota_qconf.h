#ifndef __SGE_RESOURCE_QUOTA_QCONF
#define __SGE_RESOURCE_QUOTA_QCONF

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

#include "cull.h"
#include "gdi/sge_gdi_ctx.h"

bool rqs_show(sge_gdi_ctx_class_t *ctx, lList **answer_list, const char *name);

bool rqs_get_via_gdi(sge_gdi_ctx_class_t *ctx, lList **answer_list, const lList *rqsref_list,
                            lList **rqs_list);

bool rqs_get_all_via_gdi(sge_gdi_ctx_class_t *ctx, lList **answer_list, lList **rqs_list);
bool rqs_add(sge_gdi_ctx_class_t *ctx, lList **answer_list, const char *name);
bool rqs_modify(sge_gdi_ctx_class_t *ctx, lList **answer_list, const char *name);
bool rqs_add_from_file(sge_gdi_ctx_class_t *ctx, lList **answer_list, const char *filename);
bool rqs_modify_from_file(sge_gdi_ctx_class_t *ctx, lList **answer_list, const char *filename, const char *name);

bool rqs_add_del_mod_via_gdi(sge_gdi_ctx_class_t *ctx, lList *rqs_list, lList **answer_list,
                                        u_long32 gdi_command);

#endif /* __SGE_RESOURCE_QUOTA_QCONF */

