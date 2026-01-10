# HarmonyPremium: QtScript / `QScriptEngine` integration

This document captures the current understanding of Harmony Premium's QtScript integration as observed in `RE/HarmonyPremium.exe.i64`.

## Key takeaways (high signal)

- Harmony has an internal `QObject` wrapper around the scripting runtime named **`SCR_ScriptEngineImpl`**.
- A higher-level manager **`SCR_ScriptManager`** constructs `SCR_ScriptEngineImpl`, registers a large set of interfaces (via `SCR_Scripting_registerInterfaces` at `0x140914850`), then injects itself into script as a global object named **`"___scriptManager___"`**.
- Two global native functions are installed into the script global object:
  - **`include(filename)`** → forwards to a virtual method on the script/scene manager interface (and tries to execute the included file in the caller's scope by temporarily adopting the parent context's activation/`this` objects).
  - **`require(filename)`** → resolves/remaps the path, loads the file, wraps it in a JavaScript closure that returns `exports`, evaluates it, and returns the `exports` object (or returns the uncaught exception).
- There are small helper routines used throughout Harmony to bind C++ into the scripting global object:
  - bind `QObject*` via `QScriptEngine::newQObject(...)` then `QScriptValue::setProperty(...)`
  - bind native callbacks via `QScriptEngine::newFunction(...)` then `QScriptValue::setProperty(...)`

## Main components

### `SCR_ScriptEngineImpl`

Observed in `SCR_ScriptEngineImpl_ctor` (`0x14082AEC0`).

What it does:

- Is a `QObject` subclass (has `qt_metacast`, metaobject data, etc).
- Allocates an internal `QScriptEngine` and stores it in the object (other methods access it via a field at **byte offset `+40`** from `this`).
- Installs a custom `QScriptEngineAgent` (`SCR_ScriptEngineImpl::ScriptAgent`) and sets the engine's process-events interval from preference key `"SCR_EVENT_PROCESSING_INTERVAL"`.
- Maintains a recursion depth counter (checked against **`12`**) used to prevent runaway recursion / re-entrancy during evaluation and calls.

Evaluation-related methods (renamed in the IDA DB during this session):

- `SCR_ScriptEngineImpl_callWithTempGlobal` (`0x14082B330`)
  - Temporarily swaps the engine global object (`setGlobalObject`), calls a `QScriptValue` callable (`QScriptValue::call`), restores the old global, and maps uncaught exceptions into the return value.
- `SCR_ScriptEngineImpl_evalInNewGlobal` (`0x14082BEB0`)
  - Creates a fresh global object (`newObject`), sets its prototype to a provided prototype or to the previous global, swaps it in via `setGlobalObject`, evaluates a `QString` script body, restores the previous global.
  - On unwind: triggers `collectGarbage()` when leaving the outermost evaluation.
- `SCR_ScriptEngineImpl_evalFile` (`0x14082C590`)
  - Reads a script file as UTF-8 and evaluates it.
  - Writes `__file__` into the current scope chain before evaluation (see helper below).
  - Uses `SCR_ScriptEngineImpl_evalInNewGlobal` when not already nested; otherwise evaluates directly in the existing global object.

### `SCR_ScriptManager`

Observed in `SCR_ScriptManager_ctor` (`0x14081FD60`).

What it does:

- Constructs `SCR_ScriptEngineImpl`.
- Calls a **large** interface registration routine (`SCR_Scripting_registerInterfaces`, `0x140914850`) which populates the scripting environment with many bindings.
- Injects itself into script as **global property** `"___scriptManager___"`:
  - Done via `SCR_ScriptRuntime_defineGlobalQObject` (`0x14082CB50`)
- Installs native global functions:
  - `"include"` bound to `QS_include` (`0x1408246C0`)
  - `"require"` bound to `QS_require` (`0x140827D60`)
  - Done via `SCR_ScriptRuntime_defineGlobalFunction` (`0x14082CAC0`)

## Builtins: `include()` / `require()`

### `include(filename)` (`QS_include`, `0x1408246C0`)

Behavior (from decompilation):

- Validates `argumentCount == 1` and that the argument is a string.
- Retrieves the global `"___scriptManager___"` object, converts it to a `QObject*`, and `QMetaObject::cast`s it to the expected interface type (error message says “scene manager interface”).
- Calls a virtual function at vtable offset `+88` on that interface, passing the filename string.
- If a parent script context exists, it temporarily sets the current context's activation object and this-object to the parent context's ones while doing the include, then restores them.

### `require(filename)` (`QS_require`, `0x140827D60`)

Behavior (from decompilation):

- Validates `argumentCount == 1` and that the argument is a string.
- Retrieves/casts global `"___scriptManager___"` as above.
- Calls a virtual function at vtable offset `+96` to resolve the requested module path, then applies an OS remap (`oswRemapPath::RemapPath2`).
- If the resolved path is a directory, appends `"/index.js"`.
- Reads the file and wraps it into a JS closure of the form:

```javascript
(function()
{
  var exports = {};
  var __file__ = "<resolved path>";
  // file contents
  return exports;
})
```

- Evaluates the wrapper; if it yields a function, calls it and returns the resulting `exports`.
- If the engine has an uncaught exception, returns the uncaught exception value.

## Helper routines used for global bindings

### Define global `QObject` (`SCR_ScriptRuntime_defineGlobalQObject`, `0x14082CB50`)

- Gets `engine->globalObject()`
- Wraps a `QObject*` using `QScriptEngine::newQObject(...)`
- Writes it into the global object with `QScriptValue::setProperty(...)`
  - The property flags argument is passed as `2048` in the decompiler output.

### Define global native function (`SCR_ScriptRuntime_defineGlobalFunction`, `0x14082CAC0`)

- Gets `engine->globalObject()`
- Creates a `QScriptValue` function using `QScriptEngine::newFunction(callback)`
- Writes it into the global object via `setProperty(..., flags=2048)`

### Set `__file__` in scope chain (`SCR_ScriptRuntime_setScopeFileVar`, `0x14082CCB0`)

- Fetches `QScriptEngine::currentContext()->scopeChain()`
- Sets `scopeChain()[0].__file__ = <path>` (property flags passed as `2048`)

### Accessors used throughout registration

Several small helpers reveal how Harmony threads the live `QScriptEngine*` and other host pointers through its scripting subsystem. In decompiler output these are simple pointer-chasing accessors:

- `SCR_ScriptRuntime_getEngine` (`0x14082BCD0`)
  - Returns `*(*a1 + 40)` (i.e., reads a `QScriptEngine*` stored at **byte offset `+40`** from the underlying script-engine wrapper object).
- `SCR_ScriptRuntime_getHostContext` (`0x14082CCA0`)
  - Returns `*(*a1 + 24)` (a host/session/context pointer used to initialize many interface objects via a virtual method at vtable offset `+88`).
- `SCR_ScriptEngineImpl_setSceneInterface` (`0x14082CDA0`)
  - Writes to `(*a1)+32`. Called from `SCR_ScriptManager_ctor` after interface registration; in the observed init flow it stores the singleton-like `"scene"` interface pointer (also injected as global `Scene`/`scene`).

## Why this matters for our injection goal

The above shows Harmony already has an established pattern for binding objects into the script global object (via `newQObject` + `setProperty`). The cleanest "natural" hook points to attach our own module object are:

- Right after `SCR_ScriptManager_ctor` injects `"___scriptManager___"` and registers `include/require`, or
- Anywhere we can get a pointer to the live `QScriptEngine` used for scripting (e.g. from a `SCR_ScriptEngineImpl` instance; other methods access it via the field at byte offset `+40`).

Next step: dig deeper into `SCR_Scripting_registerInterfaces` to map each global binding back to its concrete C++ type and owning subsystem (useful for choosing robust runtime hook points).

## Global namespace installed by Harmony

This section summarizes what `SCR_Scripting_registerInterfaces` (`0x140914850`) and `SCR_Scripting_registerUiTypes` (`0x140913920`) install into the engine.

### `SCR_Scripting_registerInterfaces` (`0x140914850`): core global objects

Direct `globalObject().setProperty(<name>, newQObject(...), flags=2048)` calls are visible in the decompilation for (non-exhaustive, but high-confidence) global names:

- `Scene` (capitalized; created from a singleton-like object named `"scene"`)
- `specialFolders`
- `exports` (set to a fresh script object created via `QScriptEngine::newObject`)
- `about`
- `Action` (registered at least once; later may be rebound if an action manager exists)
- `Drawing`
- `element`
- `fileMapper`
- `KeyModifiers`
- `MessageLog`
- `preferences`
- `scene` (lowercase; appears to alias the same underlying object as `Scene`)
- `System`
- `DrawingTools`
- `TimelineMarker`
- `Settings` (an instance of `SCR_SettingsInterface`)
- `column`
- `node`
- `selection`
- `PaletteObjectManager`
- `frame` (only when `WHO_Identity::family() != 4`)
- `compositionOrder` (backed by an object constructed with name `"composition"`)
- `copyPaste`
- `exporter`
- `func`
- `MovieImport`
- `render`
- `waypoint`
- `Backdrop`
- `sound`
- `stateUtil` (backed by an object constructed with name `"TB_StateUtil"`)

Also observed inside this function:

- Multiple `QMetaType` custom conversions registered via `QScriptEngine::registerCustomType(...)` (exact C++ types currently appear as `unk_140F7....` in the decompiler output).
- Additional interface initializers called (e.g. `SCR_DrawingKey::registerInterface(engine)`), plus many helper routines like `sub_14096.../sub_14097...` that likely register more types/enums.

### `SCR_Scripting_registerUiTypes` (`0x140913920`): UI/widget constructors and helpers

This function installs a number of `QMetaObject`-backed constructors into the global object using `newFunction(...)` + `newQMetaObject(...)` + `setProperty(..., flags=2048)`. Observed names include:

- `Dialog`
- `Label`
- `Button`
- `LineEdit`
- `NumberEdit`
- `DateEdit`
- `TimeEdit`
- `TextEdit`
- `SpinBox`
- `CheckBox`
- `RadioButton`
- `ComboBox`
- `GroupBox`
- `Slider`
- `ImportDrawingDlg` (conditional: `WHO_Identity::family() != 4`)
- `ExportVideoDlg` (conditional: `WHO_Identity::family() != 4`)

It also creates a `MessageBox` object with methods:

- `MessageBox.warning`
- `MessageBox.information`
- `MessageBox.critical`

And it registers several additional script-visible helpers:

- `DateEditEnum`
- `FileAccess`
- `DirSpec`
- `FileDialog` (conditional on an `a2` parameter in the decompiler output)
- `Input` (conditional on an `a2` parameter in the decompiler output)


