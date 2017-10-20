#include "stdafx.h"
#include "FifoInst.h"



#define  MAX_BUFF_LEN  5048



CFifoInst::CFifoInst()
{
   m_bIsOwner = false;
   m_strFifoFile = KFileIO::Join(KFileIO::AppPath(), KFileIO::AppName()+".fifo");
   m_fpFifo = NULL;
}


CFifoInst::~CFifoInst()
{
   Close();
}


void CFifoInst::SetOwner(bool bIsOwner)
{
   m_bIsOwner = bIsOwner;
}


void CFifoInst::SetFifoFile(KStr strFifoFile)
{
   m_strFifoFile = strFifoFile;
}


void CFifoInst::Open()
{
   m_fpFifo = _EnsureFifo();
}


void CFifoInst::Read(KStr & strLn)
{
   if (!m_fpFifo)       throw CAppException(eApp_FileNotFound, "Fifo File [%s] not opened, so cannot proceed to read from it", m_strFifoFile.sz());
   
   strLn.Empty();
   int nCnt = fread(strLn.GetBuffer(MAX_BUFF_LEN), sizeof(char), MAX_BUFF_LEN, m_fpFifo);
   if (nCnt>0)       strLn.RecalcLength();
}


void CFifoInst::Write(const KStr & strLn)
{
   if (!m_fpFifo)       throw CAppException(eApp_FileNotFound, "Fifo File [%s] not opened, so cannot proceed to write to it", m_strFifoFile.sz());
   
   int nCnt = fwrite(strLn.sz(), sizeof(char), strLn.GetLength()+1, m_fpFifo);
   if (nCnt!=strLn.GetLength()+1)       throw CAppException(eApp_FileWriteError, "Wrote [%d] bytes to Fifo File [%s] instead of [%d] due to error [%s]", nCnt, strLn.GetLength()+1, m_strFifoFile.sz(), strerror(errno)); 
}


void CFifoInst::Close(bool bRemove)
{
   if (m_fpFifo)
   {
      fclose(m_fpFifo);
      m_fpFifo = NULL;
   }
   
   if (!bRemove || !m_bIsOwner || !KFileIO::FileExist(m_strFifoFile))     return;
   
   int nRes = unlink(m_strFifoFile);
   if (nRes)      Log("Unable to delete FifoFile [%s] due to error [%s]", m_strFifoFile.sz(), strerror(errno));
}


void CFifoInst::Clear()
{
   m_fpFifo = NULL;
}


FILE * CFifoInst::_EnsureFifo()
{
   if (!m_bIsOwner && !KFileIO::FileExist(m_strFifoFile))         throw CAppException(eApp_FileNotFound, "Fifo File [%s] not found implying process is inactive", m_strFifoFile.sz());

   if (m_bIsOwner && KFileIO::FileExist(m_strFifoFile))
   {
      int nRes = unlink(m_strFifoFile);
      if (nRes)      Log("Unable to delete FifFile [%s] due to error [%s]", m_strFifoFile.sz(), strerror(errno));
   }
      
   if (!KFileIO::FileExist(m_strFifoFile))
   {
      int nRes = mkfifo(m_strFifoFile.sz(), S_IWOTH | S_IROTH | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
      if (nRes)      throw CAppException(eApp_FileWriteError, "Unable to create Fifo File [%s] due to error [%s]", m_strFifoFile.sz(), strerror(errno));
   }
   
   FILE * fp = fopen(m_strFifoFile.sz(), m_bIsOwner ? "rb" : "rb+");
   if (!fp)    throw CAppException(eApp_FileWriteError, "Unable to open Fifo File [%s] for read/write access due to error [%s]", m_strFifoFile.sz(), strerror(errno));

   return fp;  
}
