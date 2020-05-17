/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define if the closedir function returns void instead of int.  */
/* #undef CLOSEDIR_VOID */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define for DGUX with <sys/dg_sys_info.h>.  */
/* #undef DGUX */

/* Define if the `getloadavg' function needs to be run setuid or setgid.  */
/* #undef GETLOADAVG_PRIVILEGED */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your system has a working fnmatch function.  */
/* #undef HAVE_FNMATCH */

/* Define if your system has its own `getloadavg' function.  */
#define HAVE_GETLOADAVG 1

/* Define if you have the getmntent function.  */
/* #undef HAVE_GETMNTENT */

/* Define if the `long double' type works.  */
/* #undef HAVE_LONG_DOUBLE */

/* Define if you support file names longer than 14 characters.  */
/* #undef HAVE_LONG_FILE_NAMES */

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if system calls automatically restart after interruption
   by a signal.  */
/* #undef HAVE_RESTARTABLE_SYSCALLS */

/* Define if your struct stat has st_blksize.  */
/* #undef HAVE_ST_BLKSIZE */

/* Define if your struct stat has st_blocks.  */
/* #undef HAVE_ST_BLOCKS */

/* Define if you have the strcoll function and it is properly defined.  */
#define HAVE_STRCOLL 1

/* Define if your struct stat has st_rdev.  */
/* #undef HAVE_ST_RDEV */

/* Define if you have the strftime function.  */
/* #undef HAVE_STRFTIME */

/* Define if you have the ANSI # stringizing operator in cpp. */
/* #undef HAVE_STRINGIZE */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if your struct tm has tm_zone.  */
/* #undef HAVE_TM_ZONE */

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
/* #undef HAVE_TZNAME */

/* Define if you have <unistd.h>.  */
#define HAVE_UNISTD_H 1

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the wait3 system call.  */
#define HAVE_WAIT3 1

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define if your struct nlist has an n_un member.  */
/* #undef NLIST_NAME_UNION */

/* Define if you have <nlist.h>.  */
/* #undef NLIST_STRUCT */

/* Define if your C compiler doesn't accept -c and -o together.  */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE int

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
/* #undef SETVBUF_REVERSED */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define on System V Release 4.  */
/* #undef SVR4 */

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define for Encore UMAX.  */
/* #undef UMAX */

/* Define for Encore UMAX 4.3 that has <inq_status/cpustats.h>
   instead of <sys/cpustats.h>.  */
/* #undef UMAX4_3 */

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define to the name of the SCCS `get' command.  */
#define SCCS_GET "get"

/* Define this if the SCCS `get' command understands the `-G<file>' option.  */
#define SCCS_GET_MINUS_G 1

/* Define this to enable job server support in GNU make.  */
#define MAKE_JOBSERVER 1

/* Define to be the nanoseconds member of struct stat's st_mtim,
   if it exists.  */
#define ST_MTIM_NSEC tv_nsec

/* Define this if the C library defines the variable `sys_siglist'.  */
/* #undef HAVE_SYS_SIGLIST */

/* Define this if the C library defines the variable `_sys_siglist'.  */
/* #undef HAVE__SYS_SIGLIST */

/* Define this if you have the `union wait' type in <sys/wait.h>.  */
/* #undef HAVE_UNION_WAIT */

/* Define to `unsigned long' or `unsigned long long'
   if <inttypes.h> doesn't define.  */
/* #undef uintmax_t */

/* Define if the system doesn't provide fd_set.  */
/* #undef fd_set */

/* Define the type of the first arg to select().  */
#define fd_set_size_t int

/* Define this if select() args need to be cast away from fd_set (HP-UX).  */
#define SELECT_FD_SET_CAST 

/* Define if you have the INTTYPES_H function.  */
#define HAVE_INTTYPES_H 1

/* Define if you have the clock_gettime function.  */
#define HAVE_CLOCK_GETTIME 1

/* Define if you have the dup2 function.  */
#define HAVE_DUP2 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getgroups function.  */
#define HAVE_GETGROUPS 1

/* Define if you have the gethostbyname function.  */
/* #undef HAVE_GETHOSTBYNAME */

/* Define if you have the gethostname function.  */
/* #undef HAVE_GETHOSTNAME */

/* Define if you have the getloadavg function.  */
#define HAVE_GETLOADAVG 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the mktemp function.  */
#define HAVE_MKTEMP 1

/* Define if you have the pipe function.  */
#define HAVE_PIPE 1

/* Define if you have the psignal function.  */
#define HAVE_PSIGNAL 1

/* Define if you have the pstat_getdynamic function.  */
/* #undef HAVE_PSTAT_GETDYNAMIC */

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the setegid function.  */
#define HAVE_SETEGID 1

/* Define if you have the seteuid function.  */
#define HAVE_SETEUID 1

/* Define if you have the setlinebuf function.  */
#define HAVE_SETLINEBUF 1

/* Define if you have the setregid function.  */
#define HAVE_SETREGID 1

/* Define if you have the setreuid function.  */
#define HAVE_SETREUID 1

/* Define if you have the sigaction function.  */
#define HAVE_SIGACTION 1

/* Define if you have the sigsetmask function.  */
/* #undef HAVE_SIGSETMASK */

/* Define if you have the socket function.  */
/* #undef HAVE_SOCKET */

/* Define if you have the strcasecmp function.  */
/* #undef HAVE_STRCASECMP */

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1

/* Define if you have the wait3 function.  */
#define HAVE_WAIT3 1

/* Define if you have the waitpid function.  */
#define HAVE_WAITPID 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <dmalloc.h> header file.  */
/* #undef HAVE_DMALLOC_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <mach/mach.h> header file.  */
/* #undef HAVE_MACH_MACH_H */

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/wait.h> header file.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the dgc library (-ldgc).  */
/* #undef HAVE_LIBDGC */

/* Define if you have the dmalloc library (-ldmalloc).  */
/* #undef HAVE_LIBDMALLOC */

/* Define if you have the kstat library (-lkstat).  */
#define HAVE_LIBKSTAT 1

/* Define if you have the posix4 library (-lposix4).  */
#define HAVE_LIBPOSIX4 1

/* Define if you have the sun library (-lsun).  */
/* #undef HAVE_LIBSUN */

/* Name of package */
#define PACKAGE "make"

/* Version number of package */
#define VERSION "3.78.1"

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define to make fseeko etc. visible, on some hosts. */
#define _LARGEFILE_SOURCE 1

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Build host information. */
#define MAKE_HOST "i386-pc-solaris2.10"

