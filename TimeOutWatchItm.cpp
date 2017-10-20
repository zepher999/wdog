#include "stdafx.h"
#include "TimeOutWatchItm.h"
#include "WDogPxr.h"



STimeOutWatchItm::STimeOutWatchItm(CWDogPxr * _pParent, SWatchItm * _pItm, u_int uiTimeout)
{
   pParent = _pParent;
   pItm = _pItm;
   
   pItm->bScheduled = true;
   SetTimeout(uiTimeout);
   Log("Created STimeOutWatchItm pItm->ImgName [%s] for timeout [%u]", pItm->strImgName.sz(), uiTimeout);
}


void STimeOutWatchItm::FireOffTimeoutResponse()
{
   if (!pParent || !pItm)     return;
   
   Log("STimeOutWatchItm::FireOffTimeoutResponse!!!");
   pItm->bScheduled = false;
   pParent->ProcessItm(pItm);
}
