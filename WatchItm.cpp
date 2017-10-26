#include "stdafx.h"
#include "WatchItm.h"



SWatchItm::SWatchItm()
{
   args = NULL;
   pid = 0;
   timPid = 0;
   uiRetryCnt = 0;
   nRetryIntrvl = 0;
   nRetryLmt = -1;
   bRun = true;
   bScheduled = false;
}


SWatchItm::SWatchItm(const KStr & _strImgName, const KStr & _strArgs, pid_t _pid)
{
   args = NULL;
   pid = _pid;
   timPid = 0;
   uiRetryCnt = 0;
   bScheduled = false;
   
   Set(0, -1, true, _strImgName, _strArgs);
}


SWatchItm::~SWatchItm()
{
   ClearArgsArr();
}


void SWatchItm::ClearArgsArr()
{
   if (!args)                 return;
   
   for (int i=0; args[i]; i++)
   {
      delete [] args[i];
      args[i] = NULL;
   }
   
   delete [] args;
   args = NULL;
}


void SWatchItm::Set(int _nRetryIntrvl, int _nRetryLmt, bool _bRun, const KStr & _strImgName, const KStr & _strArgs)
{
   if (strImgName==_strImgName && strArgs==_strArgs)       return;
   
   strImgName = KFileIO::ResolvedPath(_strImgName);
   strArgs = _strArgs;
   nRetryIntrvl = _nRetryIntrvl;
   nRetryLmt = _nRetryLmt;
   bRun = _bRun;
   
   _BldArgsArr();
}


bool SWatchItm::FuncFndByImgName(SWatchItm * const & pItm, void * pvFndCriteria)
{
   return pItm->strImgName==*((KStr *)pvFndCriteria) || KFileIO::FileName(pItm->strImgName)==*((KStr *)pvFndCriteria);
}


bool SWatchItm::FuncFndByPid(SWatchItm * const & pItm, void * pvFndCriteria)
{
   return pItm->pid == *((int *)pvFndCriteria);
}


void SWatchItm::_BldArgsArr()
{
   strArgs.Trim();
   strArgs.Replace("  ", " ");
    
   ClearArgsArr();
   KDLnkList<KStr> lstArgs;
   lstArgs.AddTail(strImgName);

   KFieldReader fr(strArgs);
   KStr strToks = " \"";

   while (!fr.IsEndOfFields())
   {
      KStr strArg = fr.GetKStrField(strToks.sz()).TrimCopy();
      
      if (fr.GetLastReadTok() == '\"')          
      {
         if (strToks == "\"")
         {
            strToks = " \"";
            strArg = KStr("\"") + strArg + "\"";         
         }
         else
         {
            strToks = "\"";
         }
      }
      else
      {
         strToks = " \"";
      }

      if (strArg.IsEmpty())                     continue;
      lstArgs.AddTail(strArg);
   }
   
   args = new char *[lstArgs.GetCount()+1];
   args[lstArgs.GetCount()] = NULL;
   
   int i = 0;
   KStr strArg;
   for_each_itm(KStr, strArg, lstArgs)
   {
      args[i] = new char[strArg.GetLength()+1];
      strcpy(args[i], strArg.sz());
      
      i++;
   }
}
