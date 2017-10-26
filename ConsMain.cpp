////////////////////////////////////////////////////////////////////////
//
// ConsMain.cpp: source file
//
// Copyright (c) 2015, ShanKoDev Pty (Ltd). All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the owners written consent, and
// providing that this notice and the company name and all copyright
// notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// ShanKoDev accepts no liability for any damage/loss of business that
// this product may cause.
//
// See Kahless_9_user_license_agreement.txt located in framework root
// for the official terms and conditions of this license agreement.
//
// For use with Kahless_9
//
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConsMain.h"
#include "WDogPxr.h"
#include "FifoInst.h"


CConsoleMain::CConsoleMain()
{
   m_bSetup000 = false;
   m_bRun000 = false;
   m_bCold = false;
   m_bIssueCmd = false;

   
   m_strWatchListFile = KStr("data/") + KFileIO::AppName() + "_000.csv";
}


CConsoleMain::~CConsoleMain()
{

}


void CConsoleMain::ShowUsage()
{
   printf("\nusage: %s  <-000_setup>\n", KFileIO::AppName().sz());
   printf("       %s  <-000_run>  [-cold]=def or [-hot]\n", KFileIO::AppName().sz());
   printf("       %s  <watch_list_file>  [watch_list_file_othr]  [-cold] or [-hot]=def\n\n", KFileIO::AppName().sz());
   
   printf("       %s  <-start_svc> [!] or [*] or [process_name]\n", KFileIO::AppName().sz());
   printf("       %s  <-stop_svc>  [!] or [*] or [process_name] or [pid=x]\n", KFileIO::AppName().sz());
   printf("       %s  <-kill>      [!] or [*] or [process_name] or [pid=x]\n\n", KFileIO::AppName().sz());
         
   printf("       where  <watch_list_file> is path of file with list of processes to be watched\n");
   printf("          and  [watch_list_file_othr] is path to other wdog's <watch_list_file>\n");
   printf("          and  <-000_setup> is switch for creating 000 setup\n");
   printf("          and  <-000_run> is switch for running wdog as 000 instance\n");
   printf("          and  [-cold] is switch for starting wdog in cold mode\n");
   printf("          and  [-hot] is switch for starting wdog in hot mode\n");
   printf("               cold mode => remove tmp data so as NOT to use it.\n"); 
   printf("               hot mode => keep tmp data so as to use it.\n\n"); 
   
   printf("          and where  [x] == process id\n");
   printf("                and  [*] == all processes\n");
   printf("                and  [!] == all processes including wdog processes.\n\n");
}


void CConsoleMain::ParseArgs(int argc, char * argv[], char * envp[])
{
   if (argc < 2)                          throw CAppException(eApp_ShowUsage, "");
   if (argc > 4)                          throw CAppException(eApp_InvalidNumArgs, "Invalid number of arguments[%d] parsed", argc-1); 

   m_envp = envp;
   KStr strArg = argv[1];
  
   if (strArg == "-000_setup")
   {
      if (argc != 2)                          throw CAppException(eApp_InvalidNumArgs, "Invalid number of arguments[%d] parsed for -000_setup switch", argc-1);
      m_bSetup000 = true;
      return;
   }

   if (strArg == "-000_run")
   {
      m_bRun000 = true;
      m_bCold = true;
      
      if (argc == 3)                         _ParseHotColdSwitch(argv[2], true);
      else if (argc != 2)                    throw CAppException(eApp_InvalidNumArgs, "Invalid number of arguments[%d] parsed for -000_run switch", argc-1);
      return;
   }
      
   if (strArg == "-start_svc" || strArg == "-stop_svc" || strArg == "-kill")
   {
      if (argc != 3)                          throw CAppException(eApp_InvalidNumArgs, "Invalid number of arguments[%d] parsed for -stop_svc switch", argc-1);
      
      m_strCmd = strArg;
      m_strCmdArg = argv[2];
      m_bIssueCmd = true;
      return;
   }
         
   if (argc >= 3)
   {
      if (argc == 4)                               _ParseHotColdSwitch(argv[3], true);
      if (!_ParseHotColdSwitch(argv[2], false))    m_strWatchListFileOther = KFileIO::ResolvedPath(argv[2]);
   }

   m_strWatchListFile = KFileIO::ResolvedPath(argv[1]);
}


void CConsoleMain::Process()
{
   if (m_bSetup000)           _DoSetup000();
   else if (m_bRun000)        _InitRun000();
   else if (m_bIssueCmd)      _IssueCmd();
   else                       _Daemonize();
}


void CConsoleMain::_IssueCmd()
{
   KStr strOutLog = KFileIO::Join(KFileIO::AppPath(), KFileIO::AppName() + ".output.txt");
   unlink(strOutLog.sz());
   
   CFifoInst fifoInst;
   fifoInst.Open();
   fifoInst.Write(m_strCmd + " " + m_strCmdArg);
   fifoInst.Close();
   
   for (int i=0; i<3 && !KFileIO::FileExist(strOutLog); i++)
   {
      KDate::Sleep(1011);
   }
   
   if (!KFileIO::FileExist(strOutLog))    return;
   KDate::Sleep(1511);
   
   system(KStr::Str("cat %s", strOutLog.sz()).sz());
}


bool CConsoleMain::_ParseHotColdSwitch(const KStr & strHotColdSwitch, bool bThrow)
{
   if (strHotColdSwitch == "-cold")
   {
      m_bCold = true;
      return true;
   }
   
   if (strHotColdSwitch == "-hot")
   {
      m_bCold = false;
      return true;
   }
   
   if (bThrow)    throw CAppException(eApp_InvalidSwitch, "Invalid hot/cold switch, expecting [-cold] or [-hot]");
   return false;
}


void CConsoleMain::_ClearTmpPath(const KStr & strTmpPath)
{
   if (!KFileIO::DirExist(strTmpPath))       return;
   
   KDirLister dl;
   dl.SetDirectory(strTmpPath);
   dl.SetFileSpec("*.csv; *.txt");
   dl.Retrieve();
   
   KStr strFile;
   for_each_itm(KStr, strFile, (*dl.GetFileListPtr()))
   {
      unlink(KFileIO::Join(strTmpPath, strFile).sz());
   }
}


void CConsoleMain::_ClearTmps()
{
   _ClearTmpPath(KFileIO::Join(KFileIO::FilePath(m_strWatchListFile), "tmp"));
   
   if (!m_strWatchListFileOther.IsEmpty())      
   {
      _ClearTmpPath(KFileIO::Join(KFileIO::FilePath(m_strWatchListFileOther), "tmp"));
   }
}


void CConsoleMain::_DoSetup000()
{
   KStr strSub000 = KStr::Str("%s_000", KFileIO::AppName().sz());
   KStr strDir000 = KFileIO::Join(KFileIO::AppPath(), strSub000);
   KStr strBin000 = KFileIO::Join(strDir000, KFileIO::AppName());
   KStr strCsv000 = KFileIO::Join(KFileIO::Join(strDir000, "data"), KStr::Str("%s_000.csv", KFileIO::AppName().sz()));
   
   KStr strDirProcess = KFileIO::AppPath();
   KStr strBinProcess = KFileIO::GetModulePathName();
   KStr strCsvProcess = KFileIO::Join(KFileIO::Join(strDirProcess, "data"), KStr::Str("%s_process.csv", KFileIO::AppName().sz()));
   
   if (!KFileIO::FileExist(strBin000))
   {
      KFileIO::EnsureFilePath(strBin000);
      KFileIO::FileCopy(strBin000, strBinProcess);
      KFileIO::SetAttrExec(strBin000, true);
      KFileIO::SetAttrExec(strBin000, true, eAUsr_Others);
      KFileIO::SetAttrExec(strBin000, true, eAUsr_Group);
   }
   
   
   _EnsureDefCsv(strBinProcess, strCsvProcess, strCsv000, true);
   _EnsureDefCsv(strBin000, strCsv000, strCsvProcess, false);
}


void CConsoleMain::_InitRun000()
{
   KStr strSub000 = KStr::Str("%s_000", KFileIO::AppName().sz());
   KStr strDir000 = KFileIO::Join(KFileIO::AppPath(), strSub000);
   KStr strBin000 = KFileIO::Join(strDir000, KFileIO::AppName());
   KStr strCsv000 = KFileIO::Join(KFileIO::Join(strDir000, "data"), KStr::Str("%s_000.csv", KFileIO::AppName().sz()));
   
   KStr strDirProcess = KFileIO::AppPath();
   KStr strCsvProcess = KFileIO::Join(KFileIO::Join(strDirProcess, "data"), KStr::Str("%s_process.csv", KFileIO::AppName().sz()));
   KStr strArg0 = KFileIO::AppName();
   
   char * argv[5];
   argv[0] = strArg0.sz();
   argv[1] = strCsv000.sz();
   argv[2] = strCsvProcess.sz();
   argv[3] = m_bCold ? (char *)"-cold" : (char *)"-hot";
   argv[4] = NULL;
   
   pid_t pid = fork();
   if (pid < 0)         throw CAppException(eApp_GeneralError, "Error trying to fork for -000_run [%s]", strerror(errno));
   if (pid > 0)         exit(EXIT_SUCCESS);
   
   KDate::Sleep(999);
   int ret = execve(strBin000.sz(), argv, m_envp);
   exit(EXIT_FAILURE);
}


void CConsoleMain::_Daemonize()
{
   pid_t pidF = fork();
   if (pidF < 0)        exit(EXIT_FAILURE);
   if (pidF > 0)        exit(EXIT_SUCCESS);
   
   umask(0);
   setsid();
   
   KDate::Sleep(333);
   InitLoggers();
   Log("--> Daemonizing  pid [%d]", getpid());

   try
   {
      if (m_bCold)                                       _ClearTmps();   
      if (!m_strWatchListFileOther.IsEmpty())            _UpdtOthrWatchList();
      
      Log("Activating pxr WDog");
      m_pxrWDog.Activate(m_envp);
      m_pxrWDog.StartWatching(m_strWatchListFile);
      
      Log("Opening Fifo inst");
      CFifoInst fifoInst;
      fifoInst.SetOwner(true);
      fifoInst.Open();

      while (true)
      {
         KDate::Sleep(3000);
         
         KStr strLn;
         fifoInst.Read(strLn);
         
         if (strLn.IsEmpty())             continue;
         if (strLn == "STOP_SVC")         break;
         
         KFieldReader fr(strLn);
         KStr strCmd = fr.GetKStrField(" ");
         KStr strCmdItm = fr.GetLine();
         
         if (strCmd == "-start_svc")         m_pxrWDog.CmdProcess(strCmdItm, eCmd_Start);
         else if (strCmd == "-stop_svc")     m_pxrWDog.CmdProcess(strCmdItm, eCmd_Stop);
         else if (strCmd == "-kill")         m_pxrWDog.CmdProcess(strCmdItm, eCmd_Kill);
         else                                Log("Invalid cmd [%s] received", strCmd.sz());
      }
      
      fifoInst.Close();
      m_pxrWDog.StopWatching();
      m_pxrWDog.Deactivate();
      exit(EXIT_SUCCESS);
   }
   catch_app_exs("CConsoleMain::_Daemonize", DISP);
   exit(EXIT_FAILURE);
}


void CConsoleMain::_EnsureDefCsv(const KStr & strBin, const KStr & strCsv, const KStr & strCsvOther, bool bAddRec)
{
   if (KFileIO::FileExist(strCsvOther))
   {
      if (bAddRec)      _UpdtDefCsv(strBin, strCsv, strCsvOther);
      return;
   }
   
   KFileIO::EnsureFilePath(strCsvOther);
   
   FILE * fp = fopen(strCsvOther.sz(), "wt");
   if (!fp)     throw CAppException(eApp_FileOpenError, "Unable to create new csv file [%s] due to error [%s]", strCsvOther.sz(), strerror(errno));
   KAutoReleaser::KAutoFileClose afc(&fp);
   
   fprintf(fp, "--%s, %s, %s, %s, %s\n", "Retry Intrvl", "Retry Lmt", "Run", "Image Path", "Arguments");
   fprintf(fp, "------------------------------------------------------------------\n");

   if (bAddRec)      fprintf(fp, "%d, %d, %d, %s, %s %s\n", 0, -1, 1, strBin.sz(), strCsv.sz(), strCsvOther.sz());
}


void CConsoleMain::_UpdtDefCsv(const KStr & strBin, const KStr & strCsv, const KStr & strCsvOther)
{
   CWatchItmFLP flpWatchItm;
   flpWatchItm.LoadFile(strCsvOther);
   
   SWatchItm * pFnd = flpWatchItm.GetItm(strBin);
   
   if (pFnd)         pFnd->strArgs = strCsv + " " + strCsvOther;
   else              flpWatchItm.PtrLst()->AddHead(new SWatchItm(strBin, strCsv + " " + strCsvOther));

   FILE * fp = fopen(strCsvOther.sz(), "wt");
   if (!fp)     throw CAppException(eApp_FileOpenError, "Unable to create new csv file [%s] for update due to error [%s]", strCsvOther.sz(), strerror(errno));
   KAutoReleaser::KAutoFileClose afc(&fp);

   SWatchItm * pItm;
   for_each_itm(SWatchItm *, pItm, (*flpWatchItm.PtrLst()))
   {
      fprintf(fp, "%d, %d, %d, %s", pItm->nRetryIntrvl, pItm->nRetryLmt, pItm->bRun, pItm->strImgName.sz());
      if (pItm->strArgs.IsEmpty())     continue;
      
      fprintf(fp, ", %s\n", pItm->strArgs.sz());
   }
}


void CConsoleMain::_UpdtOthrWatchList()
{
   CWatchItmFLP flpWatchItmOthr;
   flpWatchItmOthr.LoadFileOnly(m_strWatchListFileOther);
   
   KStr strImgName = KFileIO::GetModulePathName();
   SWatchItm * pItm = flpWatchItmOthr.GetItm(strImgName);
   
   KStr strArgs = KFileIO::ResolvedPath(m_strWatchListFile) + " " + KFileIO::ResolvedPath(m_strWatchListFileOther);
   if (pItm)               pItm->pid = getpid();
   else                    flpWatchItmOthr.PtrLst()->AddTail(new SWatchItm(strImgName, strArgs, getpid()));
 
   flpWatchItmOthr.SaveFile(m_strWatchListFileOther);
}

