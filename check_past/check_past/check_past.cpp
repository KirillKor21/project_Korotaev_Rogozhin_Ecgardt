#include <Windows.h>
#include <iostream>
#include <openssl/evp.h>
#include <string>
#include <sstream>

int cript_error(void)
{
    return 1;
}

int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key,
    unsigned char* iv, unsigned char* decryptext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int decryptext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        cript_error();


    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        cript_error();


    if (1 != EVP_DecryptUpdate(ctx, decryptext, &len, ciphertext, ciphertext_len))
        cript_error();
    decryptext_len = len;


    if (1 != EVP_DecryptFinal_ex(ctx, decryptext + len, &len))
        cript_error();
    decryptext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return decryptext_len;
}


void addToClipboard(const char* text)
{
    // открываем буфер обмена
    if (!OpenClipboard(NULL))
    {
        // обработка ошибки
        return;
    }

    // очищаем буфер обмена
    EmptyClipboard();

    // выделяем память для текста
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(text) + 1);
    if (hMem == NULL)
    {
        // обработка ошибки
        CloseClipboard();
        return;
    }

    // копируем текст в выделенную память
    char* pMem = (char*)GlobalLock(hMem);
    if (pMem == NULL)
    {
        // обработка ошибки
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }
    strcpy_s(pMem, strlen(text) + 1, text);
    GlobalUnlock(hMem);

    // устанавливаем новое содержимое буфера обмена
    SetClipboardData(CF_TEXT, hMem);

    // закрываем буфер обмена
    CloseClipboard();
}

int main()
{
    //Запуск в фоновом режиме
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    const size_t size = 128;
    char* buffer = new char[size] { 0 };

    //Копирование данных из буфера обмена
    if (IsClipboardFormatAvailable(CF_TEXT))
    {
        if (OpenClipboard(NULL))
        {
            HANDLE hData = GetClipboardData(CF_TEXT);
            buffer = static_cast<char*>(GlobalLock(hData));
            std::cout << "***Data in clipboard is " << buffer << std::endl;
            GlobalUnlock(hData);
            CloseClipboard();
        }
    }

    //Перевод из строки HEX в байты
    //std::string hex_string = "176aa39d3eacd1585ae2212b277b33473f90b80027486559fb18a112f6688195";
    std::string hex_string = buffer;
    std::stringstream ss(hex_string); // создаем поток для чтения из строки
    std::string byte_string; // строка для хранения байтов

    // читаем по два символа и преобразуем их в байты
    for (int i = 0; i < hex_string.length(); i += 2) {
        std::string byte_hex = hex_string.substr(i, 2); // читаем два символа
        char byte = (char)strtol(byte_hex.c_str(), NULL, 16); // преобразуем в байт
        byte_string.push_back(byte); // добавляем байт в строку
    }

    
    unsigned char ciphertext[size];
    int ciphertext_len = byte_string.length();


    unsigned char decryptedtext[size];
    int decryptedtext_len;

    //Ключ
    unsigned char key[] = { 0x63, 0x64, 0x61, 0x65, 0x62, 0x31, 0x32, 0x38,
                            0x32, 0x64, 0x36, 0x31, 0x34, 0x37, 0x37, 0x32,
                            0x62, 0x65, 0x62, 0x31, 0x65, 0x37, 0x34, 0x63,
                            0x31, 0x39, 0x32, 0x62, 0x65, 0x62, 0x64, 0x61, };

    //Вектор
    unsigned char iv[]  = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                          0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
    };


    //копируем в буфер, который будем расшифровывать
    strcpy_s((char*)ciphertext, size, (char*)byte_string.c_str());

    //Расшифровка
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,decryptedtext);
    decryptedtext[decryptedtext_len] = '\0';

    printf("Decrypted text is:\n");
    printf("%s\n", decryptedtext);

    //Ожидание нажатия клавиш Ctrl + V
    std::cout << "***Waiting signal Ctrl + V... " << std::endl;

    bool status = true;
    while (status)
    {

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(0x56) & 0x8000) {

            std::cout << "***Signal Ctrl + V !" << std::endl;

            //Копируем расшифрованный текст в массив
            strcpy_s(buffer, size, (char*)decryptedtext);

            std::cout << "***Buffer is " << buffer << std::endl;

            //Запись в буфер обмена 
            addToClipboard(buffer);

            status = false;

            exit(0);

        }

    }


    return 0;
}