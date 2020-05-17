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
#if !defined(COMPILE_DC)

int verydummyprocfs;

#else

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/signal.h>

#if !defined(CRAY) && !defined(NECSX4) && !defined(NECSX5) && !defined(AIX)
#include <sys/syscall.h>
#endif

#include <unistd.h>
#include <sys/times.h>
#include <sys/wait.h>
#if defined(FREEBSD) || defined(DARWIN)
#include <sys/time.h>
#endif
#include <sys/resource.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#if defined(ALPHA) 
#  include <sys/user.h>
#  include <sys/table.h>   
#  include <sys/procfs.h>   
#endif

#if defined(SOLARIS) 
#  include <sys/procfs.h>   
#endif

#if defined(LINUX)
#include <sys/param.h>          /* for HZ (jiffies -> seconds ) */
#include "sge_proc.h"
#endif

#include "uti/sge_stdio.h"
#include "uti/sge_unistd.h"

#include "sge_log.h"
#include "msg_sge.h"
#include "sgermon.h"
#include "basis_types.h"
#include "sgedefs.h"
#include "exec_ifm.h"
#include "pdc.h"
#include "cull.h"

#if !defined(CRAY)
#include "procfs.h"
#endif

#if defined(LINUX) || defined(ALPHA) || defined(SOLARIS)
#   if defined(SVR3)
#      define PROC_DIR "/debug"
#   else
#      define PROC_DIR "/proc"
#   endif
#endif

#if defined(LINUX)

#define BIGLINE 1024

#endif


/*-----------------------------------------------------------------------*/
#if defined(LINUX) || defined(ALPHA) || defined(SOLARIS)

static DIR *cwd;
static struct dirent *dent;

#if defined(LINUX)

int groups_in_proc (void) 
{
   char buf[1024];
   FILE* fd = (FILE*) NULL;
   
   if (!(fd = fopen(PROC_DIR "/self/status", "r"))) {
      return 0;
   }
   while (fgets(buf, sizeof(buf), fd)) {
      if (strcmp("Groups:", strtok(buf, "\t"))==0) {
         FCLOSE(fd);
         return 1;
      }
   }
   FCLOSE(fd);
   return 0;
FCLOSE_ERROR:
   return 0;
}

#endif


/* search in job list for the pid
   return the proc element */
static lnk_link_t *find_pid_in_jobs(pid_t pid, lnk_link_t *job_list)
{
   lnk_link_t *job, *proc = NULL;
   proc_elem_t *proc_elem = NULL;
   job_elem_t *job_elem = NULL;

   /*
    * try to find a matching job
    */
   for (job=job_list->next; job != job_list; job=job->next) {

      job_elem = LNK_DATA(job, job_elem_t, link);

      /*
       * try to find process in this jobs' proc list
       */

      for (proc=job_elem->procs.next; proc != &job_elem->procs;
               proc=proc->next) {

         proc_elem = LNK_DATA(proc, proc_elem_t, link);
         if (proc_elem->proc.pd_pid == pid)
            break; /* found it */
      }

      if (proc == &job_elem->procs) {
         /* end of procs list - no process found - try next job */
         proc = NULL;
      } else
         /* found a process */
         break;
   }

   return proc;
}


static void touch_time_stamp(const char *d_name, int time_stamp, lnk_link_t *job_list)
{
   pid_t pid;
   proc_elem_t *proc_elem;
   lnk_link_t *proc;

   DENTER(TOP_LAYER, "touch_time_stamp");

   sscanf(d_name, pid_t_fmt, &pid);
   if ((proc = find_pid_in_jobs(pid, job_list))) {
      proc_elem = LNK_DATA(proc, proc_elem_t, link);
      proc_elem->proc.pd_tstamp = time_stamp;
#ifdef MONITOR_PDC
      INFO((SGE_EVENT, "found job to process %s: set time stamp\n", d_name));
#endif
   }
#ifdef MONITOR_PDC
   else
      INFO((SGE_EVENT, "found no job to process %s\n", d_name));
#endif

   DEXIT;
   return;
}

void procfs_kill_addgrpid(gid_t add_grp_id, int sig,
   tShepherd_trace shepherd_trace)
{
   char procnam[128];
   int i;
   int groups=0;
   u_long32 max_groups;
   gid_t *list;
#if defined(SOLARIS) || defined(ALPHA)
   int fd;
   prcred_t proc_cred;
#elif defined(LINUX)
   FILE *fp;
   char buffer[1024];
   uid_t uids[4];
   gid_t gids[4];
#endif

   DENTER(TOP_LAYER, "procfs_kill_addgrpid");

   /* quick return in case of invalid add. group id */
   if (add_grp_id == 0) {
      DEXIT;
      return;
   }

   max_groups = sge_sysconf(SGE_SYSCONF_NGROUPS_MAX);
   if (max_groups <= 0)
      if (shepherd_trace) {
         char err_str[256];

         sprintf(err_str, MSG_SGE_NGROUPS_MAXOSRECONFIGURATIONNECESSARY );
         shepherd_trace(err_str);
      }
/*
 * INSURE detects a WRITE_OVERFLOW when getgroups was invoked (LINUX).
 * Is this a bug in the kernel or in INSURE?
 */
#if defined(LINUX)
   list = (gid_t*) malloc(2*max_groups*sizeof(gid_t));
#else
   list = (gid_t*) malloc(max_groups*sizeof(gid_t));
#endif
   if (list == NULL)
      if (shepherd_trace) {
         char err_str[256];

         sprintf(err_str, MSG_SGE_PROCFSKILLADDGRPIDMALLOCFAILED );
         shepherd_trace(err_str);
      }

   pt_open();

   /* find next valid entry in procfs  */
   while ((dent = readdir(cwd))) {
      if (!dent->d_name)
         continue;
      if (!dent->d_name[0])
         continue;

      if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, "."))
         continue;

      if (atoi(dent->d_name) == 0)
         continue;

#if defined(SOLARIS) || defined(ALPHA)
      sprintf(procnam, "%s/%s", PROC_DIR, dent->d_name);
      if ((fd = open(procnam, O_RDONLY, 0)) == -1) {
         DPRINTF(("open(%s) failed: %s\n", procnam, strerror(errno)));
         continue;
      }
#elif defined(LINUX)
      if (!strcmp(dent->d_name, "self"))
         continue;

      sprintf(procnam, PROC_DIR "/%s/status", dent->d_name);
      if (!(fp = fopen(procnam, "r")))
         continue;
#endif

#if defined(SOLARIS) || defined(ALPHA)
      /* get number of groups */
      if (ioctl(fd, PIOCCRED, &proc_cred) == -1) {
         close(fd);
         continue;
      }

      /* get list of supplementary groups */
      groups = proc_cred.pr_ngroups;
      if (ioctl(fd, PIOCGROUPS, list) == -1) {
         close(fd);
         continue;
      }

#elif defined(LINUX)
      /* get number of groups and current uids, gids
       * uids[0], gids[0] => UID and GID
       * uids[1], gids[1] => EUID and EGID
       * uids[2], gids[2] => SUID and SGID
       * uids[3], gids[3] => FSUID and FSGID
       */
      groups = 0;
      while (fgets(buffer, sizeof(buffer), fp)) {
         char *label = NULL;
         char *token = NULL;

         label = strtok(buffer, " \t\n");
         if (label) {
            if (!strcmp("Groups:", label)) {
               while ((token = strtok((char*) NULL, " \t\n"))) {
                  list[groups]=(gid_t) atol(token);
                  groups++;
               }
            } else if (!strcmp("Uid:", label)) {
               int i = 0;

               while ((i < 4) && (token = strtok((char*) NULL, " \t\n"))) {
                  uids[i]=(uid_t) atol(token);
                  i++;
               }
            } else if (!strcmp("Gid:", label)) {
               int i = 0;

               while ((i < 4) && (token = strtok((char*) NULL, " \t\n"))) {
                  gids[i]=(gid_t) atol(token);
                  i++;
               }
            }
         }
      }
#endif

#if defined(SOLARIS) || defined(ALPHA)
      close(fd);
#elif defined(LINUX)
      FCLOSE(fp);
FCLOSE_ERROR:
#endif

      /* send each process a signal which belongs to add_grg_id */
      for (i = 0; i < groups; i++) {
         if (list[i] == add_grp_id) {
            pid_t pid;
            pid = (pid_t) atol(dent->d_name);

#if defined(LINUX)
            /* if UID, GID, EUID and EGID == 0
             *  don't kill the process!!! - it could be the rpc.nfs-deamon
             */
            if (!(uids[0] == 0 && gids[0] == 0 &&
                  uids[1] == 0 && gids[1] == 0)) {
#elif defined(SOLARIS) || defined(ALPHA)
            if (!(proc_cred.pr_ruid == 0 && proc_cred.pr_rgid == 0 &&
                  proc_cred.pr_euid == 0 && proc_cred.pr_egid == 0)) {
#endif

               if (shepherd_trace) {
                  char err_str[256];

                  sprintf(err_str, MSG_SGE_KILLINGPIDXY_UI , sge_u32c(pid), groups);
                  shepherd_trace(err_str);
               }

               kill(pid, sig);

            } else {
               if (shepherd_trace) {
                  char err_str[256];

                  sprintf(err_str, MSG_SGE_DONOTKILLROOTPROCESSXY_UI ,
                     sge_u32c(atol(dent->d_name)), groups);
                  shepherd_trace(err_str);
               }
            }

            break;
         }
      }
   }
   pt_close();
   free(list);
   DEXIT;
}

int pt_open(void)
{
   cwd = opendir(PROC_DIR);
   return !cwd;
}
void pt_close(void)
{
   closedir(cwd);
}

int pt_dispatch_proc_to_job(
lnk_link_t *job_list,
int time_stamp,
time_t last_time
) {
   char procnam[128];
   int fd = -1;
#if defined(LINUX)
   char buffer[BIGLINE];
   lListElem *pr = NULL;
   SGE_STRUCT_STAT fst;
   unsigned long utime, stime, vsize, pid;
   int pos_pid = lGetPosInDescr(PRO_Type, PRO_pid);
   int pos_utime = lGetPosInDescr(PRO_Type, PRO_utime);
   int pos_stime = lGetPosInDescr(PRO_Type, PRO_stime);
   int pos_vsize = lGetPosInDescr(PRO_Type, PRO_vsize);
   int pos_groups = lGetPosInDescr(PRO_Type, PRO_groups);
   int pos_rel = lGetPosInDescr(PRO_Type, PRO_rel);
   int pos_run = lGetPosInDescr(PRO_Type, PRO_run);
   int pos_io = lGetPosInDescr(PRO_Type, PRO_io);
   int pos_group = lGetPosInDescr(GR_Type, GR_group);
#else
   prstatus_t pr;
   prpsinfo_t pri;
#endif

#if defined(SOLARIS) || defined(ALPHA)   
   prcred_t proc_cred;
#endif

   int ret;
   u_long32 max_groups;
   gid_t *list;
   int groups=0;
   int pid_tmp;

   proc_elem_t *proc_elem = NULL;
   job_elem_t *job_elem = NULL;
   lnk_link_t *curr;
   double old_time = 0;
   uint64 old_vmem = 0;

   DENTER(TOP_LAYER, "pt_dispatch_proc_to_job");

   max_groups = sge_sysconf(SGE_SYSCONF_NGROUPS_MAX);
   if (max_groups <= 0) {
      ERROR((SGE_EVENT, MSG_SGE_NGROUPS_MAXOSRECONFIGURATIONNECESSARY));
      DEXIT;
      return 1;  
   }   

   list = (gid_t*) malloc(max_groups*sizeof(gid_t));
   if (list == NULL) {
      ERROR((SGE_EVENT, MSG_SGE_PTDISPATCHPROCTOJOBMALLOCFAILED));
      DEXIT;
      return 1;
   }

   /* find next valid entry in procfs */ 
   while ((dent = readdir(cwd))) {
      char *pidname;

      if (!dent->d_name)
         continue;
      if (!dent->d_name[0])
         continue;

      if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, "."))
         continue;

      if (dent->d_name[0] == '.')
          pidname = &dent->d_name[1];
      else
          pidname = dent->d_name;

      if (atoi(pidname) == 0)
         continue;

#if defined(LINUX)
      /* check only processes which belongs to a GE job */
      if ((pr = get_pr(atoi(pidname))) != NULL) {
         /* set process as still running */
         lSetPosBool(pr, pos_run, true);
         if (lGetPosBool(pr, pos_rel) != true) {
            continue;
         }
      }
      sprintf(procnam, PROC_DIR "/%s/stat", dent->d_name);

      if (SGE_STAT(procnam, &fst)) {
         if (errno != ENOENT) {
#ifdef MONITOR_PDC
            INFO((SGE_EVENT, "could not stat %s: %s\n", procnam, strerror(errno)));
#endif
            touch_time_stamp(dent->d_name, time_stamp, job_list);
         }
         continue;
      }
      /* TODO (SH): This does not work with Linux 2.6. I'm looking for a workaround.
       * If the stat file was not changed since our last parsing there is no need to do it again
       */
      /*if (pr == NULL || fst.st_mtime > last_time) {*/
      {
#else
         sprintf(procnam, "%s/%s", PROC_DIR, dent->d_name);
#endif
         if ((fd = open(procnam, O_RDONLY, 0)) == -1) {
            if (errno != ENOENT) {
#ifdef MONITOR_PDC
               if (errno == EACCES)
                  INFO((SGE_EVENT, "(uid:"gid_t_fmt" euid:"gid_t_fmt") could not open %s: %s\n",
                           getuid(), geteuid(), procnam, strerror(errno)));
               else
                  INFO((SGE_EVENT, "could not open %s: %s\n", procnam, strerror(errno)));
#endif
                  touch_time_stamp(dent->d_name, time_stamp, job_list);
            }
            continue;
         }

         /** 
          ** get a list of supplementary group ids to decide
          ** whether this process will be needed;
          ** read also prstatus
          **/

#  if defined(LINUX)

         /* 
          * Read the line and append a 0-Byte 
          */
         if ((ret = read(fd, buffer, BIGLINE-1))<=0) {
            close(fd);
            if (ret == -1 && errno != ENOENT) {
#ifdef MONITOR_PDC
               INFO((SGE_EVENT, "could not read %s: %s\n", procnam, strerror(errno)));
#endif
               touch_time_stamp(dent->d_name, time_stamp, job_list);
            }
            continue;
         }
         buffer[BIGLINE-1] = '\0';

         /* 
          * get prstatus
          */
         ret = sscanf(buffer, "%lu %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*u %lu",
                      &pid,
                      &utime,
                      &stime,
                      &vsize);

         if (ret != 4) {
            close(fd);
            continue;
         }

         if (pr == NULL) {
            pr = lCreateElem(PRO_Type);
            lSetPosUlong(pr, pos_pid, pid);
            lSetPosBool(pr, pos_rel, false);
            append_pr(pr);
         }
         
         lSetPosUlong(pr, pos_utime, utime);
         lSetPosUlong(pr, pos_stime, stime);

#ifdef TARGET_64BIT
         lSetPosLong(pr, pos_vsize, vsize);
#else
         lSetPosUlong(pr, pos_vsize, vsize);
#endif
         
         close(fd);
      }
      /* mark this proc as running */
      lSetPosBool(pr, pos_run, true);

      /* 
       * get number of groups; 
       * get list of supplementary groups 
       */
      {
         char procnam[256];
         lList *groupTable = lGetPosList(pr, pos_groups);

         sprintf(procnam, PROC_DIR "/%s/status", dent->d_name);
         if (SGE_STAT(procnam, &fst) != 0) {
            if (errno != ENOENT) {
#ifdef MONITOR_PDC
               INFO((SGE_EVENT, "could not stat %s: %s\n", procnam, strerror(errno)));
#endif
               touch_time_stamp(dent->d_name, time_stamp, job_list);
            }
            continue;
         }

         groups = 0;
         if (fst.st_mtime < last_time && groupTable != NULL) {
            lListElem *group;

            for_each(group, groupTable) {
               list[groups] = lGetPosUlong(group, pos_group);
               groups++;
            }
         } else {
            char buf[1024];
            FILE* f = (FILE*) NULL;

            if (!(f = fopen(procnam, "r"))) {
               continue;
            }
            /* save groups also in the table */
            groupTable = lCreateList("groupTable", GR_Type);
            while (fgets(buf, sizeof(buf), f)) {
               if (strcmp("Groups:", strtok(buf, "\t"))==0) {
                  char *token;
                  
                  while ((token=strtok((char*) NULL, " "))) {
                     lListElem *gr = lCreateElem(GR_Type);
                     long group = atol(token);
                     list[groups] = group;
                     lSetPosUlong(gr, pos_group, group);
                     lAppendElem(groupTable, gr);
                     groups++;
                  }
                  break;
               }
            }
            lSetPosList(pr, pos_groups, groupTable);
            fclose(f);
         }
      } 
#  elif defined(SOLARIS) || defined(ALPHA)
      
      /* 
       * get prstatus 
       */
      if (ioctl(fd, PIOCSTATUS, &pr)==-1) {
         close(fd);
         if (errno != ENOENT) {
#ifdef MONITOR_PDC
            INFO((SGE_EVENT, "could not ioctl(PIOCSTATUS) %s: %s\n", procnam, strerror(errno)));
#endif
            touch_time_stamp(dent->d_name, time_stamp, job_list);
         }
         continue;
      }
                                    
      /* 
       * get number of groups 
       */
      ret=ioctl(fd, PIOCCRED, &proc_cred);
      if (ret < 0) {
         close(fd);
         if (errno != ENOENT) {
#ifdef MONITOR_PDC
            INFO((SGE_EVENT, "could not ioctl(PIOCCRED) %s: %s\n", procnam, strerror(errno)));
#endif
            touch_time_stamp(dent->d_name, time_stamp, job_list);
         }
         continue;
      }
      
      /* 
       * get list of supplementary groups 
       */
      groups = proc_cred.pr_ngroups;
      ret=ioctl(fd, PIOCGROUPS, list);
      if (ret<0) {
         close(fd);
         if (errno != ENOENT) {
#ifdef MONITOR_PDC
            INFO((SGE_EVENT, "could not ioctl(PIOCCRED) %s: %s\n", procnam, strerror(errno)));
#endif
            touch_time_stamp(dent->d_name, time_stamp, job_list);
         }
         continue;
      }

#  endif

      /* 
       * try to find a matching job 
       */
      for (curr=job_list->next; curr != job_list; curr=curr->next) {
         int found_it = 0;
         int group;
         
         job_elem = LNK_DATA(curr, job_elem_t, link);
         for (group=0; !found_it && group<groups; group++) {
            if (job_elem->job.jd_jid == list[group]) {
#if defined(LINUX)
               /* mark this process as relevant */
               lSetPosBool(pr, pos_rel, true);
#endif
               found_it = 1;
            }
         }
         if (found_it)
            break;
      }

      if (curr == job_list) { /* this is not a traced process */ 
         close(fd);
         continue;
      }

      /* we always read only one entry per function call
         the while loop is needed to read next one */
      break;
   } /* while */

   free(list);

   if (!dent) {/* visited all files in procfs */
#if defined(LINUX)
      clean_procList();
#endif
      DEXIT;
      return 1;
   }
   /* 
    * try to find process in this jobs' proc list 
    */

#if defined(LINUX)
   pid_tmp = lGetPosUlong(pr, pos_pid);
#else
   pid_tmp = pr.pr_pid;
#endif
   for (curr=job_elem->procs.next; curr != &job_elem->procs; 
            curr=curr->next) {
      proc_elem = LNK_DATA(curr, proc_elem_t, link);
      
      if (proc_elem->proc.pd_pid == pid_tmp)
         break;
   }

   if (curr == &job_elem->procs) { 
      /* new process, add a proc element into jobs proc list */
      if (!(proc_elem=(proc_elem_t *)malloc(sizeof(proc_elem_t)))) {
         if (fd >= 0)
            close(fd);
         DEXIT;
         return 0;
      }
      memset(proc_elem, 0, sizeof(proc_elem_t));
      proc_elem->proc.pd_length = sizeof(psProc_t);
      proc_elem->proc.pd_state  = 1; /* active */
      LNK_ADD(job_elem->procs.prev, &proc_elem->link);
      job_elem->job.jd_proccount++;

#ifdef MONITOR_PDC
      {
         double utime, stime;
#if defined(LINUX)
         utime = ((double)lGetPosUlong(pr, pos_utime))/HZ;
         stime = ((double)lGetPosUlong(pr, pos_stime))/HZ;

         INFO((SGE_EVENT, "new process "sge_u32" for job "pid_t_fmt" (utime = %f stime = %f)\n", 
               lGetPosUlong(pr, pos_pid), job_elem->job.jd_jid, utime, stime)); 
#else
         utime = pr.pr_utime.tv_sec + pr.pr_utime.tv_nsec*1E-9;
         stime = pr.pr_stime.tv_sec + pr.pr_stime.tv_nsec*1E-9;

         INFO((SGE_EVENT, "new process "pid_t_fmt" for job "pid_t_fmt" (utime = %f stime = %f)\n", 
               pr.pr_pid, job_elem->job.jd_jid, utime, stime)); 
#endif
      }
#endif

   } else {
      /* save previous usage data - needed to build delta usage */
      old_time = proc_elem->proc.pd_utime + proc_elem->proc.pd_stime;
      old_vmem  = proc_elem->vmem;
   }

   proc_elem->proc.pd_tstamp = time_stamp;

#if defined(LINUX)
   proc_elem->proc.pd_pid = lGetPosUlong(pr, pos_pid);
   proc_elem->proc.pd_utime  = ((double)lGetPosUlong(pr, pos_utime))/HZ;
   proc_elem->proc.pd_stime  = ((double)lGetPosUlong(pr, pos_stime))/HZ;
   /* could retrieve uid/gid using stat() on stat file */

  #ifdef TARGET_64BIT
   proc_elem->vmem           = lGetPosLong(pr, pos_vsize);
  #else
   proc_elem->vmem           = lGetPosUlong(pr, pos_vsize);
  #endif

   /*
    * I/O accounting
    */
   proc_elem->delta_chars    = 0UL;
   {
      char procnam[256];
      uint64 new_iochars = 0UL;

      sprintf(procnam, PROC_DIR "/%s/io", dent->d_name);

      /* This io processing needs to be enabled in the kernel */
      if (SGE_STAT(procnam, &fst) == 0) {
         {
         /*if (fst.st_mtime > last_time) {*/
            FILE *fd;
            if ((fd = fopen(procnam, "r"))) {
               char buf[1024];
            
               /*
                * Trying to parse /proc/<pid>/io
                */

               while (fgets(buf, sizeof(buf), fd))
               {
                 char *label = strtok(buf, " \t\n");
                 char *token = strtok((char*) NULL, " \t\n");

                 if (label && token)
                   if (!strcmp("rchar:", label) ||
                       !strcmp("wchar:", label))
                   {
                      unsigned long long nchar = 0UL;
                      if (sscanf(token, "%Lu", &nchar) == 1)
                         new_iochars += (uint64) nchar;
                   }
               } /* while */

               fclose(fd);
               lSetPosUlong(pr, pos_io, new_iochars);
            }
         }
      } else {
         new_iochars = lGetPosUlong(pr, pos_io);
      }
      /*
       *  Update process I/O info
       */
      if (new_iochars > 0UL)
      {
         uint64 old_iochars = proc_elem->iochars;

         if (new_iochars > old_iochars)
         {
            proc_elem->delta_chars = (new_iochars - old_iochars);
            proc_elem->iochars = new_iochars;
         }
      }
   }
#else
   proc_elem->proc.pd_pid    = pr.pr_pid;
   proc_elem->proc.pd_utime  = pr.pr_utime.tv_sec + pr.pr_utime.tv_nsec*1E-9;
   proc_elem->proc.pd_stime  = pr.pr_stime.tv_sec + pr.pr_stime.tv_nsec*1E-9;
    
   /* Don't care if this part fails */
   if (ioctl(fd, PIOCPSINFO, &pri) != -1) {
      proc_elem->proc.pd_uid    = pri.pr_uid;
      proc_elem->proc.pd_gid    = pri.pr_gid;
      proc_elem->vmem           = pri.pr_size * pagesize;
      proc_elem->rss            = pri.pr_rssize * pagesize;
      proc_elem->proc.pd_pstart = pri.pr_start.tv_sec + pri.pr_start.tv_nsec*1E-9;
   }
#endif         

   proc_elem->mem = 
         ((proc_elem->proc.pd_stime + proc_elem->proc.pd_utime) - old_time) * 
         (( old_vmem + proc_elem->vmem)/2);

#if defined(ALPHA)
#define BLOCKSIZE 512
   {
      struct user ua;
      uint64 old_ru_ioblock = proc_elem->ru_ioblock;

      /* need to do a table(2) call for each process to retrieve io usage data */   
      /* get user area stuff */
      if (table(TBL_UAREA, proc_elem->proc.pd_pid, (char *)&ua, 1, sizeof ua) == 1) {
         proc_elem->ru_ioblock = (uint64)(ua.u_ru.ru_inblock + ua.u_ru.ru_oublock);
         proc_elem->delta_chars = (proc_elem->ru_ioblock - old_ru_ioblock)* BLOCKSIZE;
      }
   }
#endif


   close(fd);
   DEXIT;
   return 0;
}
#endif

#endif /* (!COMPILE_DC) */
