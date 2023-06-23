#include <iostream>
#include <windows.h>

int main()
{
    const size_t size = 64;
    char* buffer = new char[size] { 0 };
    if (IsClipboardFormatAvailable(CF_TEXT))
    {
        if (OpenClipboard(NULL))
        {
            HANDLE hData = GetClipboardData(CF_TEXT);
            buffer = static_cast<char*>(GlobalLock(hData));
            std::cout << "Data in clipboard is " << buffer << std::endl;
            GlobalUnlock(hData);
            CloseClipboard();
        }
    }
    return 0;
}
