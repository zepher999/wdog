#ifndef WatchItmFLP__h
#define WatchItmFLP__h


#include "WatchItm.h"


class CWDogPxr;
class CWatchItmFLP : public KFileLinePxr<SWatchItm>
{
public:
   CWatchItmFLP();
   
   void SetParent(CWDogPxr * pParent);
   CWDogPxr * GetParent() const;
   
   bool ProcessLine(const KStr & strLn, SWatchItm * pItm);
   void ProcessItem(SWatchItm * pItm);

   void LogLst();
   SWatchItm * GetItm(pid_t pid);
   SWatchItm * GetItm(const KStr & strImgName);
   
   void LoadFileOnly(const KStr & strFile);  
   void SaveFile(const KStr & strFile);
   
   
private:
   CWDogPxr       *  m_pParent;
   bool              m_bLoadOnly;
   u_int             m_uiLasLogTic;
   
   
   
   KStr _GetOutFile(const KStr & strFile);
   KStr _GetInputField(KFieldReader * pFR);
   bool _ProcessLineInput(const KStr & strLn, SWatchItm * pItm, KFieldReader * pFR=NULL);
   bool _ProcessLineOutput(const KStr & strLn, SWatchItm * pItm);
   bool _TooCloseToPrevLog();
};


#endif //WatchItmFLP__h
