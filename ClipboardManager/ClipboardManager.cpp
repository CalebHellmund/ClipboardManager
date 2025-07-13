// ClipboardManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include "CircularBuffer.h"

using namespace std;

int main(){
    CircularBuffer<string> clipboard(2);
    clipboard.addElement("Hello ");
    clipboard.addElement("World");
    cout << clipboard.getElement(0) << clipboard.getElement(1) << endl;
    clipboard.addElement(", this works!");
    cout << clipboard.getElement(0) << clipboard.getElement(1) << endl;

    return 0;
}
