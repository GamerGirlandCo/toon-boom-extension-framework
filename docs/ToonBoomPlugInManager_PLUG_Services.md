# ToonBoomPlugInManager.dll - PLUG_Services Class Analysis

This document contains reverse engineering analysis of the `PLUG_Services` class hierarchy from `ToonBoomPlugInManager.dll` used in Toon Boom Harmony Premium and Storyboard Pro.

## Overview

The plugin services system in Toon Boom uses a singleton pattern to provide access to various application services. The main classes are:

- **PLUG_Services** - Static class providing access to service interfaces
- **PLUG_ServicesPrivate** - Extended static functionality with setters
- **PLUG_ServicesPrivateImpl** - (alias name, same as PLUG_ManagerImpl)
- **PLUG_ManagerImpl** - Main singleton implementation containing all service pointers
- **PLUG_Manager** - Base class for the manager

## Architecture

```
                    PLUG_Services (static class)
                           │
                           │ getters return from
                           ▼
    ┌──────────────────────────────────────────────┐
    │           PLUG_ManagerImpl (singleton)        │
    │   (inherits: QObject → PLUG_Manager)         │
    │                                              │
    │  +0x000: vftable (PLUG_ManagerImpl)          │
    │  +0x010: embedded interface object           │
    │  +0x188: QCoreApplication*                   │
    │  +0x190: unknown interface                   │
    │  +0x198-0x240: Service interface pointers    │
    │  +0x248: QString                             │
    │  +0x260: bool flag                           │
    └──────────────────────────────────────────────┘
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
    AC_Manager*     CM_Services*     PLUG_ScriptingInterface*
                        ...
```

## Global Variables

| Address | Name | Type | Description |
|---------|------|------|-------------|
| 0x180016590 | g_PLUG_ManagerImpl_instance | PLUG_ManagerImpl* | Main singleton instance |
| 0x180016570 | g_PLUG_ModuleLibraryInterface | PLUG_ModuleLibraryInterface* | Module library (separate global) |
| 0x180016578 | g_PLUG_SetupModeQueryInterface | PLUG_SetupModeQueryInterface* | Setup mode query interface |
| 0x180016580 | g_PLUG_PlaybackRange | PLUG_PlaybackRange | Static playback range instance |
| 0x180016538 | g_PLUG_InteractiveViewManagerImpl | PLUG_InteractiveViewManagerImpl* | Interactive view manager singleton |

---

## Class: PLUG_Services

### Purpose
Static utility class providing access to all plugin service interfaces. All methods are static.

### Public Static Methods

| Address | Method | Return Type | Offset in Singleton |
|---------|--------|-------------|---------------------|
| 0x180005a50 | getActionManager() | AC_Manager* | +0x1A0 (416) |
| 0x180005a70 | getColorManagerServices() | CM_Services* | +0x218 (536) |
| 0x180005a90 | getCurrentFrameInterface() | SC_CurrentFrameInterface* | +0x1B0 (432) |
| 0x180005ab0 | getDataToolInterface() | SC_CVDataToolInterface* | +0x220 (544) |
| 0x180005ad0 | getDragDropInterface() | PLUG_DragDropInterface* | +0x1F8 (504) |
| 0x180005af0 | getEditionStackInterface() | SC_SceneEditionStackInterface* | +0x1B8 (440) |
| 0x180005b10 | getExpressionScriptingInterface() | AT_ExprScriptEngine* | +0x200 (512)* |
| 0x180005b40 | getHttpAPI() | SC_HttpAPI* | +0x228 (552) |
| 0x180005b60 | getImportEngine() | PLUG_ImportEngine* | +0x1E0 (480) |
| 0x180005b80 | getInteractiveRenderManager() | SC_InteractiveRenderManagerInterface* | +0x198 (408) |
| 0x180005ba0 | getInteractiveViewManager() | PLUG_InteractiveViewManager* | +0x210 (528)** |
| 0x180005bf0 | getKeyStateInterface() | PLUG_KeyStateInterface* | +0x1E8 (488) |
| 0x180005c10 | getLayoutManager() | TULayoutManager* | +0x1F0 (496) |
| 0x180005c30 | getMenuService() | PLUG_MenuService* | +0x1D8 (472) |
| 0x180005c50 | getModuleLibraryInterface() | PLUG_ModuleLibraryInterface* | (separate global) |
| 0x180005c60 | getNetworkViewInterface() | SC_NetworkViewInterface* | +0x240 (576) |
| 0x180005c80 | getOGLRenderPlaybackInterface() | PLUG_OGLRenderPlaybackInterface* | +0x230 (560) |
| 0x180005d00 | getPluginPath(const QString&) | QString | via vtable |
| 0x180005d50 | getPreference() | PLUG_PreferenceUI* | (thunk to PLUG_PreferenceUIImpl::instance) |
| 0x180005d60 | getScriptingInterface() | PLUG_ScriptingInterface* | +0x1C8 (456) |
| 0x180005d80 | getSelection() | SL_Selection* | +0x1A8 (424) |
| 0x180005da0 | getSessionContext() | SC_SessionContext* | +0x1C0 (448) |
| 0x180005dc0 | getToolbarService() | PLUG_ToolbarService* | +0x1D0 (464) |
| 0x180005de0 | getVectorizationInterface() | PLUG_VectorizationInterface* | +0x238 (568) |
| 0x180005e00 | getWidgetFactoryRegistry() | PLUG_WidgetFactoryRegistry* | +0x208 (520) |

\* `getExpressionScriptingInterface()` requires `SC_SessionContext` at +0x1C0 to be non-null
\*\* `getInteractiveViewManager()` lazily creates `PLUG_InteractiveViewManagerImpl` if null

### Implementation Pattern

Most getters follow this pattern:
```cpp
Type* PLUG_Services::getXXX() {
    if (g_PLUG_ManagerImpl_instance)
        return g_PLUG_ManagerImpl_instance->m_xxx; // at specific offset
    return nullptr;  // returns 0 if singleton not initialized
}
```

---

## Class: PLUG_ServicesPrivate

### Purpose
Extended static class with additional functionality and setters (typically used internally).

### Public Static Methods

| Address | Method | Return Type | Description |
|---------|--------|-------------|-------------|
| 0x180005cb0 | getPluginBinFilePath(const QString&) | QString | Gets binary file path for plugin |
| 0x180005fa0 | isSetupMode() | bool | Queries setup mode via g_PLUG_SetupModeQueryInterface |
| 0x180007390 | setModuleLibraryInterface(PLUG_ModuleLibraryInterface*) | bool | Sets g_PLUG_ModuleLibraryInterface |
| 0x1800073f0 | setSetupModeQueryInterface(PLUG_SetupModeQueryInterface*) | void | Sets g_PLUG_SetupModeQueryInterface |
| 0x1800072D0 | setColorManagerServices(CM_Services*) | void | Sets +0x218 offset |
| 0x1800072F0 | setDataToolInterface(SC_CVDataToolInterface*) | void | Sets +0x220 offset |
| 0x180007330 | setHttpAPI(SC_HttpAPI*) | void | Sets +0x228 offset |
| 0x1800073B0 | setOGLRenderPlaybackInterface(PLUG_OGLRenderPlaybackInterface*) | void | Sets +0x230 offset |
| 0x180007410 | setVectorizationInterface(PLUG_VectorizationInterface*) | void | Sets +0x238 offset |
| 0x180007420 | setWidgetFactoryRegistry(PLUG_WidgetFactoryRegistry*) | void | Sets +0x208 offset |

---

## Class: PLUG_ManagerImpl

### Purpose
Main singleton implementation that holds all service interface pointers. Inherits from QObject and PLUG_Manager.

### Constructor
**Address**: `0x180004180`

**Signature**:
```cpp
PLUG_ManagerImpl(QObject* parent, UnknownInterface* iface1, UnknownInterface* iface2);
```

### Memory Layout (x64 MSVC)

| Offset | Hex | Size | Type | Member Name | Description |
|--------|-----|------|------|-------------|-------------|
| 0 | 0x000 | 8 | ptr | vftable | PLUG_ManagerImpl vtable |
| 8 | 0x008 | 8 | | (QObject data) | d_ptr from QObject |
| 16 | 0x010 | 8 | ptr | embedded_vftable | Embedded interface vtable |
| 24 | 0x018 | 72 | | (reserved) | Zeroed on construction |
| 96 | 0x060 | 304 | | (reserved block) | memset to 0, size 0x130 |
| 392 | 0x188 | 8 | ptr | m_coreApp | QCoreApplication* instance |
| 400 | 0x190 | 8 | ptr | m_unknownInterface | Constructor param a3 |
| 408 | 0x198 | 8 | ptr | m_interactiveRenderManager | SC_InteractiveRenderManagerInterface* |
| 416 | 0x1A0 | 8 | ptr | m_actionManager | AC_Manager* |
| 424 | 0x1A8 | 8 | ptr | m_selection | SL_Selection* |
| 432 | 0x1B0 | 8 | ptr | m_currentFrameInterface | SC_CurrentFrameInterface* |
| 440 | 0x1B8 | 8 | ptr | m_editionStackInterface | SC_SceneEditionStackInterface* |
| 448 | 0x1C0 | 8 | ptr | m_sessionContext | SC_SessionContext* |
| 456 | 0x1C8 | 8 | ptr | m_scriptingInterface | PLUG_ScriptingInterface* |
| 464 | 0x1D0 | 8 | ptr | m_toolbarService | PLUG_ToolbarService* |
| 472 | 0x1D8 | 8 | ptr | m_menuService | PLUG_MenuService* |
| 480 | 0x1E0 | 8 | ptr | m_importEngine | PLUG_ImportEngine* |
| 488 | 0x1E8 | 8 | ptr | m_keyStateInterface | PLUG_KeyStateInterface* |
| 496 | 0x1F0 | 8 | ptr | m_layoutManager | TULayoutManager* |
| 504 | 0x1F8 | 8 | ptr | m_dragDropInterface | PLUG_DragDropInterface* |
| 512 | 0x200 | 8 | ptr | m_exprScriptEngine | AT_ExprScriptEngine* |
| 520 | 0x208 | 8 | ptr | m_widgetFactoryRegistry | PLUG_WidgetFactoryRegistry* |
| 528 | 0x210 | 8 | ptr | m_interactiveViewManager | PLUG_InteractiveViewManager* |
| 536 | 0x218 | 8 | ptr | m_colorManagerServices | CM_Services* |
| 544 | 0x220 | 8 | ptr | m_dataToolInterface | SC_CVDataToolInterface* |
| 552 | 0x228 | 8 | ptr | m_httpAPI | SC_HttpAPI* |
| 560 | 0x230 | 8 | ptr | m_oglRenderPlaybackInterface | PLUG_OGLRenderPlaybackInterface* |
| 568 | 0x238 | 8 | ptr | m_vectorizationInterface | PLUG_VectorizationInterface* |
| 576 | 0x240 | 8 | ptr | m_networkViewInterface | SC_NetworkViewInterface* |
| 584 | 0x248 | 24 | QString | m_unknownString | QString member |
| 608 | 0x260 | 1 | bool | m_flag | Initialized to true |

**sizeof(PLUG_ManagerImpl) ≈ 0x268 (616 bytes minimum)**

---

## Class: PLUG_Manager

### Purpose
Base class for the manager singleton.

### Constructor
**Address**: `0x180001210`

### Memory Layout
| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 8 | ptr | vftable (PLUG_Manager) |

---

## Class: PLUG_ScriptingInterface

### Purpose
Interface for scripting functionality. Base class for script execution.

### Constructor
**Address**: `0x180009cd0`

### Memory Layout
| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 8 | ptr | vftable |

### Nested Struct: Program

Represents a script program to be executed.

**Memory Layout**:
| Offset | Size | Type | Member | Description |
|--------|------|------|--------|-------------|
| 0 | 24 | QString | path | Script path/name |
| 24 | 24 | QString | description | Script description |
| 48 | 24 | QString | content | Script content |
| 72 | 8+ | QDateTime | timestamp | Modification timestamp |

**sizeof(PLUG_ScriptingInterface::Program) ≈ 80+ bytes**

---

## Class: PLUG_InteractiveViewManager / PLUG_InteractiveViewManagerImpl

### Purpose
Manages interactive view delegates for drawing and tool handling.

### Constructor
**Address**: `0x180001ef0`

### Memory Layout
| Offset | Size | Type | Description |
|--------|------|------|-------------|
| 0 | 8 | ptr | vftable |
| 8 | 8 | ptr | m_delegateList (linked list) |
| 16 | 8 | ptr | m_delegate2 |

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x180003060 | instance() | Returns singleton from g_PLUG_InteractiveViewManagerImpl |
| 0x180003180 | registerDelegate(PLUG_InteractiveViewDelegate*) | Registers a view delegate |
| 0x180003320 | unregisterDelegate(PLUG_InteractiveViewDelegate*) | Unregisters a view delegate |
| 0x1800030a0 | isDelegateTypeRegistered(PLUG_InteractiveViewDelegate*) | Checks if delegate type exists |
| 0x180003070 | invalidateAllViews(int) | Invalidates all views |
| 0x180003080 | invalidateTimeline() | Invalidates timeline view |
| 0x180002e20 | handleMouseDown(...) | Handles mouse down events |
| 0x180002ee0 | handleMouseMove(...) | Handles mouse move events |
| 0x180002fa0 | handleMouseUp(...) | Handles mouse up events |
| 0x180002d60 | handleGetCursor(...) | Gets cursor for position |

---

## Class: PLUG_PreferenceUI / PLUG_PreferenceUIImpl

### Purpose
Manages preference panels and UI customization.

### Singleton Access
**Address**: `0x180007a20` (PLUG_PreferenceUIImpl::instance)

Uses thread-local storage for singleton initialization with `Init_thread_header`/`Init_thread_footer` pattern.

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x180007970 | addCustomizer(const PLUG_PreferenceUICustomizerInterface*) | Adds preference customizer |
| 0x180007aa0 | onCreateColorPreferencePanel(eAppContext, QWidget*) | Creates color pref panel |
| 0x180007b20 | onCreatePreferencePanel(eAppContext, QWidget*) | Creates general pref panel |

---

## Service Interface Types Reference

### Action & Menu System
- **AC_Manager** - Action/command manager
- **PLUG_MenuService** - Menu service interface
- **PLUG_ToolbarService** - Toolbar service interface

### Scene & Session
- **SC_SessionContext** - Current session context
- **SC_CurrentFrameInterface** - Current frame access
- **SC_SceneEditionStackInterface** - Scene edition stack
- **SC_NetworkViewInterface** - Network view interface
- **SC_HttpAPI** - HTTP API for server communication

### Drawing & Rendering
- **SC_InteractiveRenderManagerInterface** - Render management
- **SC_CVDataToolInterface** - Drawing data tool interface
- **CM_Services** - Color manager services
- **PLUG_VectorizationInterface** - Vectorization tools
- **PLUG_OGLRenderPlaybackInterface** - OpenGL render playback

### Selection & Layout
- **SL_Selection** - Selection management
- **TULayoutManager** - Layout system manager
- **PLUG_InteractiveViewManager** - Interactive view management

### Scripting
- **PLUG_ScriptingInterface** - Script execution interface
- **AT_ExprScriptEngine** - Expression script engine

### Import/Export
- **PLUG_ImportEngine** - Import functionality
- **PLUG_ModuleLibraryInterface** - Module library access

### Input & UI
- **PLUG_KeyStateInterface** - Keyboard state tracking
- **PLUG_DragDropInterface** - Drag and drop handling
- **PLUG_WidgetFactoryRegistry** - Widget factory registration
- **PLUG_PreferenceUI** - Preference panels

---

## Environment Variables

- **TOONBOOM_PLUGINPATH** - Overrides plugin search path (checked in `sub_180005380`)

---

## Analysis Methodology

1. **Function List Analysis**: Extracted all functions matching `*PLUG_Services*` pattern
2. **Decompilation**: Analyzed each getter method to determine offsets in singleton
3. **Constructor Analysis**: Traced `PLUG_ManagerImpl` constructor to map memory layout
4. **Cross-Reference Analysis**: Found setters and initialization points via xrefs to globals
5. **Pattern Recognition**: Identified common getter/setter patterns
6. **Global Identification**: Named and documented global singleton variables

## Database File
- **Source**: `RE/ToonBoomPlugInManager.dll.i64`
- **Module Base**: `0x180000000`
