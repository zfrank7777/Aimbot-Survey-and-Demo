# GameHackDemo  
We made a health recovery plug-in for our self-made game "FPSdemo.exe".  
The program searchGame.cpp first search for the address at which the HP value is stored.  
After knowing the address, the plug-in DLL "hack.dll" is then injected into the game executable.  
Everytime the player presses the key '\[{' (VK_OEM_4), the cheat mode is then turned on, and the HP value will be reset to 100 periodically.  
If the player presses the key '\]}' (VK_OEM_6), the cheat mode will be turned off.
