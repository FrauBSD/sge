/* $XConsortium: def.h /main/30 1996/12/04 10:11:12 swick $ */
/*

Copyright (c) 1993, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef X_NOT_POSIX
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXDEFINES      512
#define MAXFILES        1024
#define MAXDIRS         64
#define SYMTABINC       10   /* must be > 1 for define() to work right */
#define TRUE            1
#define FALSE           0

/* the following must match the directives table in main.c */
#define IF              0
#define IFDEF           1
#define IFNDEF          2
#define ELSE            3
#define ENDIF           4
#define DEFINE          5
#define UNDEF           6
#define INCLUDE         7
#define LINE            8
#define PRAGMA          9
#define ERROR           10
#define IDENT           11
#define SCCS            12
#define ELIF            13
#define EJECT           14
#define IFFALSE         15     /* pseudo value --- never matched */
#define ELIFFALSE       16     /* pseudo value --- never matched */
#define INCLUDEDOT      17     /* pseudo value --- never matched */
#define IFGUESSFALSE    18     /* pseudo value --- never matched */
#define ELIFGUESSFALSE  19     /* pseudo value --- never matched */

#ifdef DEBUG
extern int   _debugmask;
/*
 * debug levels are:
 * 
 *     0   show ifn*(def)*,endif
 *     1   trace defined/!defined
 *     2   show #include
 *     3   show #include SYMBOL
 *     4-6   unused
 */
#define debug(level,arg) { if (_debugmask & (1 << level)) warning arg; }
#else
#define   debug(level,arg) /**/
#endif /* DEBUG */

/* typedef   unsigned char boolean; */
typedef   int boolean;
struct symtab {
   char   *s_name;
   char   *s_value;
};

/* possible i_flag */
#define DEFCHECKED      (1<<0)   /* whether defines have been checked */
#define NOTIFIED        (1<<1)   /* whether we have revealed includes */
#define MARKED          (1<<2)   /* whether it's in the makefile */
#define SEARCHED        (1<<3)   /* whether we have read this */
#define FINISHED        (1<<4)   /* whether we are done reading this */
#define INCLUDED_SYM    (1<<5)   /* whether #include SYMBOL was found
                                    Can't use i_list if TRUE */
struct   inclist {
   char              *i_incstring;     /* string from #include line */
   char              *i_file;          /* path name of the include file */
   struct inclist    **i_list;         /* list of files it itself includes */
   int               i_listlen;        /* length of i_list */
   struct symtab     **i_defs;         /* symbol table for this file and its
                                          children when merged */
   int               i_ndefs;          /* current # defines */
   boolean           *i_merged;        /* whether we have merged child
                                          defines */
   unsigned char     i_flags;
};

struct filepointer {
   char   *f_p;
   char   *f_base;
   char   *f_end;
   long   f_len;
   long   f_line;
};

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#if defined(macII) && !defined(__STDC__)  /* stdlib.h fails to define these */
char *malloc(), *realloc();
#endif /* macII */
#else
char         *malloc();
char         *realloc();
#endif

#if NeedFunctionPrototypes
char *copy( char *str);
char *base_name(char *file, int strip);
char *sge_getline(struct filepointer *filep);
struct filepointer *getfile(char *file);
struct inclist *newinclude(char *newfile, char *incstring);
struct inclist *inc_path(char	*file, char *include, boolean	dot);
void included_by(struct inclist *ip, struct inclist *newfile);
void inc_clean(void);
void define2(char *name, char *val, struct inclist *file);
void define(char *def, struct inclist *file);
int find_includes(struct filepointer *filep, struct inclist *file, 
                  struct inclist *file_red, int recursion, boolean failOK);
void recursive_pr_include(struct inclist *head, char *file, char *base);
void freefile(struct filepointer *fp);
int cppsetup(char	*line, struct filepointer *filep, struct inclist *inc);
struct symtab **isdefined( char *symbol,
                           struct inclist *file,
                           struct inclist **srcfile);
#else
char *copy();
char *base_name();
char *sge_getline();
struct filepointer *getfile();
struct inclist *newinclude();
struct inclist *inc_path();
void inc_clean();
void included_by();
void define2();
void define();
int find_includes();
void recursive_pr_include();
void freefile();
int cppsetup();
struct symtab **isdefined();
#endif

#if NeedVarargsPrototypes
extern void fatalerr(char *, ...);
extern void warning(char *, ...);
extern void warning1(char *, ...);
#else
void fatalerr();
void warning();
void warning1();
#endif


extern boolean local_include;
