#include "stdafx.h"
#include "ConsMain.h"



int main(int argc, char * argv[], char * envp[])
{
   CConsoleMain cm;

   try
   {
      chdir(KFileIO::AppPath().sz());
      cm.ParseArgs(argc, argv, envp);
      cm.Process();
      
      printf("Done\n");
      return 0;
   }
   catch_app_exs("main", DISP);
   cm.ShowUsage();
   return -1;
}
