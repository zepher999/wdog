////////////////////////////////////////////////////////////////////////
//
// ConsMain.h: header file
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


#ifndef ConsoleMain__h
#define ConsoleMain__h

#include "WDogPxr.h"


class CConsoleMain
{
public:
   CConsoleMain();
   ~CConsoleMain();

   void ShowUsage();
   void ParseArgs(int argc, char * argv[], char * envp[]);
   void Process();
   

private:
   char           ** m_envp;
   KStr              m_strWatchListFile;
   KStr              m_strWatchListFileOther;
   KStr              m_strSvcName;
   KStr              m_strCmd;
   KStr              m_strCmdArg;
   bool              m_bIssueCmd;
   bool              m_bSetup000;
   bool              m_bRun000;
   bool              m_bCold;
   CWDogPxr          m_pxrWDog;
   
   
   void _IssueCmd();
   bool _ParseHotColdSwitch(const KStr & strHotColdSwitch, bool bThrow);

   void _ClearTmpPath(const KStr & strTmpPath);
   void _ClearTmps();
   
   void _DoSetup000();
   void _InitRun000();
   void _Daemonize();
   
   void _EnsureDefCsv(const KStr & strBin, const KStr & strCsv, const KStr & strCsvOther, bool bAddRec);
   void _UpdtDefCsv(const KStr & strBin, const KStr & strCsv, const KStr & strCsvOther);
   void _UpdtOthrWatchList();
};


#endif //ConsoleMain__h
