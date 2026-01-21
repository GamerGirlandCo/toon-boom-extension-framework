#include "include/lib_all.hpp"
#include <hooks/toon_boom_hooks.hpp>
#include <iostream>

extern bool is_first_load;

void AddExamples(QScriptEngine *engine) {
  std::cout << "AddExamples" << std::endl;
  auto examples = new ToonBoomExamples();
  engine->globalObject().setProperty("extensionExamples",
                                     examples->getExamples(engine));
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
                                                       DWORD ul_reason_for_call,
                                                       LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
    bool was_first_load = is_first_load;
    if (was_first_load) {
      // Sleep(20000);
      Add_ScriptEngine_hook(&AddExamples);
    }
    if (hookInit() != TRUE) {
      std::cerr << "Failed to initialize hooks" << std::endl;
      MessageBoxA(NULL, "Failed to initialize hooks", "Error",
                  MB_ICONERROR | MB_OK);
      return FALSE;
    }
    if (was_first_load) {
      MessageBoxA(NULL, "Hooks initialized!!!", "Congratulations!!!",
                  MB_ICONINFORMATION | MB_OK);
    }
    break;
  }
  return TRUE;
}