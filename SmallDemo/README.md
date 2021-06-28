# Small Demo

This demonstrates how memory scanning is done.  
  
## victim.cpp  
The first version of the targeted victim program which is not equipped with any defense.  

## search.cpp  
The program responsible for memory scanning, finding where the HP value is stored.  
  
## victim_close_privilege.cpp  
An updated version of "victim.cpp". It closes all privileges before the game start.  
However, it is apparent that read/write permissions to its private memory space is still granted to other programs (e.g. "search.cpp").  
Surprisingly, it is still possible to attach a debugger program (e.g. "CheatEngine") to it.  
We then concluded that on a Windows10 platform, it is useless to set privileges only when it comes to preventing game cheats.  
  
## victim_check_debugger.cpp  
Another patched version of "victim.cpp".  
Instead of closing all privileges, it periodically checks whether a remote debug program is attached to itself.  
This time, when we try to start a debugger within "CheatEngine", this program stops immediately upon detection.  
It is worthy to note that this trick does not hinder a debugger-free memory manipulation.  
Hence, the attacker can still read/write to the program's memory space without being detected.
