#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cwchar>
#include <tchar.h>
#include <windows.h>
#include <psapi.h>
#include "util.hpp"


bool CheckIsProcess(DWORD processID, const char *targetProcessName){
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    // Get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    // Get the process name.
    if (NULL != hProcess){
        HMODULE hMod;  DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));   
    }

    // Print the process name and identifier.
    bool ret = (_tcscmp(szProcessName, targetProcessName) == 0);
    CloseHandle(hProcess);
    return ret;
}

DWORD searchVictimPID(const char *targetProcessName){
	DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return 1;

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
    for (unsigned int i = 0; i < cProcesses; i++ )
        if(aProcesses[i]!=0 && CheckIsProcess(aProcesses[i], targetProcessName)) return aProcesses[i];
    return -1;
}


void searchOverVirtualMemory(HANDLE handle, MatchingParam &mparam){
	unsigned char *pAddress = (unsigned char*)0;
	MEMORY_BASIC_INFORMATION memInfo;
	
	while((uintptr_t)pAddress < 0x100000000000 && VirtualQueryEx(handle, (LPCVOID)pAddress, &memInfo, sizeof(memInfo)) != 0){
		if(memInfo.State!=MEM_COMMIT || memInfo.Type != MEM_PRIVATE || memInfo.AllocationProtect!=PAGE_READWRITE || memInfo.Protect==PAGE_NOACCESS || memInfo.Protect==PAGE_GUARD){
			pAddress += memInfo.RegionSize;  continue;
		}

		uintptr_t start = (uintptr_t)memInfo.BaseAddress;
		uintptr_t end = start + memInfo.RegionSize;
		printf("Searching %012llx ~ %012llx\n", start, end);
		
		
		char buff[1024];
		float float_buff = 0;
		for(uintptr_t ptr=(uintptr_t)pAddress; (ptr+4)<=end; ++ptr){
			ReadProcessMemory(handle, (LPVOID)ptr, &float_buff, sizeof(float_buff), NULL);
			if(float_buff == mparam.HPvalue){
				//return;
				sprintf(buff, "Address: %012llx", ptr);
				float_buff = float_buff*2 + 10;
				WriteProcessMemory(handle, (LPVOID)ptr, &float_buff, sizeof(float_buff), NULL);
				int ret = MessageBoxA(NULL, buff, "Is it modified?", MB_YESNO | MB_SETFOREGROUND);
				float_buff = mparam.HPvalue;
				WriteProcessMemory(handle, (LPVOID)ptr, &float_buff, sizeof(float_buff), NULL);
				if(ret == IDYES){
					mparam.HPptr = (float *)ptr;  return;
				}
			}
		}
		pAddress += memInfo.RegionSize;
	}
}


int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Format: %s [HP value]", argv[0]);  exit(1);
	}
	
    DWORD pid = searchVictimPID("demoFPS.exe");
    if(pid == -1){
    	printf("Not found!\n");  exit(1);
    }else printf("Found PID = %u\n", pid);

    
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    
    MatchingParam mparam;
    mparam.HPptr = NULL;  sscanf(argv[1], "%f", &mparam.HPvalue);
    searchOverVirtualMemory(hProcess, mparam);
    printf("\nHP address is at %012llx\n", mparam.HPptr);
    DLLParam dllparam;  dllparam.HPptr = mparam.HPptr;
    writeDLLParam2File("dllparam.tmp", dllparam);
    
    char dll[] = "hack.dll", dllPath[128] = {0};
    GetFullPathNameA(dll, 128, dllPath, NULL);
    LPVOID dllNameRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(hProcess, dllNameRemote, dllPath, strlen(dllPath)+1, NULL);
    HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, dllNameRemote, NULL, NULL);

    WaitForSingleObject(handleThread, INFINITE);
    CloseHandle(handleThread);
    VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
    
    CloseHandle(hProcess);
    return 0;
}