#include "stdafx.h"
#include "WDogPxr.h"
#include "FifoInst.h"
#include <sys/wait.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <unistd.h>



#define RELAUNCH_INTERVAL 2000



CWDogPxr::CWDogPxr()
{
   m_nMonitorScanCntDwn = 1;
   m_nOutputLstCntDwn = 0;
   
   m_flpWatchItm.SetParent(this);
}


CWDogPxr::~CWDogPxr()
{
   Deactivate();
}


void CWDogPxr::Activate(char ** envp)
{
   SetEnvp(envp);
   KFileIO::EnsurePath("data/tmp");
   
   m_pxrMonitorScan.Activate(this, &CWDogPxr::_MonitorProcessesScan, 13000);
   m_pxrOutputLst.Activate(this, &CWDogPxr::_OutputLst, -1);
   SetCycleSleepTime(1000);
   
   int ret = prctl(PR_SET_CHILD_SUBREAPER, (u_long)getpid(), (u_long)0, (u_long)0);
   Log("prctl --> [%d]", ret);   
}


void CWDogPxr::Deactivate()
{
   StopWatching();
}


char ** CWDogPxr::GetEnvp() const
{
   return m_envp;
}


void CWDogPxr::SetEnvp(char ** envp)
{
   m_envp = envp;
}


void CWDogPxr::StartWatching(const KStr & strWatchListFile)
{
   Log("Start Watching [%s]", strWatchListFile.sz());

   Start();
   m_flpWatchItm.LoadFileOnly(strWatchListFile);
   m_flpWatchItm.ProcessFile(strWatchListFile, false);
   
   Log("Starting Output Lst pxr");
   m_pxrOutputLst.Start();
      
   Log("Starting monitor scanner");
   m_pxrMonitorScan.Start();
   
   Log("Started WatchDog");
}


void CWDogPxr::StopWatching()
{
   Log("Stopping WatchDog");
   PostStop();
   
   _UnblockWait();

   Stop();
   m_pxrMonitorScan.Stop();
   m_pxrOutputLst.Stop();
   Log("Stopped WatchDog");
}


void CWDogPxr::Process()
{
   _MonitorProcessesWait();
}


void CWDogPxr::ProcessItm(SWatchItm * pItm)
{
   if (!pItm->lock.Lock(0))            return;
   KLock::KAutoReleaser ar(&pItm->lock);
   
   _ProcessPsLst(pItm, true, NULL);
   
   pid_t pid = fork();
   
   if (pid < 0)      throw CAppException(eApp_GeneralError, "Unable to fork to spawn process [%s] due to error [%s]", pItm->strImgName.sz(), strerror(errno));
   if (pid > 0)      
   {
      pItm->pid = pid;
      pItm->timPid = KDate::GetTickCount();
      return;
   }
   
   KStr strImgDir = KFileIO::ResolvedPath(KFileIO::FilePath(pItm->strImgName));
   chdir(strImgDir.sz());
   
   Log("Starting [%s] with args [%s] pid [%d] under ppid [%d]", pItm->strImgName.sz(), pItm->strArgs.sz(), getpid(), getppid());
   int nRet = execve(pItm->strImgName.sz(), pItm->args, m_envp);
   
   Log("Unable to start [%s] with args [%s] due to error [%s] with ret [%d]\n", pItm->strImgName.sz(), pItm->strArgs.sz(), strerror(errno), nRet);
   exit(EXIT_FAILURE);
}


void CWDogPxr::CmdStopSvc(const KStr & strCmdItm)
{
   _CmdProcess(strCmdItm, false);
}


void CWDogPxr::CmdKill(const KStr & strCmdItm)
{
   _CmdProcess(strCmdItm, true);
}


pid_t CWDogPxr::_GetCmdPid(const KStr & strCmdItm) const
{
   try
   {
      KFieldReader fr(strCmdItm);
      if (fr.IsEndOfFields())    return 0;
      
      KStr strName = fr.GetKStrField("=").TrimCopy();
      if (strName != "pid")      return 0;
      
      return fr.GetIntField();
   }
   catch (...)
   {}
   
   return 0;
}


void CWDogPxr::_CmdProcess(const KStr & strCmdItm, bool bKill)
{
   KLog OutLog(KFileIO::Join(KFileIO::AppPath(), KFileIO::AppName() + ".output.txt"));
   OutLog.EraseLogFile();
   
   pid_t pid = _GetCmdPid(strCmdItm);
   if (pid)
   {
      _CmdPid(pid, bKill, OutLog);
      return;
   }
   
   bool bFound = strCmdItm=="*" || strCmdItm=="!" || strCmdItm==KFileIO::FileNameAndExt(KFileIO::GetModulePathName());
   
   SWatchItm * pItm;
   for_each_itm(SWatchItm *, pItm, (*m_flpWatchItm.PtrLst()))
   {
      if (!pItm->bValid)                        continue;
      if (KFileIO::FileNameAndExt(pItm->strImgName) != strCmdItm)
      {
         if (strCmdItm == "*")
         {
            if (KFileIO::FileNameAndExt(pItm->strImgName) == KFileIO::FileNameAndExt(KFileIO::GetModulePathName()))            continue;
         }
         else if (strCmdItm != "!")      
         {
            continue;
         }
      }
      
      bFound = true;
      _CmdItm(pItm, bKill, OutLog);
   }
   
   if (!bFound)      OutLog.PlainWrite("Unable to find process [%s] for Cmd [%s]\n", strCmdItm.sz(), bKill?"kill -9":"stop_svc");
   
   if (strCmdItm=="!" || strCmdItm==KFileIO::FileNameAndExt(KFileIO::GetModulePathName()))      _CmdSelf(bKill, OutLog);
}


void CWDogPxr::_CmdPid(pid_t pid, bool bKill, KLog & OutLog)
{
   if (pid == getpid() || pid == 9999)
   {
      SWatchItm * pItm = m_flpWatchItm.GetItm(KFileIO::FileName(KFileIO::GetModulePathName()));
      if (pItm)
      {
         KStr strFifoF = pItm->strImgName+".fifo";
         KStr strW = KStr::Str("%s pid=%d", bKill?"-kill":"-stop_svc", pid);
         
         CFifoInst fifo;
         fifo.SetFifoFile(strFifoF);
         
         fifo.Open();
         fifo.Write(strW);
         fifo.Close(false);
         
         OutLog.PlainWrite("Instructed other wdog to [%s]\n", strW.sz());
         return;
      }
      
      _CmdSelf(bKill, OutLog);
      return;
   }
   
   SWatchItm * pItm = m_flpWatchItm.GetItm(pid);
   if (!pItm)
   {
      OutLog.PlainWrite("Unable to process [%s] cmd because Process with pid [%d] does not exist\n", bKill?"kill -9":"stop_svc", pid);
      return;
   }
   
   _CmdItm(pItm, bKill, OutLog);   
}


void CWDogPxr::_CmdItm(SWatchItm * pItm, bool bKill, KLog & OutLog)
{
   KStr strCmd;
   pItm->bValid = false;
   
   if (bKill)                 strCmd.Format("kill -9 %d", pItm->pid);
   else                       strCmd.Format("stop_svc %s", pItm->strImgName.sz());
   
   system(strCmd.sz());
   OutLog.PlainWrite("Executed [%s]\n", strCmd.sz());
}


void CWDogPxr::_CmdSelf(bool bKill, KLog & OutLog)
{
   KStr strCmd;

   if (bKill)                 strCmd.Format("kill -9 %d", getpid());
   else                       strCmd.Format("stop_svc %s", KFileIO::GetModulePathName().sz());

   OutLog.PlainWrite("Executed Self [%s]\n", strCmd.sz());   
   system(strCmd.sz());
}


void CWDogPxr::_ProcessPsLst(SWatchItm * pItm, bool bCreate, KDLnkList<pid_t> * pLst)
{
   KStr strFilePs = KStr::Str("data/tmp/ps_%s.txt", KFileIO::FileName(pItm->strImgName).sz());

   if (bCreate)      system(KStr::Str("pgrep %s > %s", KFileIO::FileName(pItm->strImgName).sz(), strFilePs.sz()));   
   if (!pLst)        return;
   
   KStr strLn;
   KFileLineReader flr(strFilePs);
   
   while (flr.GetNextLine(strLn))
   {
      pLst->AddTail(atoi(strLn.sz()));
   }
}


bool CWDogPxr::_RplDaemonPid(pid_t pid)
{
   SWatchItm * pFnd = m_flpWatchItm.GetItm(pid);
   if (!pFnd)        return false;
      
   KDLnkList<pid_t> lstPrev;
   KDLnkList<pid_t> lstCurr;

   _ProcessPsLst(pFnd, false, &lstPrev);
   _ProcessPsLst(pFnd, true, &lstCurr);
   
   pid_t pidDmn;
   for_each_itm(pid_t, pidDmn, lstCurr)
   {
      if (lstPrev.Exist(KFunc<pid_t>::Find, &pidDmn))       continue;
      if (!_IsDaemonRpl(pidDmn, pFnd))                      continue;

      Log("Process [%d]==[%s] has been replaced by pid [%d]", pid, pFnd->strImgName.sz(), pidDmn);
      pFnd->pid = pidDmn;
      m_flpWatchItm.LogLst();
      
      return true;
   }
   
   pFnd->timPid = KDate::GetTickCount();
   return false;
}


bool CWDogPxr::_IsDaemonRpl(pid_t pidDmn, SWatchItm * pItm)
{
   if (pidDmn == pItm->pid)         return false;
   
   KStr strExeLink = KStr::Str("/proc/%d/exe", pidDmn);
   KStr strLinkPath;
   
   size_t nCnt = readlink(strExeLink.sz(), strLinkPath.GetBuffer(512), 512);
   if (!nCnt)     return false;
   
   strLinkPath.RecalcLength();
   return pItm->strImgName == strLinkPath;
   
   if (pItm->strImgName != strLinkPath)      return false;
   
   KStr strCmdLnF = KStr::Str("/proc/%d/cmdline", pidDmn);
   KStr strCmdLn;
   
   KFileLineReader flr(strCmdLnF);
   flr.GetNextLine(strCmdLn);
   if (strCmdLn.IsEmpty())       strCmdLn = pItm->strImgName;
   
   KStr strCmdBld = pItm->strImgName + pItm->strArgs;
   strCmdBld.Remove(' ');

   return strCmdBld == strCmdLn;
}


void CWDogPxr::_OutputLst()
{
   if (m_nOutputLstCntDwn > 0)
   {
      m_nOutputLstCntDwn--;
      return;
   }
   
   m_flpWatchItm.LogLst();
}


void CWDogPxr::_MonitorProcessesWait()
{
   try
   {
      while (m_bRun)
      {
         int status = 0;
         pid_t pid = wait(&status);
         if (!m_bRun)      return;
         
         if (pid == -1)
         {
            if (errno != 10)     Log("Error received while waiting on monitored processes errno [%d], error [%s]", errno, strerror(errno));
            return;
         }
         
         if (WIFEXITED(status))
         {
            Log("Process with pid [%d] has exited", pid);
            _RplDaemonPid(pid);
            continue;
         }
         
         if (WIFSIGNALED(status) || WCOREDUMP(status))
         {
            Log("Process [%d] terminated", pid);
            SWatchItm * pFnd = m_flpWatchItm.GetItm(pid);
            if (!pFnd)
            {
               Log("Unable to find process having pid [%d]", pid);
               continue;
            }
            
            Log("Process [%d] terminated found ppFnd where ppFnd->ImgName [%s]", pid, pFnd->strImgName.sz());
            ProcessItm(pFnd);
            m_pxrOutputLst.KickProcess();
         }
      }
   }
   catch_app_exs("_MonitorProcessesWait", DISP);      
}


void CWDogPxr::_MonitorProcessesScan()
{
   if (m_nMonitorScanCntDwn > 0)
   {
      m_nMonitorScanCntDwn--;
      return;
   }
   
   try
   {
      u_int uiTicNow = KDate::GetTickCount();
      KDLnkList<SWatchItm *> * pLst = m_flpWatchItm.PtrLst();
      
      SWatchItm * pItm;
      for_each_itm(SWatchItm *, pItm, (*pLst))
      {
         if (!pItm->bValid)                                                         continue;
         if (uiTicNow - pItm->timPid <= RELAUNCH_INTERVAL)                          continue;
         
         KStr strExeLnk = KStr::Str("/proc/%d/exe", pItm->pid);
         if (KFileIO::FileExist(strExeLnk))        continue;
         
         ProcessItm(pItm);
         m_pxrOutputLst.KickProcess();
      }
   }
   catch_app_exs("_MonitorProcessesScan", DISP);
}


void CWDogPxr::_UnblockWait()
{
   pid_t pid = fork();
   
   if (pid < 0)
   {
      Log("Unable to create fork for Unblocking Wait due to error [%s]", strerror(errno));
      return;
   }
   
   if (pid > 0)      return;
   
   KDate::Sleep(1000);
   exit(EXIT_SUCCESS);
}
