#include <Windows.h>
#include <iostream>

STARTUPINFO si;
PROCESS_INFORMATION pi;
HWND g_hWndNextViewer = NULL;
bool first_into = true;

// Обработчик сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DRAWCLIPBOARD:
        if (first_into) {
            first_into = false;
            SendMessage(g_hWndNextViewer, message, wParam, lParam);
            break;
        }
        else {

            std::cout << "***Clipboard content changed" << std::endl;
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            SendMessage(g_hWndNextViewer, message, wParam, lParam);
            CloseClipboard();
            Sleep(3000);
            exit(0);
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int main()
{

    //Создание процесса отслеживания вставки из буфера
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[] = L"C:/Users/pro10/source/repos/check_past/x64/Debug/check_past.exe";

    if (CreateProcess(cmdLine, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        std::cout << "Process id created !" << std::endl;
        std::cout << "Pid = " << std::dec << pi.dwProcessId << std::endl;
    }
    else {
        std::cout << "Process is not created! unlucky :( " << std::endl;
    }

    // Регистрация класса окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = (LPCWSTR)"MyWindowClass";
    RegisterClass(&wc);

    // Создание окна
    HWND hWnd = CreateWindow((LPCWSTR)"MyWindowClass", NULL, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    // Регистрация окна как просмотрищика буфера обмена
    g_hWndNextViewer = SetClipboardViewer(hWnd);

    // Запуск цикла обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //удаление окна и цепочки просмотрищиков буфера обмена
    ChangeClipboardChain(hWnd, g_hWndNextViewer);

    //Отменяем регистрацию класса окна
    UnregisterClass((LPCWSTR)"MyWindowClass", GetModuleHandle(NULL));

    return 0;
}