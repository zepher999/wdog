#ifndef WatchItm__h
#define WatchItm__h


struct SWatchItm
{
   pid_t          pid;
   KStr           strImgName;
   KStr           strArgs;
   char       **  args;
   KLock          lock;
   u_int          timPid;
   bool           bValid;
   
   
   
   SWatchItm();
   SWatchItm(const KStr & _strImgName, const KStr & _strArgs, pid_t _pid=0);
   ~SWatchItm();
   
   void ClearArgsArr();
   void Set(const KStr & _strImgName, const KStr & _strArgs);
   
   
private:
   void _BldArgsArr();
};


#endif //WatchItm__h

