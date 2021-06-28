#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "util.hpp"

extern float *addr_util;
DLLParam dllparam;  char buff[1024];

DWORD WINAPI MainThread(LPVOID param){
	bool muteki = false;
	sprintf(buff, "The address of HP is: 0x%012llx", addr_util);
	MessageBoxA(NULL, buff, "DLL injected", MB_OK | MB_SETFOREGROUND);
	while(1){
		if(GetAsyncKeyState(VK_OEM_4) & 0x80000){
			muteki = true;
			MessageBoxA(NULL, "Cheat Mode On!", "Mode Changed", MB_OK | MB_SETFOREGROUND | MB_APPLMODAL);
		}else if(GetAsyncKeyState(VK_OEM_6) & 0x80000){
			muteki = false;
			MessageBoxA(NULL, "Cheat Mode Off!", "Mode Changed", MB_OK | MB_SETFOREGROUND | MB_APPLMODAL);
		}
		if(muteki) *dllparam.HPptr = 100;
		Sleep(100);
	}
	return 0;
}

bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved){
	if(dwReason == DLL_PROCESS_ATTACH){
		GetModuleFileNameA((HMODULE)hModule, buff, 1024);
		int tmp = strlen(buff);
		while(buff[--tmp] != '\\');
		buff[tmp+1] = '\0';
		strcat(buff, "dllparam.tmp");
		MessageBoxA(NULL, buff, "DLL Record Path", MB_OK | MB_SETFOREGROUND);

		readDLLParamfromFile(buff, dllparam);
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}
	return true;
}