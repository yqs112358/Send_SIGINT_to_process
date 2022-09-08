#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>
using namespace std;

void PrintLastErrorMessage() {
    DWORD error_message_id = GetLastError();
    if (error_message_id == 0)
        return;

    LPWSTR message_buffer = nullptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, error_message_id, MAKELANGID(0x09, SUBLANG_DEFAULT), (LPWSTR)&message_buffer, 0, nullptr);
    wcerr << wstring(message_buffer) << endl;
    LocalFree(message_buffer);
}

void SendSIGINT(DWORD dwProcessId)
{
    FreeConsole();
    if (!AttachConsole(dwProcessId))
    {
        cerr << "Fail to attach target console!" << endl;
        PrintLastErrorMessage();
    }
    SetConsoleCtrlHandler(NULL, TRUE);
    if (!GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0))
    {
        cerr << "Fail to generate console event!" << endl;
        PrintLastErrorMessage();
    }
    FreeConsole();
}

DWORD GetProcessId(const wchar_t* processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return 0;
    }
    PROCESSENTRY32 pi;
    pi.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet = Process32First(hSnapshot, &pi);
    while (bRet)
    {
        if (wcscmp(pi.szExeFile, processName) == 0)
            return pi.th32ProcessID;
        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);
    return 0;
}

int main(int argc, char** argv)
{
    LPWSTR* szArgList;
    int argCount;
    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (argCount <= 1)
    {
        cerr << "Invalid command argument！" << endl;
        cerr << "Usage:\n\tSend_SIGINT_to_process <ProcessName>" << endl;
        cerr << "For example:\n\tSend_SIGINT_to_process target.exe" << endl;
        exit(1);
    }

    DWORD pid = GetProcessId(szArgList[1]);
    if (pid == 0)
    {
        cerr << "Process no found！" << endl;
        exit(2);
    }

    //cout << "Process ID：" << pid << endl;

    SendSIGINT(pid);
    LocalFree(szArgList);
    //cout << "SIGINT sent." << endl;
}