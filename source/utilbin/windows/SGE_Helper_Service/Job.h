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

enum en_request_type {
   req_error            =-1,
   req_job_start        = 0,
   req_send_job_usage   = 1,
   req_forward_signal   = 2
};

enum en_JobStatus {
   js_Received          = 0,
   js_ToBeStarted       = 1,
   js_Started           = 2,
   js_Finished          = 3,
   js_Failed            = 4,
   js_Deleted           = 5,
   js_Invalid           = -1
};

class C_Job {
   private:
      void FreeAllocatedMembers();
      void BuildSysEnvTable(CMapStringToString &mapSysEnv) const;
      void BuildEnvironmentFromTable(const CMapStringToString &mapMergedEnv, 
                                     char *&pszEnv) const;
      void MergeSysEnvTableWithJobEnvTable(CMapStringToString &mapSysEnv, 
                                           CMapStringToString &mapMergedEnv) const;
      void BuildTableFromJobEnv(CMapStringToString &mapJobEnv) const;
      BOOL IsEnvAPath(const CString &strKey) const;

      char* PathDelimiter(const CString &strPath) const;
 
   public:
      C_Job();
      C_Job(const C_Job& otherJob);
      ~C_Job();

      en_request_type ParseCommand(char *command);
      const char*     GetConfValue(const char *pszName) const;
      int             Serialize(HANDLE hFile) const;
      int             Unserialize(HANDLE hFile);

      void            BuildCommandLine(char *&szCmdLine) const;
      void            BuildEnvironment(char *&pszEnv) const;

      int             StoreUsage();
      int             Terminate();

      en_JobStatus m_JobStatus;
      DWORD        m_job_id;
      DWORD        m_ja_task_id;
      const char   *m_pe_task_id;
      SOCKET       m_comm_sock;
      HANDLE       m_hProcess;           // The main process of the job
      HANDLE       m_hJobObject;         // The Windows job object
      int          m_ForwardedSignal;

      // data members to start the job
      char    *jobname;
      int     nargs;
      char    **args;
      int     nconf;
      char    **conf;
      int     nenv;
      char    **env;
      char    *user;
      char    *pass;
      char    *domain;

      // data members that hold results and usage
      double  mem;
      double  cpu;
      double  vmem;

      DWORD dwExitCode;  // Exit code of job, if it ran
      char  *szError;    // Error message, in case of error
      long  lUserSec;    // User part of job run time
      long  lUserUSec;   // User part of job run time usec
      long  lKernelSec;  // Kernel part of job run time
      long  lKernelUSec; // Kernel part of job run time usec
};
