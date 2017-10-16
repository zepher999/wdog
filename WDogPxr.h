#ifndef WDogPxr__h
#define WDogPxr__h


#include "WatchItmFLP.h"
#include <poll.h>


class CWDogPxr : public KThreadProcessor
{
public:
   CWDogPxr();
   ~CWDogPxr();
   
   void Activate(char ** envp);
   void Deactivate();
   
   char ** GetEnvp() const;
   void SetEnvp(char ** envp);
   
   void StartWatching(const KStr & strWatchListFile);
   void StopWatching();
   
   void Process();
   void ProcessItm(SWatchItm * pItm);

   void CmdStopSvc(const KStr & strCmdItm);
   void CmdKill(const KStr & strCmdItm);
   
   void JippoOutput();
   void JippoOthr();
   
   
protected:
   char                           **   m_envp;
   int                                 m_nMonitorScanCntDwn;
   int                                 m_nOutputLstCntDwn;
   KPxr<CWDogPxr>                      m_pxrMonitorScan;
   KPxr<CWDogPxr>                      m_pxrOutputLst;
   KStr                                m_strCmdSvcName;
   pid_t                               m_pidCmdSvc;
   CWatchItmFLP                        m_flpWatchItm;
   
   
   
   pid_t _GetCmdPid(const KStr & strCmdItm) const;
   void _CmdProcess(const KStr & strCmdItm, bool bKill);

   void _CmdPid(pid_t pid, bool bKill, KLog & OutLog);
   void _CmdItm(SWatchItm * pItm, bool bKill, KLog & OutLog);
   void _CmdSelf(bool bKill, KLog & OutLog);

   void _StartWatching();
   void _ProcessPsLst(SWatchItm * pItm, bool bCreate, KDLnkList<pid_t> * pLst);

   bool _RplDaemonPid(pid_t pid);
   bool _IsDaemonRpl(pid_t pidDmn, SWatchItm * pItm);
   void _OutputLst();

   void _MonitorProcessesWait();
   void _MonitorProcessesScan();
   void _UnblockWait();
};


#endif //WDogPxr__h
