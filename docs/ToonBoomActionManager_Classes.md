# ToonBoomActionManager.dll Class Analysis

This document describes the reverse-engineered class structures from `ToonBoomActionManager.dll`.

**IDA Database:** `RE/ToonBoomActionManager.dll.i64`

## Overview

ToonBoomActionManager.dll provides the action/command management infrastructure for Toon Boom applications (Harmony Premium, Storyboard Pro). It implements:

- **Responder Chain**: First responder and selection responder management
- **Menu System**: XML-based menu creation and management
- **Toolbar System**: Customizable toolbars with dynamic item insertion
- **Shortcut Management**: Keyboard shortcut handling
- **Action Triggering**: Command execution and validation

## Class Hierarchy

```
AC_Manager (abstract interface)
└── AC_ManagerImpl (concrete, inherits QObject)

AC_Object (abstract interface)
├── AC_Item (abstract interface)
├── AC_Separator (abstract interface)
└── AC_Toolbar (abstract, also inherits AC_Container, AC_Help)
    └── AC_ToolbarImpl (concrete, inherits QToolBar)

AC_Container (abstract interface)
└── AC_ContainerImpl (concrete)

AC_Help (abstract interface)

AC_Responder (abstract interface)
└── AC_ResponderTemplate (concrete)
    └── AC_ResponderTemplateWidget<T> (template)
```

---

## AC_Manager / AC_ManagerImpl

### Purpose
Central manager for the action/command system. Handles responder registration, menu/toolbar creation, and action triggering.

### Key Addresses
| Symbol | Address |
|--------|---------|
| AC_Manager vtable | `0x18004e508` |
| AC_ManagerImpl vtable (QObject) | `0x18004e750` |
| AC_ManagerImpl vtable (AC_Manager) | `0x18004e7c8` |
| AC_ManagerImpl::AC_ManagerImpl | `0x18000ef20` |
| AC_ManagerImpl::~AC_ManagerImpl | `0x18000f300` |

### Memory Layout (AC_ManagerImpl)
Total size: **0x158 (344 bytes)** on x64

| Offset | Size | Type | Field |
|--------|------|------|-------|
| +0x00 | 0x10 | | QObject base |
| +0x10 | 0x08 | vptr | AC_Manager vtable |
| +0x18 | 0x08 | void* | m_keywords (shared ptr data) |
| +0x20 | 0x08 | void* | m_keywords (continued) |
| +0x28 | 0x08 | void* | m_keywords (continued) |
| +0x30 | 0x01 | bool | m_trimShortcuts (init value) |
| +0x38 | 0x18 | | Responder name→list tree |
| +0x50 | 0x18 | | Menu tree |
| +0x68 | 0x08 | ptr | m_shortcutManager |
| +0x78 | 0x18 | QString | m_hoverId |
| +0x90 | 0x18 | QString | m_genericImage |
| +0xA8 | 0x18 | QString | m_toolbarCustomizeImage |
| +0xC0 | 0x18 | | Toolbar tree |
| +0xD8 | 0x18 | vector | m_registeredResponders |
| +0xF0 | 0x18 | vector | m_responderStack |
| +0x108 | 0x08 | ptr | m_menuBar |
| +0x118 | 0x08 | ptr | m_applicationResponder |
| +0x120 | 0x08 | ptr | m_selectionResponder |
| +0x128 | 0x08 | ptr | m_itemGenerator |
| +0x130 | 0x01 | bool | m_option_trimShortcuts |

### Key Methods

#### Responder Management
- `firstResponder()` - Returns top of responder stack (offset +248)
- `applicationResponder()` - Returns m_applicationResponder (offset +296)
- `registerResponder(responder, widget)` - Adds responder to registry
- `setFirstResponder(responder)` - Sets active first responder
- `pushUp(responder)` / `pushOut(responder)` - Stack manipulation

#### Menu/Toolbar Creation
- `createMenuBar(element, parent)` - Create menu from XML
- `createToolbar(element, ids, mainWindow, area, name, owner)` - Create toolbar
- `loadMenus(path)` / `loadToolbars(path, ids)` - Load from file

#### Action Triggering
- `trigger(responderIdentity, actionName, forEach)` - Execute action
- `performValidation(responderIdentity, actionName, enabled, checked)` - Validate action state

### Signals
```cpp
void firstResponderChanged();
void selectionResponderChanged();
void updateToolbarsSignal();
void updateToolbarsText();
```

---

## AC_Toolbar / AC_ToolbarImpl

### Purpose
Customizable toolbar with support for:
- Dynamic item insertion from XML
- Placeholder-based customization
- Configuration save/restore
- Context-sensitive help

### Key Addresses
| Symbol | Address |
|--------|---------|
| AC_ToolbarImpl vtable (QObject) | `0x180054c90` |
| AC_ToolbarImpl vtable (QPaintDevice) | `0x180054e70` |
| AC_ToolbarImpl vtable (AC_Toolbar) | `0x180054eb0` |
| AC_ToolbarImpl vtable (AC_Container) | `0x180054ef8` |
| AC_ToolbarImpl vtable (AC_Help) | `0x1800550f8` |
| AC_ToolbarImpl::AC_ToolbarImpl | `0x180032df0` |
| AC_ToolbarImpl::~AC_ToolbarImpl | `0x180033080` |

### Memory Layout (AC_ToolbarImpl)
Total size: **0x118 (280 bytes)** on x64

| Offset | Size | Type | Field |
|--------|------|------|-------|
| +0x00 | 0x28 | | QToolBar base (includes QObject, QPaintDevice) |
| +0x28 | 0x58 | AC_ContainerImpl | m_container (embedded) |
| +0x80 | 0x08 | vptr | AC_Toolbar vtable |
| +0x88 | 0x08 | vptr | AC_Container vtable |
| +0x90 | 0x08 | vptr | AC_Help vtable |
| +0x98 | 0x18 | QString | m_responderIdentity |
| +0xB0 | 0x18 | vector | m_defaultConfig |
| +0xC8 | 0x01 | bool | m_isCustomizable |
| +0xD0 | 0x08 | ptr | m_customizeButton |
| +0xD8 | 0x08 | ptr | m_unknown |
| +0xE0 | 0x08 | QObject* | m_owner |
| +0xE8 | 0x10 | QWeakPointer | m_mainWindow |
| +0xF8 | 0x04 | float | m_scaleFactor (default 1.0) |
| +0x100 | 0x18 | QString | m_translationContext |

### Interface Pointers
When you have an `AC_Toolbar*`:
- `QToolBar*` = `(char*)this - 128`
- `AC_ContainerImpl*` = `(char*)this - 88`
- Identity QString is at `(char*)this - 72`

### Key Methods

#### Item Management
- `insert(beforeObject, element)` - Insert item from XML
- `insertAtPlaceholder(placeholder, element)` - Insert at named placeholder
- `insertSeparator(beforeObject)` - Insert separator
- `removeObject(identity)` - Remove item by identity
- `clear()` - Remove all items

#### Configuration
- `config()` - Get current item order as QList<QString>
- `setConfig(list)` - Restore from saved configuration
- `defaultConfig()` - Get default item order
- `isDefaultConfig()` - Check if current matches default

#### Validation
- `validateContent()` - Update enabled/checked states
- `validateContentIfVisible()` - Only validate if toolbar is shown
- `updateSeparators()` - Hide separators adjacent to hidden items

### Signals
```cpp
void customized(QString identity);
```

---

## AC_ContainerImpl

### Purpose
Base container implementation for managing AC_Object children.

### Memory Layout
Total size: **0x58 (88 bytes)** on x64

| Offset | Size | Type | Field |
|--------|------|------|-------|
| +0x00 | 0x08 | vptr | AC_ContainerImpl vtable |
| +0x08 | 0x08 | ptr | m_manager (AC_ManagerImpl*) |
| +0x10 | 0x18 | QString | m_identity |
| +0x28 | 0x01 | bool | m_enabled |
| +0x29 | 0x01 | bool | m_isToolbar |
| +0x30 | 0x18 | vector | m_objects |
| +0x48 | 0x10 | | Placeholder tree |

---

## Enumerations

### AC_Result
```cpp
enum class AC_Result : int {
    NotHandled = 0,  // Action not handled by any responder
    Handled = 1,     // Action handled successfully
    Error = 2        // Error during handling
};
```

### AC_ManagerOption
```cpp
enum class AC_ManagerOption : int {
    TrimShortcuts = 0  // Whether to trim whitespace from shortcuts
};
```

### AC_Help::cshHelpType
```cpp
enum cshHelpType {
    Type0 = 0,
    Type1 = 1,
    Type2 = 2,
    Type3 = 3,
    Type4 = 4,
    ToolbarHelp = 5
};
```

---

## XML Element Types

AC_ToolbarImpl::insert() handles these XML element types:

| Tag Name | Handler | Creates |
|----------|---------|---------|
| `item` | sub_1800363E0 | AC_ToolbarItemImpl |
| `menu` | sub_1800363E0 | AC_ToolbarItemImpl (menu button) |
| `multibutton` | sub_180037F30 | AC_ToolbarMultiButtonImpl |
| `combobox` | sub_1800395D0 | AC_ToolbarComboBox |
| `placeholder` | sub_18001B9F0 | Placeholder object |
| `separator` | sub_18003ABD0 | AC_ToolbarSeparatorImpl |

---

## Usage Patterns

### Getting AC_Manager Instance
```cpp
// Via PLUG_Services (from ToonBoomPlugInManager.dll)
AC_Manager* manager = PLUG_Services::getActionManager();
```

### Creating a Toolbar
```cpp
QList<QString> ids;
AC_Toolbar* toolbar = manager->createToolbar(
    xmlElement,        // QDomElement with toolbar definition
    &ids,              // Output: list of created item IDs
    mainWindow,        // QMainWindow to attach to
    Qt::TopToolBarArea,// Toolbar area
    "myToolbar",       // Object name
    owner              // Owner QObject for cleanup
);
```

### Triggering an Action
```cpp
AC_Result result = manager->trigger(
    "NetworkView",     // Responder identity
    "onActionZoomIn",  // Action/slot name
    false              // Don't iterate all responders
);
```

### Validating an Action
```cpp
bool enabled, checked;
manager->performValidation("NetworkView", "onActionZoomIn", &enabled, &checked);
```

---

## Related Files

- `framework/include/toon_boom/ac_manager.hpp` - AC_Manager header
- `framework/include/toon_boom/toolbar.hpp` - AC_Toolbar header
- `framework/include/toon_boom/PLUG_Services.hpp` - Service access

---

## Methodology

This analysis was performed using:
1. IDA Pro decompilation of ToonBoomActionManager.dll
2. Vtable analysis to identify class hierarchies
3. Constructor/destructor analysis for memory layout
4. Cross-reference analysis for method identification
5. String analysis for signal/slot names

All offsets verified against x64 Windows builds of Toon Boom Harmony Premium.
