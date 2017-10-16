#include "stdafx.h"
#include "WatchItmFLP.h"
#include "WDogPxr.h"


#define MIN_TIM_BETW_LOGS 999



CWatchItmFLP::CWatchItmFLP()
{
   m_uiLasLogTic = 0;
}


void CWatchItmFLP::SetParent(CWDogPxr * pParent)
{
   m_pParent = pParent;
   m_bLoadOnly = false;
}


CWDogPxr * CWatchItmFLP::GetParent() const
{
   return m_pParent;
}


bool CWatchItmFLP::ProcessLine(const KStr & strLn, SWatchItm * pItm)
{
   bool bSuccess = m_bLoadOnly ? _ProcessLineOutput(strLn, pItm) : _ProcessLineInput(strLn, pItm);
   if (!bSuccess)       return false;
   
   SWatchItm * pFnd = GetItm(pItm->strImgName);
   if (!pFnd)        return true;
   
   return pItm->strArgs != pFnd->strArgs;
}


void CWatchItmFLP::ProcessItem(SWatchItm * pItm)
{
   if (!pItm->pid)    m_pParent->ProcessItm(pItm);
}


void CWatchItmFLP::LogLst()
{
   SaveFile(m_strFile);
   //if (_TooCloseToPrevLog())     return;
   
   //KDate::Sleep(1003);
   Log("");
   Log("");
   
   Log("%-10s %-20s %-30s", "Pid", "ImageName", "Args");
   Log("----------------------------------------------------------------------------");
   
   SWatchItm * pItm;
   for_each_itm (SWatchItm *, pItm, m_lstItems)
   {
      Log("%-10d %-20s %-30s", pItm->pid, pItm->strImgName.sz(), pItm->strArgs.sz());
   }
   
   Log("============================================================================");
}


SWatchItm * CWatchItmFLP::GetItm(pid_t pid)
{
    SWatchItm ** ppFnd = m_lstItems.GetItemPtr(_FuncFndByPid, &pid);
    
    return ppFnd ? *ppFnd : NULL;
}


SWatchItm * CWatchItmFLP::GetItm(const KStr & strImgName)
{
   SWatchItm ** ppFnd = m_lstItems.GetItemPtr(_FuncFndByImgName, (KStr *)&strImgName);
    
   return ppFnd ? *ppFnd : NULL;
}


void CWatchItmFLP::LoadFileOnly(const KStr & strFile)
{
   KStr strOutFile = _GetOutFile(strFile);
   if (!KFileIO::FileExist(strOutFile))       return;
   
   m_bLoadOnly = true;
   KFileLinePxr<SWatchItm>::LoadFile(strOutFile, false);
   
   m_bLoadOnly = false;
}


void CWatchItmFLP::SaveFile(const KStr & strFile)
{
   KStr strOutFile = _GetOutFile(strFile);
    
   FILE * fp = fopen(strOutFile.sz(), "wt");
   if (!fp)
   {
      KFileIO::EnsureFilePath(strOutFile);
      fp = fopen(strOutFile.sz(), "wt");
      
      if (!fp)       throw CAppException(eApp_FileCreateError, "Unable to create WatchItm Pxr Output file [%s] due to error [%s]", strOutFile.sz(), strerror(errno));
   }
   
   KAutoReleaser::KAutoFileClose afc(&fp);
      
   SWatchItm * pItm;
   for_each_itm (SWatchItm *, pItm, m_lstItems)
   {
      fprintf(fp, "%d, %s, %s\n", pItm->pid, pItm->strImgName.sz(), pItm->strArgs.sz());
   }
}


KStr CWatchItmFLP::_GetOutFile(const KStr & strFile)
{
   KStr strDataTmp = KFileIO::Join(KFileIO::FilePath(strFile), "tmp");
   return KFileIO::Join(strDataTmp, KFileIO::FileName(strFile)) + ".out" + KFileIO::FileExt(strFile);
}


KStr CWatchItmFLP::_GetInputField(KFieldReader * pFR)
{
   try
   {
      return pFR->GetKStrField(",").TrimCopy();   
   }
   catch (KFieldReaderException)
   {}  
    
   return "";
}


bool CWatchItmFLP::_ProcessLineInput(const KStr & strLn, SWatchItm * pItm, KFieldReader * pFR)
{
   try
   {
      KFieldReader fr(strLn);
      pFR = pFR ? pFR : &fr;
      
      KStr strImgName = _GetInputField(pFR);
      if (strImgName.IsEmpty())        return false;
      
      pItm->Set(strImgName, _GetInputField(pFR));
      return true;
   }
   catch (KFieldReaderException & e)
   {
      Log("CWatchItmFLP::_ProcessLineInput caught KFieldReaderException, Name [%s], Desc [%s]", e.GetExceptionName(), e.GetExceptionDesc());
   }
   
   return false;
}


bool CWatchItmFLP::_ProcessLineOutput(const KStr & strLn, SWatchItm * pItm)
{
   try
   {
      KFieldReader fr(strLn);
      pItm->pid = fr.GetIntField(",");
      
      return _ProcessLineInput("", pItm, &fr);
   }
   catch (KFieldReaderException & e)
   {
      Log("CWatchItmFLP::_ProcessLineOutput caught KFieldReaderException, Name [%s], Desc [%s]", e.GetExceptionName(), e.GetExceptionDesc());
   }
   
   return false;
}


bool CWatchItmFLP::_TooCloseToPrevLog()
{
   u_int uiTic = KDate::GetTickCount();
   u_int uiDiff = uiTic >= m_uiLasLogTic ? uiTic - m_uiLasLogTic : 0xFFFFFFFF - m_uiLasLogTic + uiTic;
   
   if (uiDiff <= MIN_TIM_BETW_LOGS)       return true;
   
   m_uiLasLogTic = uiTic;
   return false;
}


bool CWatchItmFLP::_FuncFndByPid(SWatchItm * const & pItm, void * pvFndCriteria)
{
   return pItm->bValid && pItm->pid == *((int *)pvFndCriteria);
}


bool CWatchItmFLP::_FuncFndByImgName(SWatchItm * const & pItm, void * pvFndCriteria)
{
   return pItm->bValid && (pItm->strImgName == *((KStr *)pvFndCriteria) || KFileIO::FileName(pItm->strImgName) == *((KStr *)pvFndCriteria));
}


