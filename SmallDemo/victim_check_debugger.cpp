#include <cstdio>
#include <synchapi.h>
#include <windows.h>
#include <strsafe.h>

class Player {
private:
	int HP;
	int bombs;
public:
	static int cnt;
	
	Player() : HP(9453), bombs(5) {}

	void shoot() {
		bombs -= 1;
	}

	void show() {
		printf("\r                                                                           ");
		printf("\r%d    HP: %d    bombs: %d    HP address: 0x%012llx", cnt, HP, bombs, &HP);
		cnt += 1;
	}
};

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}





int Player::cnt = 0;

int main() {
	DWORD pid = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	BOOL isRemoteDebugOn = FALSE;
	Player player1;
	while(1){
		player1.show();
		if(CheckRemoteDebuggerPresent(hProcess, (PBOOL)&isRemoteDebugOn) == 0)
			ErrorExit(TEXT("CheckRemoteDebuggerPresent"));
		
		if(isRemoteDebugOn == TRUE){
			MessageBoxA(NULL, "Someone is cheating!", "Remote Debugger Detected", MB_OK | MB_SETFOREGROUND);
			exit(1);
		}
		Sleep(1000);
	}
	return 0;
}