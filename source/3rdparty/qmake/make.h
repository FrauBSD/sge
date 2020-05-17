/* Miscellaneous global declarations and portability cruft for GNU Make.
Copyright (C) 1988,89,90,91,92,93,94,95,96,97,99 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* AIX requires this to be the first thing in the file.  */
#if defined (_AIX) && !defined (__GNUC__)
 #pragma alloca
#endif

/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because make.h was found in $srcdir).  */
#include <config.h>
#undef  HAVE_CONFIG_H
#define HAVE_CONFIG_H 1


/* Use prototypes if available.  */
#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
# undef  PARAMS
# define PARAMS(protos)  protos
#else /* Not C++ or ANSI C.  */
# undef  PARAMS
# define PARAMS(protos)  ()
#endif /* C++ or ANSI C.  */


/* For now, set gettext macro to a no-op.  */
#undef _
#undef N_
#define _(s)    s
#define N_(s)   s


#ifdef  CRAY
/* This must happen before #include <signal.h> so
   that the declaration therein is changed.  */
# define signal bsdsignal
#endif

/* If we're compiling for the dmalloc debugger, turn off string inlining.  */
#if defined(HAVE_DMALLOC_H) && defined(__GNUC__)
# define __NO_STRING_INLINES
#endif

#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_SYS_TIMEB_H
/* SCO 3.2 "devsys 4.2" has a prototype for `ftime' in <time.h> that bombs
   unless <sys/timeb.h> has been included first.  Does every system have a
   <sys/timeb.h>?  If any does not, configure should check for it.  */
# include <sys/timeb.h>
#endif
#include <time.h>
#include <errno.h>

#ifndef errno
extern int errno;
#endif

/* A shortcut for EINTR checking.  Note you should be careful when negating
   this!  That might not mean what you want if EINTR is not available.  */
#ifdef EINTR
# define EINTR_SET (errno == EINTR)
#else
# define EINTR_SET (0)
#endif

#ifndef isblank
# define isblank(c)     ((c) == ' ' || (c) == '\t')
#endif

#ifdef  HAVE_UNISTD_H
# include <unistd.h>
/* Ultrix's unistd.h always defines _POSIX_VERSION, but you only get
   POSIX.1 behavior with `cc -YPOSIX', which predefines POSIX itself!  */
# if defined (_POSIX_VERSION) && !defined (ultrix) && !defined (VMS)
#  define POSIX 1
# endif
#endif

/* Some systems define _POSIX_VERSION but are not really POSIX.1.  */
#if (defined (butterfly) || defined (__arm) || (defined (__mips) && defined (_SYSTYPE_SVR3)) || (defined (sequent) && defined (i386)))
# undef POSIX
#endif

#if !defined (POSIX) && defined (_AIX) && defined (_POSIX_SOURCE)
# define POSIX 1
#endif

#if defined (HAVE_SYS_SIGLIST) && !defined (SYS_SIGLIST_DECLARED)
extern char *sys_siglist[];
#endif

#if !defined (HAVE_SYS_SIGLIST) || !defined (HAVE_STRSIGNAL)
# include "signame.h"
#endif

/* Some systems do not define NSIG in <signal.h>.  */
#ifndef NSIG
# ifdef _NSIG
#  define NSIG  _NSIG
# else
#  define NSIG  32
# endif
#endif

#ifndef RETSIGTYPE
# define RETSIGTYPE     void
#endif

#ifndef sigmask
# define sigmask(sig)   (1 << ((sig) - 1))
#endif

#ifdef  HAVE_LIMITS_H
# include <limits.h>
#endif
#ifdef  HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#ifndef PATH_MAX
# ifndef POSIX
#  define PATH_MAX      MAXPATHLEN
# endif
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#ifdef  PATH_MAX
# define GET_PATH_MAX   PATH_MAX
# define PATH_VAR(var)  char var[PATH_MAX]
#else
# define NEED_GET_PATH_MAX 1
# define GET_PATH_MAX   (get_path_max ())
# define PATH_VAR(var)  char *var = (char *) alloca (GET_PATH_MAX)
extern unsigned int get_path_max PARAMS ((void));
#endif

#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

/* Nonzero if the integer type T is signed.  */
#define INTEGER_TYPE_SIGNED(t) ((t) -1 < 0)

/* The minimum and maximum values for the integer type T.
   Use ~ (t) 0, not -1, for portability to 1's complement hosts.  */
#define INTEGER_TYPE_MINIMUM(t) \
  (! INTEGER_TYPE_SIGNED (t) ? (t) 0 : ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1))
#define INTEGER_TYPE_MAXIMUM(t) (~ (t) 0 - INTEGER_TYPE_MINIMUM (t))

#ifndef CHAR_MAX
# define CHAR_MAX INTEGER_TYPE_MAXIMUM (char)
#endif

#ifdef STAT_MACROS_BROKEN
# ifdef S_ISREG
#  undef S_ISREG
# endif
# ifdef S_ISDIR
#  undef S_ISDIR
# endif
#endif  /* STAT_MACROS_BROKEN.  */

#ifndef S_ISREG
# define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
# define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifdef VMS
# include <stdio.h>
# include <types.h>
# include <unixlib.h>
# include <unixio.h>
# include <errno.h>
# include <perror.h>
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__
#  define __attribute__(x)
# endif
/* The __-protected variants of `format' and `printf' attributes
   are accepted by gcc versions 2.6.4 (effectively 2.7) and later.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#  define __format__ format
#  define __printf__ printf
# endif
#endif

#if defined (STDC_HEADERS) || defined (__GNU_LIBRARY__)
# include <stdlib.h>
# include <string.h>
# define ANSI_STRING 1
#else   /* No standard headers.  */
# ifdef HAVE_STRING_H
#  include <string.h>
#  define ANSI_STRING 1
# else
#  include <strings.h>
# endif
# ifdef HAVE_MEMORY_H
#  include <memory.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# else
extern char *malloc PARAMS ((int));
extern char *realloc PARAMS ((char *, int));
extern void free PARAMS ((char *));

extern void abort PARAMS ((void)) __attribute__ ((noreturn));
extern void exit PARAMS ((int)) __attribute__ ((noreturn));
# endif /* HAVE_STDLIB_H.  */

#endif /* Standard headers.  */

#if ST_MTIM_NSEC
# if HAVE_INTTYPES_H
#  include <inttypes.h>
# endif
# define FILE_TIMESTAMP uintmax_t
#else
# define FILE_TIMESTAMP time_t
#endif

#ifdef  ANSI_STRING

# ifndef index
#  define index(s, c)       strchr((s), (c))
# endif
# ifndef rindex
#  define rindex(s, c)      strrchr((s), (c))
# endif

# ifndef bcmp
#  define bcmp(s1, s2, n)   memcmp ((s1), (s2), (n))
# endif
# ifndef bzero
#  define bzero(s, n)       memset ((s), 0, (n))
# endif
# if defined(HAVE_MEMMOVE) && !defined(bcopy)
#  define bcopy(s, d, n)    memmove ((d), (s), (n))
# endif

#else   /* Not ANSI_STRING.  */

# ifndef bcmp
extern int bcmp PARAMS ((const char *, const char *, int));
# endif
# ifndef bzero
extern void bzero PARAMS ((char *, int));
#endif
# ifndef bcopy
extern void bcopy PARAMS ((const char *b1, char *b2, int));
# endif

#endif  /* ANSI_STRING.  */
#undef  ANSI_STRING

/* SCO Xenix has a buggy macro definition in <string.h>.  */
#undef  strerror

#if !defined(ANSI_STRING) && !defined(__DECC)
extern char *strerror PARAMS ((int errnum));
#endif

#ifdef __GNUC__
# undef alloca
# define alloca(n)      __builtin_alloca (n)
#else   /* Not GCC.  */
# ifdef HAVE_ALLOCA_H
#  include <alloca.h>
# else /* Not HAVE_ALLOCA_H.  */
#  ifndef _AIX
extern char *alloca ();
#  endif /* Not AIX.  */
# endif /* HAVE_ALLOCA_H.  */
#endif /* GCC.  */

/* ISDIGIT offers the following features:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
      NOTE!  Make relies on this behavior, don't change it!
   - It's typically faster.
   Posix 1003.2-1992 section 2.5.2.1 page 50 lines 1556-1558 says that
   only '0' through '9' are digits.  Prefer ISDIGIT to isdigit() unless
   it's important to use the locale's definition of `digit' even when the
   host does not conform to Posix.  */
#define ISDIGIT(c) ((unsigned) (c) - '0' <= 9)

#ifndef iAPX286
# define streq(a, b) \
   ((a) == (b) || \
    (*(a) == *(b) && (*(a) == '\0' || !strcmp ((a) + 1, (b) + 1))))
# ifdef HAVE_CASE_INSENSITIVE_FS
/* This is only used on Windows/DOS platforms, so we assume strcmpi().  */
#  define strieq(a, b) \
    ((a) == (b) || \
     (tolower(*(a)) == tolower(*(b)) && (*(a) == '\0' || !strcmpi ((a) + 1, (b) + 1))))
# else
#  define strieq(a, b) streq(a, b)
# endif
#else
/* Buggy compiler can't handle this.  */
# define streq(a, b) (strcmp ((a), (b)) == 0)
# define strieq(a, b) (strcmp ((a), (b)) == 0)
#endif
#define strneq(a, b, l) (strncmp ((a), (b), (l)) == 0)

/* Add to VAR the hashing value of C, one character in a name.  */
#define HASH(var, c) \
  ((var += (c)), (var = ((var) << 7) + ((var) >> 20)))
#ifdef HAVE_CASE_INSENSITIVE_FS /* Fold filenames */
# define HASHI(var, c) \
   ((var += tolower((c))), (var = ((var) << 7) + ((var) >> 20)))
#else
# define HASHI(var, c) HASH(var,c)
#endif

#if defined(__GNUC__) || defined(ENUM_BITFIELDS)
# define ENUM_BITFIELD(bits)    :bits
#else
# define ENUM_BITFIELD(bits)
#endif

#if defined(__MSDOS__) || defined(WINDOWS32)
# define PATH_SEPARATOR_CHAR ';'
#else
# if defined(VMS)
#  define PATH_SEPARATOR_CHAR ','
# else
#  define PATH_SEPARATOR_CHAR ':'
# endif
#endif

#ifdef WINDOWS32
# include <fcntl.h>
# include <malloc.h>
# define pipe(p) _pipe(p, 512, O_BINARY)
# define kill(pid,sig) w32_kill(pid,sig)

extern void sync_Path_environment(void);
extern int kill(int pid, int sig);
extern int safe_stat(char *file, struct stat *sb);
extern char *end_of_token_w32(char *s, char stopchar);
extern int find_and_set_default_shell(char *token);

/* indicates whether or not we have Bourne shell */
extern int no_default_sh_exe;

/* is default_shell unixy? */
extern int unixy_shell;
#endif  /* WINDOWS32 */

struct floc
  {
    char *filenm;
    unsigned long lineno;
  };
#define NILF ((struct floc *)0)


/* Fancy processing for variadic functions in both ANSI and pre-ANSI
   compilers.  */
#if defined __STDC__ && __STDC__
extern void message (int prefix, const char *fmt, ...)
                     __attribute__ ((__format__ (__printf__, 2, 3)));
extern void error (const struct floc *flocp, const char *fmt, ...)
                   __attribute__ ((__format__ (__printf__, 2, 3)));
extern void fatal (const struct floc *flocp, const char *fmt, ...)
                   __attribute__ ((noreturn, __format__ (__printf__, 2, 3)));
#else
extern void message ();
extern void error ();
extern void fatal ();
#endif

extern void die PARAMS ((int)) __attribute__ ((noreturn));
extern void log_working_directory PARAMS ((int));
extern void pfatal_with_name PARAMS ((char *)) __attribute__ ((noreturn));
extern void perror_with_name PARAMS ((char *, char *));
extern char *savestring PARAMS ((const char *, unsigned int));
extern char *concat PARAMS ((char *, char *, char *));
extern char *xmalloc PARAMS ((unsigned int));
extern char *xrealloc PARAMS ((char *, unsigned int));
extern char *xstrdup PARAMS ((const char *));
extern char *find_next_token PARAMS ((char **, unsigned int *));
extern char *next_token PARAMS ((char *));
extern char *end_of_token PARAMS ((char *));
extern void collapse_continuations PARAMS ((char *));
extern void remove_comments PARAMS((char *));
extern char *sindex PARAMS ((const char *, unsigned int, \
                             const char *, unsigned int));
extern char *lindex PARAMS ((const char *, const char *, int));
extern int alpha_compare PARAMS ((const void *, const void *));
extern void print_spaces PARAMS ((unsigned int));
extern char *find_char_unquote PARAMS ((char *, char *, int));
extern char *find_percent PARAMS ((char *));

#ifndef NO_ARCHIVES
extern int ar_name PARAMS ((char *));
extern void ar_parse_name PARAMS ((char *, char **, char **));
extern int ar_touch PARAMS ((char *));
extern time_t ar_member_date PARAMS ((char *));
#endif

extern int dir_file_exists_p PARAMS ((char *, char *));
extern int file_exists_p PARAMS ((char *));
extern int file_impossible_p PARAMS ((char *));
extern void file_impossible PARAMS ((char *));
extern char *dir_name PARAMS ((char *));

extern void define_default_variables PARAMS ((void));
extern void set_default_suffixes PARAMS ((void));
extern void install_default_suffix_rules PARAMS ((void));
extern void install_default_implicit_rules PARAMS ((void));

extern void build_vpath_lists PARAMS ((void));
extern void construct_vpath_list PARAMS ((char *pattern, char *dirpath));
extern int vpath_search PARAMS ((char **file, FILE_TIMESTAMP *mtime_ptr));
extern int gpath_search PARAMS ((char *file, int len));

extern void construct_include_path PARAMS ((char **arg_dirs));

extern void user_access PARAMS ((void));
extern void make_access PARAMS ((void));
extern void child_access PARAMS ((void));

#ifdef  HAVE_VFORK_H
# include <vfork.h>
#endif

/* We omit these declarations on non-POSIX systems which define _POSIX_VERSION,
   because such systems often declare them in header files anyway.  */

#if !defined (__GNU_LIBRARY__) && !defined (POSIX) && !defined (_POSIX_VERSION) && !defined(WINDOWS32)

extern long int atol ();
# ifndef VMS
extern long int lseek ();
# endif

#endif  /* Not GNU C library or POSIX.  */

#ifdef  HAVE_GETCWD
extern char *getcwd ();
# ifdef VMS
extern char *getwd PARAMS ((char *));
# endif
#else
extern char *getwd ();
# define getcwd(buf, len)       getwd (buf)
#endif

extern const struct floc *reading_file;

extern char **environ;

extern int just_print_flag, silent_flag, ignore_errors_flag, keep_going_flag;
extern int debug_flag, print_data_base_flag, question_flag, touch_flag;
extern int env_overrides, no_builtin_rules_flag, no_builtin_variables_flag;
extern int print_version_flag, print_directory_flag;
extern int warn_undefined_variables_flag, posix_pedantic;
extern int clock_skew_detected;

/* can we run commands via 'sh -c xxx' or must we use batch files? */
extern int batch_mode_shell;

extern unsigned int job_slots;
extern int job_fds[2];
extern int job_rfd;
#ifndef NO_FLOAT
extern double max_load_average;
#else
extern int max_load_average;
#endif

extern char *program;
extern char *starting_directory;
extern unsigned int makelevel;
extern char *version_string, *remote_description;

extern unsigned int commands_started;

extern int handling_fatal_signal;


#ifndef MIN
#define MIN(_a,_b) ((_a)<(_b)?(_a):(_b))
#endif
#ifndef MAX
#define MAX(_a,_b) ((_a)>(_b)?(_a):(_b))
#endif

#define DEBUGPR(msg) \
  do if (debug_flag) { print_spaces (depth); printf (msg, file->name); \
                       fflush (stdout); } while (0)

#ifdef VMS
# ifndef EXIT_FAILURE
#  define EXIT_FAILURE 3
# endif
# ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 1
# endif
# ifndef EXIT_TROUBLE
#  define EXIT_TROUBLE 2
# endif
#else
# ifndef EXIT_FAILURE
#  define EXIT_FAILURE 2
# endif
# ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
# endif
# ifndef EXIT_TROUBLE
#  define EXIT_TROUBLE 1
# endif
#endif

/* Set up heap debugging library dmalloc.  */

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif
