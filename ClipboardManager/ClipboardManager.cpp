#include <Windows.h>
#include <iostream>
#include <string>
#include "CircularBuffer.h"

using namespace std;
void copyToClipboard(const wstring& tocopy);
wstring getFromClipboard();

int main(){
    wstring test = L"This also works";
    CircularBuffer<wstring> clipboard(2);
    clipboard.addElement(L"Hello ");
    clipboard.addElement(L"World");
    wcout << clipboard.getElement(0) << clipboard.getElement(1) << endl;
    clipboard.addElement(L", this works!");
    wcout << clipboard.getElement(0) << clipboard.getElement(1) << endl;
    copyToClipboard(test);
    wcout << getFromClipboard() << endl;
    return 0;
}
void copyToClipboard(const wstring& tocopy) {
    OpenClipboard(NULL);
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
    OpenClipboard(NULL);
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