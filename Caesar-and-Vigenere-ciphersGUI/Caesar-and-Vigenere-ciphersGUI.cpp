#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <sstream>
#include <locale.h>
#include <commdlg.h>    // для диалогов выбора файлов
#include "Ciphers.hpp"

#define IDC_COMBO_CIPHER     1001
#define IDC_EDIT_TEXT        1002
#define IDC_EDIT_KEY         1003
#define IDC_BUTTON_ENCRYPT   1004
#define IDC_BUTTON_DECRYPT   1005
#define IDC_BUTTON_ENCRYPT_FILE 1006
#define IDC_BUTTON_DECRYPT_FILE 1007
#define IDC_EDIT_RESULT      1008
#define IDC_STATIC_KEYHINT   1009

HINSTANCE hInst;
HWND hComboCipher, hEditText, hEditKey, hBtnEncrypt, hBtnDecrypt;
HWND hBtnEncryptFile, hBtnDecryptFile, hEditResult, hKeyHint;
int g_currentCipher = 0;

CaesarCipher g_caesar(3);
VigenereCipher g_vigenere("RAF");

// ---------------------- Утилиты ----------------------
static std::string GetEditText(HWND hEdit)
{
    int len = GetWindowTextLengthA(hEdit);
    if (len <= 0) return "";
    std::string s(len, '\0');
    GetWindowTextA(hEdit, &s[0], len + 1);
    return s;
}

static void SetEditText(HWND hEdit, const std::string& s)
{
    SetWindowTextA(hEdit, s.c_str());
}

static void ShowMessage(LPCSTR text, LPCSTR title = "Ошибка")
{
    MessageBoxA(NULL, text, title, MB_OK | MB_ICONERROR);
}

static void UpdateKeyHint()
{
    std::ostringstream ss;
    if (g_currentCipher == 0)
        ss << "Ключ (по умолчанию: " << g_caesar.getKey() << ")";
    else
        ss << "Ключ (по умолчанию: " << g_vigenere.getKey() << ")";
    SetWindowTextA(hKeyHint, ss.str().c_str());
}

static bool GetFileName(HWND hwnd, std::string& path, bool save)
{
    char filename[MAX_PATH] = "";
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Текстовые файлы (*.txt)\0*.txt\0Все файлы (*.*)\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    BOOL res = save ? GetSaveFileNameA(&ofn) : GetOpenFileNameA(&ofn);
    if (!res) return false;
    path = filename;
    return true;
}

// ---------------------- Шифрование/дешифрование ----------------------
static void DoEncryptOrDecrypt(bool encrypt)
{
    std::string key = GetEditText(hEditKey);
    if (key.empty()) {
        if (g_currentCipher == 0) key = std::to_string(g_caesar.getKey());
        else key = g_vigenere.getKey();
    }

    std::string inputText;
    // при шифровании берем из ввода, при дешифровании из результата
    if (encrypt)
        inputText = GetEditText(hEditText);
    else
        inputText = GetEditText(hEditResult);

    if (inputText.empty()) {
        ShowMessage("Поле текста пустое!");
        return;
    }

    std::string outputText;
    try {
        if (g_currentCipher == 0) {
            g_caesar.setKey(std::stoi(key));
            outputText = encrypt ? g_caesar.encrypt(inputText) : g_caesar.decrypt(inputText);
        }
        else {
            g_vigenere.setKey(key);
            outputText = encrypt ? g_vigenere.encrypt(inputText) : g_vigenere.decrypt(inputText);
        }
    }
    catch (...) {
        ShowMessage("Ошибка при обработке текста или ключа.");
        return;
    }

    SetEditText(hEditResult, outputText);
}


static void DoFileEncryptOrDecrypt(HWND hwnd, bool encrypt)
{
    std::string inputFile, outputFile;
    if (!GetFileName(hwnd, inputFile, false)) return;
    if (!GetFileName(hwnd, outputFile, true)) return;

    std::string key = GetEditText(hEditKey);
    if (key.empty()) {
        if (g_currentCipher == 0) key = std::to_string(g_caesar.getKey());
        else key = g_vigenere.getKey();
    }

    bool success = false;
    try {
        if (g_currentCipher == 0) {
            g_caesar.setKey(std::stoi(key));
            success = encrypt ? g_caesar.encryptFile(inputFile, outputFile)
                : g_caesar.decryptFile(inputFile, outputFile);
        }
        else {
            g_vigenere.setKey(key);
            success = encrypt ? g_vigenere.encryptFile(inputFile, outputFile)
                : g_vigenere.decryptFile(inputFile, outputFile);
        }
    }
    catch (...) {
        success = false;
    }

    if (!success) ShowMessage("Ошибка при работе с файлом.");
}

// ---------------------- GUI ----------------------
static void CreateControls(HWND hWnd)
{
    hComboCipher = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        10, 10, 200, 200, hWnd, (HMENU)IDC_COMBO_CIPHER, hInst, NULL);
    SendMessageA(hComboCipher, CB_ADDSTRING, 0, (LPARAM)"Цезарь");
    SendMessageA(hComboCipher, CB_ADDSTRING, 0, (LPARAM)"Виженер");
    SendMessageA(hComboCipher, CB_SETCURSEL, 0, 0);

    CreateWindowA("STATIC", "Текст:", WS_CHILD | WS_VISIBLE, 10, 45, 100, 20, hWnd, NULL, hInst, NULL);
    hEditText = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        10, 65, 600, 24, hWnd, (HMENU)IDC_EDIT_TEXT, hInst, NULL);

    CreateWindowA("STATIC", "Ключ:", WS_CHILD | WS_VISIBLE, 10, 100, 40, 20, hWnd, NULL, hInst, NULL);
    hEditKey = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        50, 96, 150, 24, hWnd, (HMENU)IDC_EDIT_KEY, hInst, NULL);

    hKeyHint = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 210, 100, 400, 20, hWnd, (HMENU)IDC_STATIC_KEYHINT, hInst, NULL);

    hBtnEncrypt = CreateWindowA("BUTTON", "Зашифровать", WS_CHILD | WS_VISIBLE,
        10, 130, 120, 28, hWnd, (HMENU)IDC_BUTTON_ENCRYPT, hInst, NULL);
    hBtnDecrypt = CreateWindowA("BUTTON", "Расшифровать", WS_CHILD | WS_VISIBLE,
        140, 130, 120, 28, hWnd, (HMENU)IDC_BUTTON_DECRYPT, hInst, NULL);

    hBtnEncryptFile = CreateWindowA("BUTTON", "Зашифровать файл", WS_CHILD | WS_VISIBLE,
        270, 130, 140, 28, hWnd, (HMENU)IDC_BUTTON_ENCRYPT_FILE, hInst, NULL);
    hBtnDecryptFile = CreateWindowA("BUTTON", "Расшифровать файл", WS_CHILD | WS_VISIBLE,
        420, 130, 140, 28, hWnd, (HMENU)IDC_BUTTON_DECRYPT_FILE, hInst, NULL);

    CreateWindowA("STATIC", "Результат:", WS_CHILD | WS_VISIBLE, 10, 170, 100, 20, hWnd, NULL, hInst, NULL);
    hEditResult = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE |
        ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 10, 190, 600, 220, hWnd, (HMENU)IDC_EDIT_RESULT, hInst, NULL);

    UpdateKeyHint();
}

// ---------------------- WinAPI ----------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
    {
        WORD id = LOWORD(wParam), code = HIWORD(wParam);
        if (id == IDC_COMBO_CIPHER && code == CBN_SELCHANGE) {
            g_currentCipher = (int)SendMessageA(hComboCipher, CB_GETCURSEL, 0, 0);
            UpdateKeyHint();
        }
        else if (id == IDC_BUTTON_ENCRYPT && code == BN_CLICKED)
            DoEncryptOrDecrypt(true);
        else if (id == IDC_BUTTON_DECRYPT && code == BN_CLICKED)
            DoEncryptOrDecrypt(false);
        else if (id == IDC_BUTTON_ENCRYPT_FILE && code == BN_CLICKED)
            DoFileEncryptOrDecrypt(hWnd, true);
        else if (id == IDC_BUTTON_DECRYPT_FILE && code == BN_CLICKED)
            DoFileEncryptOrDecrypt(hWnd, false);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXA wcex = { sizeof(WNDCLASSEXA) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = "CaesarVigenereClass";
    return RegisterClassExA(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowA("CaesarVigenereClass", "Шифры: Цезарь и Виженер",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 640, 470,
        NULL, NULL, hInstance, NULL);

    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    CreateControls(hWnd);
    return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
