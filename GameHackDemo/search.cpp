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



uintptr_t getHPAddress(HANDLE handle, const uintptr_t start, const uintptr_t end, int match_value){
	uintptr_t ptr = start, target = (uintptr_t)0x61fe10;
	if(target < start || target >= end) return NULL;
	int readValue = 0;
	while((ptr+4) <= end){
		ReadProcessMemory(handle, (LPVOID)ptr, &readValue, sizeof(readValue), NULL);
		if(readValue == match_value) return ptr;
		else ptr += 1;
	}
	return NULL;
}


void searchOverVirtualMemory(HANDLE handle, MatchingParam &mparam){
	unsigned char *pAddress = 0;
	MEMORY_BASIC_INFORMATION memInfo;
	
	while(pAddress < (unsigned char*)0xf0000000 && VirtualQueryEx(handle, pAddress, &memInfo, sizeof(memInfo)) != 0){
		//printf("0x12d\n", pAddress);
		if(memInfo.State!=MEM_COMMIT && memInfo.Protect==PAGE_NOACCESS && memInfo.Protect==PAGE_GUARD || memInfo.State==MEM_FREE || memInfo.AllocationProtect!=PAGE_READWRITE){
			pAddress += memInfo.RegionSize;  continue;
		}

		uintptr_t start = (uintptr_t)memInfo.BaseAddress;
		uintptr_t end = start + memInfo.RegionSize;

		//printf("Searching %012x ~ %012x\n", start, end);
		
		int int_buff = 0;
		for(uintptr_t ptr=(uintptr_t)pAddress; (ptr+4)<=end; ++ptr){
			ReadProcessMemory(handle, (LPVOID)ptr, &int_buff, sizeof(int_buff), NULL);
			if(int_buff == mparam.HPvalue){
				mparam.HPptr = (int *)ptr;
				return;
				//printf("%012x\n", ptr);
			}
		}
		pAddress += memInfo.RegionSize;
	}
}


int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Format: %s [process name]\n", argv[0]);
		exit(1);
	}

    // Get the list of process identifiers.

    DWORD pid = searchVictimPID(argv[1]);
    if(pid == -1){
    	printf("Not found!\n");
    	exit(1);
    }else printf("Found PID = %u\n", pid);

    char dll[] = "hack.dll";
    char dllPath[128] = {0};
    GetFullPathNameA(dll, 128, dllPath, NULL);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    
    MatchingParam mparam;
    mparam.HPptr = NULL;  mparam.HPvalue = 9453;
    searchOverVirtualMemory(hProcess, mparam);
    //printf("%08x\n", mparam.HPptr);


    DLLParam dllparam;
    dllparam.HPptr = mparam.HPptr;
    writeDLLParam2File("dllparam.tmp", dllparam);
    
    LPVOID pszlibFileRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pszlibFileRemote, dllPath, strlen(dllPath)+1, NULL);
    HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszlibFileRemote, NULL, NULL);

    WaitForSingleObject(handleThread, INFINITE);
    CloseHandle(handleThread);
    VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
    
    CloseHandle(hProcess);
    return 0;
}