#include "./include/util.hpp"
#include <toon_boom/ext/util.hpp>
using namespace util;
void sendEscapeKeyToWindow(HWND hwnd) {
  {
    SetForegroundWindow(hwnd);
		INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_ESCAPE;
   
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_ESCAPE;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    
		debug::out << "SendInput: " << uSent << std::endl;
    
  }
}