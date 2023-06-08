#include <windows.h>
#include <iostream>
#include "memtool.h"

int main()
{
    const char *dllPath = "path.dll";
    const wchar_t *processName = L"process.exe";

    DWORD processId = 0;

    // Wait for the process to start
    while (!processId)
    {
        std::cout << "wait for process" << std::endl;
        // Get the process id
        processId = GetProcessId(processName);
        Sleep(30);
    }

    std::cout << "process found" << std::endl;
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    if (!processHandle)
    {
        // Failed to open process
        std::cout << "Error: " << GetLastError() << std::endl;
        return 0;
    }

    if (processHandle && processHandle != INVALID_HANDLE_VALUE)
    {
        std::cout << "Process handle: " << processHandle << std::endl;

        // Allocate memory in the process
        void *loc = VirtualAllocEx(processHandle, 0, sizeof(dllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        // Write the dll path to the process
        WriteProcessMemory(processHandle, loc, dllPath, strlen(dllPath) + 1, 0);

        // Create a thread in the process that will load the dll
        HANDLE hThread = CreateRemoteThread(processHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
        std::cout << "loc" << loc << std::endl;

        if (hThread)
        {
            std::cout << "hThread" << hThread << std::endl;
            std::cout << "Dll injected" << std::endl;
            CloseHandle(hThread);
        }
        else
        {
            std::cout << "Error: " << GetLastError() << std::endl;
        }
    }
    if (processHandle)
    {
        CloseHandle(processHandle);
    }
    std::cin.get();

    return 0;
}