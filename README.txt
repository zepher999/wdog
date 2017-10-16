
DESCRIPTION
----------------
wdog is a linux watchdog utility written in c++.  

The utility is capable of starting a list of specified processes and keeping them alive as well as stopping or killing any of these.  

The utility also has the ability of launching an instance in order to watch the watch dog.


USAGE
-------
usage: wdog  <-000_setup>
       wdog  <-000_run>  [-cold]=def or [-hot]
       wdog  <watch_list_file>  [watch_list_file_othr]  [-cold] or [-hot]=def

       wdog  <-stop_svc>  [!] or [*] or [process_name] or [pid=x]
       wdog  <-kill>      [!] or [*] or [process_name] or [pid=x]

       where  <watch_list_file> is path of file with list of processes to be watched
          and  [watch_list_file_othr] is path to other wdog's <watch_list_file>
          and  <-000_setup> is switch for creating 000 setup
          and  <-000_run> is switch for running wdog as 000 instance
          and  [-cold] is switch for starting wdog in cold mode
          and  [-hot] is switch for starting wdog in hot mode
               cold mode => remove tmp data so as NOT to use it.
               hot mode => keep tmp data so as to use it.

          and where  [x] == process id
                and  [*] == all processes
                and  [!] == all processes including wdog processes.



NOTES
------
wdog is built with the g++ compiler from ubuntu 15.04 and links against the Kahless_9 framework.

Running wdog with the first option [-000_setup] will perform a 000 setup which is merely the utility 
creating a copy of itself under a [wdog_000] sub folder which will be considered the [000] version.

This is the version that is actually run with the [-000_run] option which in turn processes it's 
csv file named [wdog_000.csv] located in a [data] subfolder under the [wdog_000] folder.

When running [-000_setup] wdog will automatically create this [wdog_000.csv] file with a single record inside
that launches the real [wdog] process found in the parent directory to this [wdog_000] subfolder.

The real [wdog] instance will then launch its processes to be watched and it will itself be watched by the 000
version.  The 000 version will consequently also be watched by the real version but only via it scanning mechanism
and not through signalling.


   Example:
   --------
   assume we copied wdog to the following path:
      /home/user/Projects/Tools/bins/wdog_process$
   
   
   then in this path we run:
      /home/user/Projects/Tools/bins/wdog_process$ wdog -000_setup
   
   
   this will create the following subfolder:
   	/home/user/Projects/Tools/bins/wdog_process/wdog_000$
   	
   	
   with a copy of wdog therein and a data subfolder containing file [wdog_000.csv].
   then [wdog_000].csv will contain the following record:
      /home/user/Projects/Tools/bins/wdog_process/wdog  /home/user/Projects/Tools/bins/wdog_process/data/wdog_process.csv  /home/user/Projects/Tools/bins/wdog_process/wdog_000/wdog_000.csv
      
      
   a data subfolder will be created under [/home/user/Projects/Tools/bins/wdog_process/]
   which will contain an empty file called [wdog_process.csv].
   
   
   within this [wdog_process.csv] file we add in a fully qualified path to a process
   to be started and watched and the parameters with which it should be started, seperated
   by a comma from the process path.
   
   Note that if this file or any others within the [-000_setup] process already existed then it will not be overwritten.
   
   
   To run a simple singular instance of wdog we can do the following:
      /home/user/Projects/Tools/bins/wdog_process$ wdog data/wdog_process.csv
   
   
   this will startup the process instance of wdog with all its processes to be watched in [wdog_process.csv],
   
   
   In order to startup the other wdog process [the 000 instance] to startup and watch the real one we can do the following:
      /home/user/Projects/Tools/bins/wdog_process/wdog_000$  wdog  /home/user/Projects/Tools/bins/wdog_process/wdog_000/data/wdog_000.csv   /home/user/Projects/Tools/bins/wdog_process/data/wdog_process.csv
      
   or using relative paths we can do the following as well:
      /home/user/Projects/Tools/bins/wdog_process/wdog_000$  wdog  data/wdog_000.csv   ../data/wdog_process.csv
      
      
   or more simply:
      /home/user/Projects/Tools/bins/wdog_process$  wdog  -000_run
      
      
   
The options [-stop_svc] and [-kill] are built into wdog to make stopping/terminating of a watched process possible while wdog is still active.

Basically if you were to kill and exit one of the watched processes, then wdog will detect it and attempt to restart the process unless it was instructed via 
the [-stop_svc] or [-kill] option to do so.

The [-kill] option sends the kill -9 signal to a process.  This wdog option must be accompanied by either a pid=xyz or process_name where xyz = [a process id] 
and process_name is the fully qualified and plain name of a process being watched by wdog.  If a plain process name is specified then wdog will send the kill -9 
signal to all processes of that name.

The [-stop_svc] option utilizes a built in utility of the Kahless_9 framework called stop_svc.  This is a very simple utility as can be seen from its source code and 
basically only expects a process name (or fully qualified if not being run within processes binary path) to which it checks if an associated [.fifo] file exists in which 
case it writes a [STOP_SVC] to it.

The owning process of this fifo file can then read this string out of the file and act upon it by typically exiting in a graceful way.

The accompanying parameters for [-stop_svc] is exactly the same as for the [-kill] option.




   
   
   
   