#ifndef FifoInst__h
#define FifoInst__h


class CFifoInst
{
public:
   CFifoInst();
   ~CFifoInst();

   void SetOwner(bool bIsOwner);
   void SetFifoFile(KStr strFifoFile);
   
   void Open();
   void Read(KStr & strLn);
   void Write(const KStr & strLn);
   void Close(bool bRemove=true);
   void Clear();
   
   
      
private:
   FILE     *  m_fpFifo;
   KStr        m_strFifoFile;
   bool        m_bIsOwner;
   

   FILE * _EnsureFifo();
};


#endif //FifoInst__h
