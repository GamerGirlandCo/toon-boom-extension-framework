#pragma once

#include <cstdint>
#include <optional>

#include <windows.h>

namespace toon_boom_module::harmony {

// Returns the address of HarmonyPremium's internal helper:
//   QScriptEngine* SCR_ScriptRuntime_getEngine(SCR_ScriptRuntime* rt)
//
// This is resolved by scanning the target module's .text section for the exact
// machine-code bytes observed in IDA:
//   48 8B 01 48 8B 40 28 C3
//
// If the pattern is not found uniquely, returns std::nullopt.
std::optional<std::uintptr_t> find_SCR_ScriptRuntime_getEngine(HMODULE target_module);

// Returns the address of HarmonyPremium's SCR_ScriptManager constructor.
//
// Resolution strategy:
// - Scan .text for a unique mid-function sequence that:
//   - constructs QString("___scriptManager___") then calls defineGlobalQObject
//   - constructs QString("include") then calls defineGlobalFunction(QS_include)
//   - constructs QString("require") then calls defineGlobalFunction(QS_require)
// - Convert the match address to the containing function start using x64 unwind
//   metadata via RtlLookupFunctionEntry, and sanity-check the function size.
std::optional<std::uintptr_t> find_SCR_ScriptManager_ctor(HMODULE target_module);

}  // namespace toon_boom_module::harmony


