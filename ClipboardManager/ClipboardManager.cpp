#include <Windows.h>
#include <iostream>
#include <string>
#include "CircularBuffer.h"
#define ENABLE_DEBUG 0

using namespace std;
void copyToClipboard(const wstring& tocopy);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
wstring getFromClipboard();

bool ignoreNextClipboard = false;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // Step 1: Register window class
    const wchar_t CLASS_NAME[] = L"ClipboardListenerWindow";
    CircularBuffer<wstring> clipboard(10);

    #if ENABLE_DEBUG

        AllocConsole();

        FILE* fp;
        _wfreopen_s(&fp, L"CONOUT$", L"w", stdout);
        _wfreopen_s(&fp, L"CONOUT$", L"w", stderr);
        _wfreopen_s(&fp, L"CONIN$", L"r", stdin);
        std::wcout << L"Clipboard listener started...\n";

    #endif


    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Step 2: Create invisible message-only window
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"", 0,
        0, 0, 0, 0,
        HWND_MESSAGE, nullptr, hInstance, &clipboard
    );

    if (!hwnd) return 1;

    for (int i = 0; i < 10; ++i) { //Adds hotkeys
        RegisterHotKey(hwnd, i, MOD_CONTROL | MOD_SHIFT, '0' + ((i + 1) % 10));
    }


    // Step 3: Register as clipboard listener
    AddClipboardFormatListener(hwnd);

    // Step 4: Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void copyToClipboard(const wstring& tocopy) {
    ignoreNextClipboard = true;

    if (!OpenClipboard(nullptr)) return;


    EmptyClipboard();
    size_t sizeInBytes = (tocopy.size() + 1) * sizeof(wchar_t);
    HGLOBAL HG = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
    if (!HG) {
        CloseClipboard();
    }
    else {
        memcpy(GlobalLock(HG), tocopy.c_str(), sizeInBytes);
        GlobalUnlock(HG);
        SetClipboardData(CF_UNICODETEXT, HG);
        CloseClipboard();
    }
}

wstring getFromClipboard() {
    if (!OpenClipboard(nullptr)) return L"";

    wstring input;
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        CloseClipboard();
        return input;
    }
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData) {
        LPCWSTR pText = static_cast<LPCWSTR>(GlobalLock(hData));
        if (pText) {
            input = pText;
            GlobalUnlock(hData);
        }
    }

    CloseClipboard();
    return input;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_NCCREATE) {
        // This runs once when the window is created
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        void* userData = cs->lpCreateParams;

        // Store your pointer in the window's user data slot
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userData));
        return TRUE; 
    }

    // Retrieve the pointer on later messages
    auto* clipboard = reinterpret_cast<CircularBuffer<wstring>*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (uMsg) {
    case WM_CLIPBOARDUPDATE: {
        if (ignoreNextClipboard) {
            ignoreNextClipboard = false;  // reset
            return 0;
        }
        std::wstring clipText = getFromClipboard();

        if (!clipText.empty()) {
            clipboard->addElement(clipText);
            #if ENABLE_DEBUG
                std::wcout << L"[Clipboard Update] " << clipText << std::endl;
            #endif
        }
        return 0;
    }

    case WM_DESTROY: {
        RemoveClipboardFormatListener(hwnd);
        PostQuitMessage(0);
        return 0;
    }
    case WM_HOTKEY: {
        int index = static_cast<int>(wParam);  // hotkey ID is index 0–9
        auto* clipboard = reinterpret_cast<CircularBuffer<wstring>*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (clipboard && index >= 0 && index < clipboard->size()) {
            int bufferIndex = (clipboard->size() - 1 - index + clipboard->size()) % clipboard->size();
            std::wstring entry = clipboard->getElement(bufferIndex);
            copyToClipboard(entry);

            #if ENABLE_DEBUG
                std::wcout << L"[PASTE HOTKEY] Ctrl+Shift+" << ((index + 1) % 10) << L" - " << entry << std::endl;
            #endif

            keybd_event(VK_CONTROL, 0, 0, 0);
            keybd_event('V', 0, 0, 0);
            keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        }
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}