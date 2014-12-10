//http://sone.tistory.com/234
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

DWORD __stdcall ThreadFunction(LPVOID lparam)
{
	while (1)
		__asm rdtsc
	return 0;
}

int HWBP(HANDLE hThread)
{
	CONTEXT mt;
	memset(&mt, 0, sizeof(mt));

	mt.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	Sleep(500);
	//SuspendThread(hThread);
	GetThreadContext(hThread, &mt);
	if (mt.Dr0 || mt.Dr1 || mt.Dr2 || mt.Dr3)
	{
		printf("HWBP detected!\n");
	}
	else
	{
		printf("HWBP not detected!\n");
	}
	ResumeThread(hThread);

	return 0;
}

int TID(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return 0;

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);     // Must clean up the snapshot object!
		return(FALSE);
	}

	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			HWBP(OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID));
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return 1;
}

int main(void)
{
	HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);

	while (1)
	{
		Sleep(500);
		TID(GetCurrentProcessId());
	}

	return 0;
}