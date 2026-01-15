#include "../include/public/hooks/toon_boom_hooks.hpp"
#include "../include/internal/harmony_signatures.hpp"
#include <iostream>
#include <vector>

QScriptEngine *global_engine_ptr = NULL;
bool is_first_load = true;
SCR_ScriptManager_ctor_t SCR_ScriptManager_ctor_original_ptr = NULL;
std::vector<ScriptEngine_hook_t> script_engine_hooks;

void *SCR_ScriptManager_ctor_hook(void *_this, void *_engine, void *_parent) {
  std::cout << "SCR_ScriptManager_ctor_hook" << std::endl;
	void *result = SCR_ScriptManager_ctor_original_ptr(_this, _engine, _parent);
	HMODULE target_module = GetModuleHandle(NULL);
	std::optional<std::uintptr_t> SCR_ScripRuntime_getEngine_original =
      toon_boom_module::harmony::find_SCR_ScriptRuntime_getEngine(
          target_module);
  if (SCR_ScripRuntime_getEngine_original == std::nullopt) {
    std::cerr << "Failed to find SCR_ScriptRuntime_getEngine" << std::endl;
    return result;
  }
  auto SCR_ScripRuntime_getEngine_original_ptr = reinterpret_cast<SCR_ScriptRuntime_getEngine_t>(SCR_ScripRuntime_getEngine_original.value());

	void* mgr_data = *reinterpret_cast<void**>(reinterpret_cast<std::byte*>(_this) + 0x20);
	if (!mgr_data) {
    std::cerr << "SCR_ScriptManager data pointer was null" << std::endl;
    return result;
  }
  void* runtime_handle = *reinterpret_cast<void**>(mgr_data);
  if (!runtime_handle) {
    std::cerr << "SCR_ScriptManager runtime handle was null" << std::endl;
    return result;
  }
  QScriptEngine* engine = SCR_ScripRuntime_getEngine_original_ptr(runtime_handle);
  if (!engine) {
    std::cerr << "SCR_ScriptRuntime_getEngine returned null" << std::endl;
    return result;
  }
  global_engine_ptr = engine;
  for(auto hook : script_engine_hooks) {
    hook(engine);
  }
  return result;
}

void Add_ScriptEngine_hook(ScriptEngine_hook_t hook) {
  script_engine_hooks.push_back(hook);
}

BOOL hookInit() {
	if(!is_first_load) {
		return TRUE;
	}
	if(MH_Initialize() != MH_OK) {
		std::cerr << "Failed to initialize MinHook" << std::endl;
		return FALSE;
	}
	auto scr_ScriptManager_ctor_ptr = toon_boom_module::harmony::find_SCR_ScriptManager_ctor(GetModuleHandle(NULL));
	if(scr_ScriptManager_ctor_ptr == std::nullopt) {
		std::cerr << "Failed to find SCR_ScriptManager_ctor" << std::endl;
		return FALSE;
	}
	auto SCR_ScriptManager_ctor_original_ptr_val = reinterpret_cast<SCR_ScriptManager_ctor_t>(scr_ScriptManager_ctor_ptr.value());
	MH_STATUS status = MH_CreateHook(
		reinterpret_cast<LPVOID>(SCR_ScriptManager_ctor_original_ptr_val),
		reinterpret_cast<LPVOID>(&SCR_ScriptManager_ctor_hook),
		reinterpret_cast<LPVOID *>(&SCR_ScriptManager_ctor_original_ptr));
	if(status != MH_OK) {
		std::cerr << "Failed to create hook for SCR_ScriptManager_ctor" << std::endl;
		return FALSE;
	}
	status = MH_EnableHook(MH_ALL_HOOKS);
	if(status != MH_OK) {
		std::cerr << "Failed to enable hooks" << std::endl;
		MH_RemoveHook(reinterpret_cast<LPVOID>(SCR_ScriptManager_ctor_original_ptr_val));
		MH_Uninitialize();
		return FALSE;
	}
	std::cout << "Hooks initialized and enabled" << std::endl;
	is_first_load = false;
	return TRUE;
}
