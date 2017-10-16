#include "stdafx.h"
#include "WatchItm.h"



SWatchItm::SWatchItm()
{
   args = NULL;
   pid = 0;
   timPid = 0;
   bValid = true;
}


SWatchItm::SWatchItm(const KStr & _strImgName, const KStr & _strArgs, pid_t _pid)
{
   args = NULL;
   pid = _pid;
   timPid = 0;
   bValid = true;
   
   Set(_strImgName, _strArgs);
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


void SWatchItm::Set(const KStr & _strImgName, const KStr & _strArgs)
{
   if (strImgName==_strImgName && strArgs==_strArgs)       return;
   
   strImgName = KFileIO::ResolvedPath(_strImgName);
   strArgs = _strArgs;
   
   _BldArgsArr();
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
