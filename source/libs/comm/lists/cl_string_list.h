#ifndef __CL_STRING_LIST_H
#define __CL_STRING_LIST_H

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
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2001 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 ************************************************************************/
/*___INFO__MARK_END__*/

#include "cl_lists.h"

typedef struct cl_string_list_elem_t {
   char*   string;     /* data */
   cl_raw_list_elem_t*   raw_elem;
} cl_string_list_elem_t;


/* basic functions */
int cl_string_list_setup(cl_raw_list_t** list_p, char* list_name );
int cl_string_list_cleanup(cl_raw_list_t** list_p);

/* thread list functions that will lock the list */
int cl_string_list_append_string(cl_raw_list_t* list_p, char* string, int lock_list);
int cl_string_list_remove_string(cl_raw_list_t* list_p, char* string, int lock_list);

/* thread functions that will not lock the list */
cl_string_list_elem_t* cl_string_list_get_first_elem(cl_raw_list_t* list_p);
cl_string_list_elem_t* cl_string_list_get_least_elem(cl_raw_list_t* list_p);
cl_string_list_elem_t* cl_string_list_get_next_elem(cl_string_list_elem_t* elem);
cl_string_list_elem_t* cl_string_list_get_last_elem(cl_string_list_elem_t* elem);


#endif /* __CL_STRING_LIST_H */

