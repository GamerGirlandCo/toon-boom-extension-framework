#pragma once
#include <MinHook.h>
#include <QtScript/QScriptEngine>

typedef QScriptEngine* (__stdcall *SCR_ScriptRuntime_getEngine_t)(void*);

typedef void* (__stdcall *SCR_ScriptManager_ctor_t)(void*, void*, void*);


QScriptEngine* SCR_ScriptRuntime_getEngine(void*);

typedef void (__stdcall *ScriptEngine_hook_t)(QScriptEngine*);

__declspec(dllexport) void Add_ScriptEngine_hook(ScriptEngine_hook_t hook);
__declspec(dllexport) BOOL hookInit();