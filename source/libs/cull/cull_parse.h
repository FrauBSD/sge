#ifndef __CULL_PARSE_H
#define __CULL_PARSE_H
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


#ifdef  __cplusplus
extern "C" {
#endif

/* to make cull parsing reentrant */
typedef struct {
   int         token_is_valid;       /* state of cull token parser    */
   const char  *t;                   /* position of cull token parser */
   int         token;                /* token last recognized         */
} cull_parse_state;

void eat_token(cull_parse_state *state);
int scan(const char *s, cull_parse_state *state);


/* -------------- values returned by scan() --------------------- */

enum {
   NO_TOKEN = 0,

   TYPE,
   FIELD,

   SUBSCOPE,
   PLUS,
   MINUS,

   INT,
   STRING,
   ULONG,
   SUBLIST,
   FLOAT,
   DOUBLE,
   LONG,
   CHAR,
   BOOL,
   REF,

   CULL_ALL,
   CULL_NONE,

   EQUAL,
   NOT_EQUAL,
   LOWER_EQUAL,
   LOWER,
   GREATER_EQUAL,
   GREATER,
   BITMASK,
   STRCASECMP,
   PATTERNCMP,
   HOSTNAMECMP,

   AND,
   OR,
   NEG,
   BRA,
   KET
};

#ifdef  __cplusplus
}
#endif

#endif /* __CULL_PARSE_H */

