////////////////////////////////////////////////////////////////////////
//
// Loggers.cpp: source file
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
#include "Loggers.h"



KLog Log;


void InitLoggers()
{
   Log.SetLogFile(KFileIO::Join(KFileIO::AppPath(), "Logs/") + KFileIO::AppName() + ".log");
   Log.SetLoggingOn(true);
   //Log.EraseLogFile();
   Log.LogHeader();

   SetAppLogPtr(&Log);
}


void DuoLog(const char * szFmt, ...)
{
   va_list vl;
   va_start(vl, szFmt);
   KStr strLn;
   strLn.FormatV(szFmt, vl);
   va_end(vl);

   printf("%s\n", strLn.sz());
   Log(strLn);
}



