
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <psapi.h>
 
 
typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
void SuspendProcess(DWORD process_id) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    NtSuspendProcess pfnNtSuspendProcess =
        (NtSuspendProcess)GetProcAddress(GetModuleHandle(L"ntdll"), "NtSuspendProcess");
    pfnNtSuspendProcess(processHandle);
    CloseHandle(processHandle);
}
 
typedef LONG(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);
void ResumeProcess(DWORD process_id) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    NtResumeProcess pfnNtResumeProcess =
        (NtResumeProcess)GetProcAddress(GetModuleHandle(L"ntdll"), "NtResumeProcess");
    pfnNtResumeProcess(processHandle);
    CloseHandle(processHandle);
}
 
DWORD GetPId(WCHAR* szProcessName)
{
    HANDLE hProcessSnapShot = NULL;
    PROCESSENTRY32 pe32 = { 0 };
 
    hProcessSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
 
    if (hProcessSnapShot == (HANDLE)-1) return NULL;
 
    pe32.dwSize = sizeof(PROCESSENTRY32);
 
    if (Process32First(hProcessSnapShot, &pe32))
    {
        do {
            if (!wcscmp(szProcessName, pe32.szExeFile)) return pe32.th32ProcessID;
        } while (Process32Next(hProcessSnapShot, &pe32));
    }
    else
        ::CloseHandle(hProcessSnapShot);
 
    return NULL;
}
HWND g_hButtonTerminate;
HWND g_hButtonTakeOwnership;
HWND g_hWnd;
HWND g_hButtonSuspend;
HWND g_hButtonResume;
HWND g_hTextBox;
 
// 消息回调函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        if (reinterpret_cast<HWND>(lParam) == g_hButtonSuspend && HIWORD(wParam) == BN_CLICKED)
        {
            // 当“冻结”按钮被点击时执行的代码
            wchar_t buffer[256];
            GetWindowText(g_hTextBox, buffer, 256);
            std::wstring processName(buffer);
            DWORD pid = GetPId(buffer);
            if (pid != NULL) {
                // 获取要冻结进程的路径
                wchar_t processPath[MAX_PATH];
                HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                if (processHandle != NULL) {
                    if (GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH) > 0) {
                        // 获取所有权
                        std::wstring commandLine = L"cmd.exe /C takeown /F \"" + std::wstring(processPath) + L"\" /R /A /D Y";
                        size_t size = (commandLine.size() + 1) * sizeof(WCHAR);
                        char* mbstr = new char[size];
                        wcstombs_s(nullptr, mbstr, size, commandLine.c_str(), size);
                        system(mbstr);
                        delete[] mbstr;
                    }
                    CloseHandle(processHandle);
                }
                SuspendProcess(pid);
            }
            else {
                MessageBox(hwnd, L"未找到进程", L"错误", MB_OK | MB_ICONERROR);
            }
        }
        else if (reinterpret_cast<HWND>(lParam) == g_hButtonResume && HIWORD(wParam) == BN_CLICKED)
        {
            // 当“解冻”按钮被点击时执行的代码
            wchar_t buffer[256];
            GetWindowText(g_hTextBox, buffer, 256);
            std::wstring processName(buffer);
            DWORD pid = GetPId(buffer);
            if (pid != NULL) {
                ResumeProcess(pid);
            }
            else {
                MessageBox(hwnd, L"未找到进程", L"错误", MB_OK | MB_ICONERROR);
            }
        }
        else if (reinterpret_cast<HWND>(lParam) == g_hButtonTakeOwnership && HIWORD(wParam) == BN_CLICKED)
        {
            // 当“获取所有权”按钮被点击时执行的代码
            wchar_t buffer[256];
            GetWindowText(g_hTextBox, buffer, 256);
            std::wstring processName(buffer);
            DWORD pid = GetPId(buffer);
            if (pid != NULL) {
                wchar_t processPath[MAX_PATH];
                HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                if (processHandle != NULL) {
                    if (GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH) > 0) {
                        // 获取所有权
                        std::wstring commandLine = L"cmd.exe /C takeown /F \"" + std::wstring(processPath) + L"\" /R /A /D Y";
                        size_t size = (commandLine.size() + 1) * sizeof(WCHAR);
                        char* mbstr = new char[size];
                        wcstombs_s(nullptr, mbstr, size, commandLine.c_str(), size);
                        system(mbstr);
                        delete[] mbstr;
                    }
                    CloseHandle(processHandle);
                }
            }
            else {
                MessageBox(hwnd, L"未找到进程", L"错误", MB_OK | MB_ICONERROR);
            }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
 
    return 0;
}
 
 
// 主函数
int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // 注册窗口类
    const wchar_t CLASS_NAME[] = L"MyWindowClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
 
    // 创建窗口
    g_hWnd = CreateWindowEx(
        0,                              // 扩展窗口样式
        CLASS_NAME,                     // 窗口类名
        L"进程冻结器",                  // 窗口标题
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_EX_TOPMOST,            // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,   // 窗口位置
        410, 125,                        // 窗口大小
        NULL,                           // 父窗口句柄
        NULL,                           // 菜单句柄
        hInstance,                      // 实例句柄
        NULL                            // 创建参数
    );
 
    // 创建“进程名称”标签
    HWND hLabelProcessName = CreateWindow(
        L"STATIC",                      // 控件类名
        L"进程名：",                    // 控件文本
        WS_VISIBLE | WS_CHILD,          // 控件样式
        10, 10, 80, 20,                 // 控件位置和大小
        g_hWnd,                         // 父窗口句柄
        NULL,                           // 控件标识符
        hInstance,                      // 实例句柄
        NULL                            // 创建参数
    );
 
    // 创建“进程名称”文本框
    g_hTextBox = CreateWindow(
        L"EDIT",                        // 控件类名
        L"StudentMain.exe",             // 控件文本（初始为空）
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,        // 控件样式
        105, 10, 280, 20,               // 控件位置和大小
        g_hWnd,                         // 父窗口句柄
        NULL,                           // 控件标识符
        hInstance,                      // 实例句柄
        NULL                            // 创建参数
    );
 
    // 创建“冻结”按钮
    g_hButtonSuspend = CreateWindow(
        L"BUTTON",                      // 控件类名
        L"冻结",                        // 控件文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,    // 控件样式
        105, 40, 80, 30,                // 控件位置和大小
        g_hWnd,                         // 父窗口句柄
        NULL,                           // 控件标识符
        hInstance,                      // 实例句柄
        NULL                            // 创建参数
    );
 
    // 创建“解冻”按钮
    g_hButtonResume = CreateWindow(
        L"BUTTON",                      // 控件类名
        L"解冻",                    // 控件文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,    // 控件样式
        205, 40, 80, 30,               // 控件位置和大小
        g_hWnd,                         // 父窗口句柄
        NULL,                           // 控件标识符
        hInstance,                      // 实例句柄
        NULL                            // 创建参数
    );
    // 显示窗口
    ShowWindow(g_hWnd, nCmdShow);
    // 主消息循环
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
 
    return 0;
}