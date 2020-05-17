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

int
sge_init_shared_ssl_lib(void);

int
sge_done_shared_ssl_lib(void);

void 
buffer_encrypt(const char *buffer_in, size_t buffer_in_length, 
               char **buffer_out, size_t *buffer_out_size, 
               size_t *buffer_out_length);
int 
buffer_decrypt(const char *buffer_in, size_t buffer_in_length,
               char **buffer_out, size_t *buffer_out_size,
               size_t *buffer_out_length, char *err_str);

int
password_find_entry(char *users[], char *encryped_pwds[], const char *user);

unsigned char *
buffer_encode_hex(unsigned char *input, size_t len, unsigned char **output);

unsigned char *
buffer_decode_hex(unsigned char *buf, size_t *len, unsigned char **outbuf);

