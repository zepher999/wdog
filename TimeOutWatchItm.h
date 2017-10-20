#ifndef TimeOutWatchItm__h
#define TimeOutWatchItm__h

#include "WatchItm.h"


class CWDogPxr;
struct STimeOutWatchItm : public KTimeoutItem
{
   STimeOutWatchItm(CWDogPxr * _pParent=NULL, SWatchItm * _pItm=NULL, u_int uiTimeout=0);
   void FireOffTimeoutResponse();
   

   CWDogPxr       *  pParent;  
   SWatchItm      *  pItm;
};
   

#endif //TimeOutWatchItm__h
