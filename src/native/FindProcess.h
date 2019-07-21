#include <tlhelp32.h>

using namespace std;

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64(HANDLE processHandle)
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		fnIsWow64Process(processHandle, &bIsWow64);
	}
	return bIsWow64;
}

DWORD GetProcessIdByName(const char* processName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == NULL)
	{
		return 0;
	}

	if (!Process32First(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		return 0;
	}

	do {
		if (strcmp(pe32.szExeFile, processName) == 0)
		{
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &pe32) == TRUE);

	CloseHandle(hSnapshot);
	return 0;
}

BOOL BringToFront(HWND windowHWND) {
	SendMessage(windowHWND, WM_SYSCOMMAND, SC_RESTORE, 0);
	
	if (SetForegroundWindow(windowHWND)) {
		return true;
	}

	return false;
}

struct HandleData {
	unsigned long processId;
	HWND windowHandle;
};

BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	unsigned long processId = 0;
	GetWindowThreadProcessId(handle, &processId);
	if (data.processId != processId || !IsMainWindow(handle))
		return TRUE;
	data.windowHandle = handle;
	return FALSE;
}

HWND FindMainWindowByProcessId(DWORD processId)
{
	HandleData data;
	data.processId = processId;
	data.windowHandle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.windowHandle;
}

HWND FindProcessMainWindowByName(const char* processName)
{
	DWORD processId = GetProcessIdByName(processName);

	if (processId) {
		return FindMainWindowByProcessId(processId);
	}

	return 0;
}
