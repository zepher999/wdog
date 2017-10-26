#ifndef WatchItm__h
#define WatchItm__h


struct SWatchItm
{
   pid_t          pid;
   KStr           strImgName;
   KStr           strArgs;
   int            nRetryIntrvl;
   int            nRetryLmt;
   bool           bRun;
   bool           bScheduled;

   char       **  args;
   KLock          lock;
   u_int          timPid;
   u_int          uiRetryCnt;
   
   
   
   SWatchItm();
   SWatchItm(const KStr & _strImgName, const KStr & _strArgs, pid_t _pid=0);
   ~SWatchItm();
   
   void ClearArgsArr();
   void Set(int _nRetryIntrvl, int _nRetryLmt, bool _bRun, const KStr & _strImgName, const KStr & _strArgs);
   
   
   static bool FuncFndByImgName(SWatchItm * const & pItm, void * pvFndCriteria);
   static bool FuncFndByPid(SWatchItm * const & pItm, void * pvFndCriteria);
   
   
private:
   void _BldArgsArr();
};


#endif //WatchItm__h

