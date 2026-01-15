# TULayoutView Toolbar Integration in Toon Boom

This document explains how toolbars are created, managed, and displayed within TULayoutView instances in Toon Boom Harmony Premium and Storyboard Pro.

**IDA Databases:**
- `RE/HarmonyPremium.exe.i64` - Main application
- `RE/ToonBoomActionManager.dll.i64` - AC_Toolbar and AC_Manager implementations
- `RE/ToonBoomLayout.dll.i64` - TULayoutView and TULayoutFrame implementations

## Overview

Toon Boom uses a two-tier toolbar system:

1. **Global Toolbars** - Application-wide toolbars (FileToolbar, EditToolbar, DrawingToolToolbar, etc.)
2. **View-Specific Toolbars** - Toolbars that appear when a specific view has focus (DrawingViewToolbar, TimelineViewToolbar, CameraViewToolbar, etc.)

## Toolbar Buttons Disabled?

If your custom view’s toolbar shows but every button is disabled, the root cause is almost always **validation / responder resolution**, not the toolbar XML.

See `docs/AC_Toolbar_ButtonEnablement.md:1` for the IDA-verified validation flow and why `enabled="true"` is ignored.

## Important: raiseArea vs Toolbar Display

**`TULayoutManager::raiseArea()` does NOT directly display toolbars.**

`raiseArea` is used to bring a view/area to the foreground:
```cpp
// Signature:
TULayoutView* raiseArea(const QString& areaName, TULayoutFrame* frame, 
                        bool createNew, const QPoint& pos);

// Example usage (from HarmonyPremium.exe):
layoutManager->raiseArea("Colour", nullptr, true, QPoint(0,0));  // Shows Colour palette
layoutManager->raiseArea("Morphing", nullptr, true, QPoint(0,0)); // Shows Morphing view
```

**Toolbar display is triggered separately** through:
1. `TULayoutFrame::setCurrentTab(...)` (tab/view change)
2. `TULayoutFrame::showViewToolBar()` (updates the view-toolbar for the new current view)
3. `view->toolbar()` (returns toolbar definition as a `QDomElement`)

`TULayoutManager::setCurrentLayoutFrame(frame)` does **not** call `showViewToolBar()` directly (verified in ToonBoomLayout.dll `0x7ffa0be541d0`).

If you call `raiseArea` and don't see toolbars, the likely causes are:
- Your view's `toolbar()` override returns an empty `QDomElement`
- The toolbar XML hasn't been registered with `AC_Manager`
- The view isn't properly receiving focus after being raised
- `LAY_ToolbarInfo::m_isDefault` is still non-zero (the frame hides/resets the view-toolbar in this case)

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      TULayoutManager                             │
│  - Manages all frames, areas, and global toolbars               │
│  - showToolbar() at vtable+416 creates/shows toolbars by name   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                       TULayoutFrame                              │
│  - Contains TULayoutMainWindow for toolbar docking              │
│  - showViewToolBar() displays view-specific toolbar             │
│  - Stores LAY_ToolbarInfo per view for configuration            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                     TULayoutViewHolder                           │
│  - Contains 1-2 TULayoutView instances                          │
│  - Notifies frame when active view changes                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                       TULayoutView                               │
│  - Abstract base class for all views                            │
│  - toolbar() virtual method returns QDomElement                 │
│  - setToolbarInfo() receives LAY_ToolbarInfo                    │
└─────────────────────────────────────────────────────────────────┘
```

## Toolbar Definition and Creation Flow

### 1. Toolbar XML Loading

During application startup, toolbars are loaded from `toolbars.xml`:

```cpp
// From HarmonyPremium.exe session initialization (0x140034EB0)
// Loads toolbars.xml via AC_Manager::loadToolbars(path, outIds) (vtable+0x178)

UT_String toolbarsPath = RM_GetResourcePath("toolbars.xml");
QList<QString> toolbarIds;
actionManager->loadToolbars(toolbarsPath.ToQString(), toolbarIds);
```

The `toolbars.xml` file defines toolbar content using XML elements like:
```xml
<toolbar id="DrawingViewToolbar" trContext="Toolbars">
    <item id="DrawingTool.Select" />
    <separator />
    <item id="DrawingTool.Brush" />
    <!-- ... more items ... -->
</toolbar>
```

### 2. Global Toolbar Initialization

Global toolbars are shown during session initialization (`sub_14002F840`):

```cpp
// Show global toolbars via TULayoutManager::showToolbar (vtable+416)
layoutManager->showToolbar("FileToolbar", true);
layoutManager->showToolbar("EditToolbar", true);
layoutManager->showToolbar("DrawingToolToolbar", true);
layoutManager->showToolbar("ArtLayerToolbar", true);

// Conditional toolbars based on features
if (WHO_Features::hasOnionSkinToolbar()) {
    layoutManager->showToolbar("OnionSkinToolbar", false, true, nullptr);
}
```

**Key Function Addresses (HarmonyPremium.exe):**
- Session toolbar init: `0x14002F840`
- `TULayoutManager::showToolbar` is called via an import thunk in HarmonyPremium.exe (address is version-specific)

### 3. View-Specific Toolbar Definition

Each view class can override `TULayoutView::toolbar()` (ToonBoomLayout.dll vtable `+0x58`) to provide a `QDomElement` describing the view-toolbar content.

Implementation detail (MSVC x64 ABI): this return-by-value is implemented with a hidden return buffer pointer (passed in `RDX` for member functions), which is why decompiled code often looks like `toolbar(this, out)`.

In Harmony Premium, many views inherit from `TUWidgetLayoutView` (via `VL_ViewQt`), so the `TULayoutView*` you get in `toolbar()` is an embedded subobject:

- `TUWidgetLayoutView::m_actionManager` is stored at `+0x30` (in the `TUWidgetLayoutView` object)
- the embedded `TULayoutView` base is at `+0x68`
- therefore, `AC_Manager*` is reachable from a `TULayoutView*` as `*(AC_Manager**)((char*)this - 0x38)`

**Example: Drawing toolbar override**

- Function: `VL_BaseDrawingViewQt__toolbar`
- Address: `0x1403B9880` (HarmonyPremium.exe)

Behavior (from disassembly/decompilation):
- Read `AC_Manager*` from `[this-0x38]`
- Call `AC_Manager` vfunc at vtable `+0x1A0` to fill a `QDomElement` for id `"DrawingViewToolbar"`
- Return an empty `QDomElement` if `AC_Manager` is null

**Example: Reference View toolbar override**

Toon Boom’s “Reference View” uses the `ModelViewToolbar` definition (see `toolbars.xml`), implemented in:
- Function: `VL_ModelViewQt__toolbar`
- Address: `0x1403C8A20` (HarmonyPremium.exe)

Behavior: same as above, but requests `"ModelViewToolbar"`.

**Relevant TULayoutView virtuals (ToonBoomLayout.dll):**
- `toolbar(...)`: vtable `+0x58`
- `setToolbarInfo(const LAY_ToolbarInfo&)`: vtable `+0x60`

### 4. Toolbar Display Flow

When the current tab/view changes, Toon Boom updates the frame’s view-toolbar via `TULayoutFrame::showViewToolBar()` (ToonBoomLayout.dll `0x7ffa0be4bb70`).

Confirmed call sites include:
- `TULayoutFrame::setCurrentTab(TULayoutViewHolder*)` → `showViewToolBar()`
- `TULayoutManager::showViewToolBars()` → `TULayoutFrame::showViewToolBar()`

High-level behavior of `showViewToolBar()` (validated from disassembly):
1. `currentView = m_layoutManager->findInstance(this, false)`
2. `toolbarEl = currentView->toolbar()` (calls the view’s override)
3. `info = currentView->getToolbarInfo()` (copy of `TULayoutView::m_toolbarInfo`)
4. Hide/reset cases:
   - If `info.m_isDefault != 0`: clear + hide the frame toolbar and return
   - If `toolbarEl` is null: clear + hide the frame toolbar and return
5. Ensure `m_viewToolbar` exists (created once via `createEmptyToolBar(info.m_name)` if needed)
6. Populate the toolbar:
   - `m_viewToolbar->setOwner(currentView->getWidget())`
   - `m_viewToolbar->changeContent(toolbarEl, info.m_buttonConfig, info.m_buttonDefaultConfig)`
7. Dock and apply placement:
   - `m_mainWindow->addToolBar(info.m_toolBarArea, m_viewToolbar->toQToolBar())`
   - `Layout::Toolbar::loadToolbar(info, m_mainWindow)`
   - `m_viewToolbar->setVisible(info.m_visible)`

### 4.1 raiseArea Function Details

The `raiseArea` function is for **bringing a view area to the foreground**, not for toolbar display:

```cpp
// Function signature (from ToonBoomLayout.dll)
TULayoutView* TULayoutManager::raiseArea(
    const QString& areaName,      // e.g., "Colour", "Morphing", "Timeline"
    TULayoutFrame* frame,          // Target frame, or nullptr for auto-selection
    bool createNew,                // true = create new instance if needed
    const QPoint& pos              // Position for floating windows
);
```

**Example usages found in HarmonyPremium.exe:**

```cpp
// sub_14086E410: Opens the Colour palette view
void showColourView() {
    TULayoutManager* mgr = getLayoutManager();
    QString areaName("Colour");
    QPoint pos(0, 0);
    mgr->raiseArea(areaName, nullptr, true, pos);
}

// sub_1401579C0: Opens Morphing view on double-click
// (called from QAbstractItemView::mouseDoubleClickEvent handler)
void openMorphingOnDoubleClick() {
    TULayoutFrame* frame = view->getLayoutFrame(widget);
    TULayoutManager* mgr = frame->getLayoutManager();
    QString areaName("Morphing");
    QPoint pos(0, 0);
    mgr->raiseArea(areaName, nullptr, true, pos);
}
```

**Key points:**
- `raiseArea` returns the `TULayoutView*` that was raised/created
- The third parameter `createNew` controls whether to create a new tab/instance
- Pass `nullptr` for `frame` to let the system choose the target frame
- Toolbar display is a **separate mechanism** triggered by focus changes

### 4.2 Ensuring Toolbar Display After raiseArea

If you need to ensure toolbars appear after calling `raiseArea`:

```cpp
// Method 1: Let the focus system handle it naturally
TULayoutView* view = layoutManager->raiseArea("MyArea", nullptr, true, QPoint(0,0));
if (view) {
    QWidget* widget = view->getWidget();
    if (widget) {
        widget->setFocus(Qt::OtherFocusReason);  // Triggers focus chain
    }
}

// Method 2: Explicitly trigger the toolbar display
TULayoutView* view = layoutManager->raiseArea("MyArea", nullptr, true, QPoint(0,0));
if (view) {
    TULayoutFrame* frame = view->getLayoutFrame(view->getWidget());
    if (frame) {
        // Direct refresh paths observed in ToonBoomLayout.dll:
        // - TULayoutFrame::setCurrentTab(...) calls frame->showViewToolBar()
        // - TULayoutManager::showViewToolBars() calls into the current frame(s)
        frame->showViewToolBar();
        layoutManager->showViewToolBars();
    }
}
```

## AC_Toolbar Integration

Toolbar definitions and instances involve three different components:
- `AC_Manager` (ToonBoomActionManager.dll): loads `<toolbar>` definitions (e.g., from `toolbars.xml`) and can create `AC_Toolbar` instances.
- `TULayoutManager` (ToonBoomLayout.dll / HarmonyPremium.exe): shows/hides *global* toolbars by name.
- `TULayoutFrame` (ToonBoomLayout.dll): owns the *view* toolbar instance (`m_viewToolbar`) and updates it on tab/view changes via `showViewToolBar()`.

### How Toolbars Are Actually Registered

1. **Toolbar Definitions are loaded from XML** (`toolbars.xml`):
   ```cpp
   // During session init (sub_140034EB0 in HarmonyPremium.exe)
   AC_Manager* actionManager = AC_CreateActionManager(keywords);
   
   // Load toolbars via AC_Manager::loadToolbars(path, outIds) (vtable+0x178)
   QString path = RM_GetResourcePath("toolbars.xml").ToQString();
   void** vtable = *reinterpret_cast<void***>(actionManager);
   using LoadToolbarsFromFileFn = void (__fastcall *)(AC_Manager* self, const QString* path, QList<QString>* outIds);
   auto loadToolbarsFromFile = reinterpret_cast<LoadToolbarsFromFileFn>(vtable[47]); // vtable+0x178
   QList<QString> toolbarIds;
   loadToolbarsFromFile(actionManager, &path, &toolbarIds);
   ```

2. **Toolbars are shown/activated via TULayoutManager::showToolbar**:
   ```cpp
   // During setupToolbars (0x14002f840)
   // This is vtable+416 on TULayoutManager, NOT AC_Manager
   layoutManager->showToolbar("FileToolbar", true);
   layoutManager->showToolbar("EditToolbar", true);
   ```

3. **View-specific toolbars are looked up via `AC_Manager::toolbarElement()`** (inside the view’s `toolbar()` override):
   ```cpp
   // Common pattern in HarmonyPremium.exe view toolbar() overrides:
   // - AC_Manager* is reachable from a TUWidgetLayoutView-embedded TULayoutView* at [this-0x38]
   AC_Manager* manager = *(AC_Manager**)((char*)this - 0x38);
   if (!manager) {
       return QDomElement();
   }
   return manager->toolbarElement(QString("DrawingViewToolbar"));
   ```

### AC_Manager Methods Used

The AC_Manager provides these key toolbar methods:

| Method | Vtable Offset | Description |
|--------|--------------|-------------|
| `toolbarElement(name)` | `+0x1A0` | Returns the `<toolbar ...>` `QDomElement` for `name` (member-function ABI uses a hidden return buffer in `RDX`) |
| `loadToolbars(element, outIds)` | `+0x170` | Loads toolbar definitions from a `QDomElement` (same structure as `toolbars.xml`) |
| `loadToolbars(path, outIds)` | `+0x178` | Loads toolbar definitions from an XML file (e.g., `toolbars.xml`) |
| `createToolbar(name, config, mainWindow, area, identity, owner)` | `+0x180` | Creates an `AC_Toolbar` instance from a stored toolbar id (implementation: `AC_ToolbarImpl`) |
| `createToolbar(element, config, mainWindow, area, identity, owner)` | `+0x188` | Creates an `AC_Toolbar` instance from a `QDomElement` (implementation: `AC_ToolbarImpl`) |

### AC_Toolbar Creation / Reuse

`TULayoutFrame::showViewToolBar()` does **not** create a new `AC_Toolbar` every time focus changes.

Observed behavior (ToonBoomLayout.dll `0x7ffa0be4bb70`):
- The frame keeps a single `AC_Toolbar*` at `TULayoutFrame + 0xC0` (`m_viewToolbar`)
- On each tab/view change, it:
  - calls `m_viewToolbar->setOwner(currentView->getWidget())`
  - calls `m_viewToolbar->changeContent(toolbarEl, buttonConfig, buttonDefaultConfig)`
  - docks `m_viewToolbar->toQToolBar()` into the frame’s `TULayoutMainWindow`

The `AC_Toolbar` object itself is created lazily by `TULayoutFrame::createEmptyToolBar(name)`:
- Builds an empty `<toolbar/>` element
- Calls `AC_Manager::createToolbar(emptyEl, nullptr, m_layoutManager, 4, nameLatin1, nullptr)`
- Stores the result as `m_viewToolbar`

### LAY_ToolbarInfo Configuration

Each view can have stored toolbar configuration via `LAY_ToolbarInfo`:

```cpp
// Structure of LAY_ToolbarInfo (~104 bytes)
// Analyzed from ToonBoomLayout.dll constructors and getters
class LAY_ToolbarInfo {
    int m_x;                              // +0x00 - X position (default: 0)
    int m_y;                              // +0x04 - Y position (default: 0)
    int m_index;                          // +0x08 - Toolbar index (default: -1)
    int m_width;                          // +0x0C - Width (default: -1)
    int m_height;                         // +0x10 - Height (default: -1)
    bool m_newline;                       // +0x14 - Break to new line (default: false)
    bool m_visible;                       // +0x15 - Visibility (default: true)
    bool m_isDefault;                     // +0x16 - Default/unconfigured state (Layout hides view-toolbar when true)
    // padding                            // +0x17
    QString m_name;                       // +0x18 - Toolbar name (24 bytes with SSO)
    Qt::Orientation m_orientation;        // +0x30 - Orientation (default: Horizontal)
    Qt::ToolBarArea m_toolBarArea;        // +0x34 - Docking area (default: TopToolBarArea=4)
    QList<QString> m_buttonConfig;        // +0x38 - Current button order (24 bytes)
    QList<QString> m_buttonDefaultConfig; // +0x50 - Default button order (24 bytes)
    // Total size: ~104 bytes
};
```

**Key LAY_ToolbarInfo Methods (ToonBoomLayout.dll):**

| Method | Address | Description |
|--------|---------|-------------|
| `LAY_ToolbarInfo()` | `0x7ffa0be6a3a0` | Default constructor |
| `LAY_ToolbarInfo(QString, int, int, int, int, int, bool, bool)` | `0x7ffa0be6a2c0` | Initializes `name,x,y,index,width,height,newline,visible` (sets `m_isDefault=1`) |
| `getName()` | `0x7ffa0be392b0` | Returns toolbar name |
| `getHeight()` | `0x7ffa0be39280` | Returns height |
| `isVisible()` | `0x7ffa0be39570` | Returns visibility |
| `isDefault()` | `0x7ffa0be39560` | Returns `m_isDefault` (Layout treats non-zero as “hide/reset view-toolbar”) |
| `getButtonConfig()` | `0x7ffa0be6a990` | Returns `nullptr` when `m_isDefault!=0`, else `&m_buttonConfig` |
| `setButtonConfig()` | `0x7ffa0be6a9a0` | Copies config into `m_buttonConfig` and sets `m_isDefault=0` |
| `setButtonDefaultConfig()` | `0x7ffa0be6aaf0` | Copies config into `m_buttonDefaultConfig` |
| `setName()` | `0x7ffa0be39bd0` | Sets toolbar name |
| `setVisible()` | `0x7ffa0be39c10` | Sets visibility |
| `fromXml()` | `0x7ffa0be6a520` | Loads from QDomElement |
| `toXml()` | `0x7ffa0be6ab00` | Saves to QDomElement |

Note: the `m_buttonDefaultConfig` accessor is ICF-folded with another trivial accessor and may show up in IDA as `?getChildren@TULayoutSplitter@@...` at `0x7ffa0be5ac60`; `TULayoutFrame::showViewToolBar()` uses that address as `&m_buttonDefaultConfig`.

### How `m_toolbarInfo` Gets Initialized (Critical)

`TULayoutFrame::showViewToolBar()` will *hide* the view-toolbar when `TULayoutView::m_toolbarInfo.m_isDefault != 0`, so the question becomes: **where does non-default toolbar info come from?**

This happens when a view is added to a frame (e.g., via `raiseArea()` / layout activation):
- `TULayoutArea::add` (ToonBoomLayout.dll `0x7ffa0be41c10`) first checks the frame’s cache (`getToolbarInfoForView(viewKey)`).
- If no cached entry exists, it calls `view->toolbar()` and reads `toolbarEl.attribute("id")` to get the toolbar identity string (e.g., `DrawingViewToolbar`).
- It then queries `TULayoutStorage::getViewToolbarConfig(viewKey, wantDefault)` (ToonBoomLayout.dll `0x7ffa0be5cff0`) to fetch button config lists keyed by `viewKey` (typically `view->getCaption(true)`).
- If configs exist, it builds a `LAY_ToolbarInfo` with:
  - `m_name = toolbarId`
  - `m_buttonConfig` + `m_buttonDefaultConfig` populated (which flips `m_isDefault -> 0`)
  - applies it via `view->setToolbarInfo(info)` and caches it in the frame.

Persisted customization updates the config lists via:
- `TULayoutFrame::toolbarWasCustomized` (ToonBoomLayout.dll `0x7ffa0be4be20`) → `TULayoutStorage::setViewToolbarConfig(viewKey, config, isDefault=false)` + `saveLayoutToolbar()`.

## Implementing Custom View Toolbars

### Step 1: Define Toolbar in XML

Create a toolbar definition that will be loaded by AC_Manager:

```xml
<toolbar id="MyCustomViewToolbar" trContext="MyToolbars">
    <item id="MyResponder.Action1" />
    <item id="MyResponder.Action2" />
    <separator />
    <placeholder id="CustomItems" />
</toolbar>
```

### Step 2: Override toolbar() in Your View

```cpp
class MyCustomView : public TULayoutView {
public:
    QDomElement toolbar() override {
        // Get AC_Manager - depends on your view's structure.
        // In HarmonyPremium.exe views derived from TUWidgetLayoutView, AC_Manager* is reachable from the embedded
        // TULayoutView* at [this-0x38]. For a custom TULayoutView subclass, store AC_Manager* yourself (or fetch
        // it via PLUG_Services::getActionManager()).
        AC_Manager* manager = getActionManager(); // your helper
        if (!manager) {
            return QDomElement();
        }
        
        return manager->toolbarElement(QString("MyCustomViewToolbar"));
    }
};
```

### Step 3: Register Toolbar XML

Ensure your toolbar XML is loaded during initialization:

```cpp
// Option 1: Add to existing toolbars.xml (if modifying installation)

// Option 2: Load programmatically
AC_Manager* manager = PLUG_Services::getActionManager();
if (manager) {
    // Use AC_Manager::loadToolbars(element, outIds) (vtable+0x170)
    // Note: loadToolbars expects the same structure as toolbars.xml; easiest is to wrap your <toolbar> inside a <toolbars> root.
    QDomDocument doc;
    doc.setContent(myToolbarXml);
    QDomElement element = doc.documentElement();
    
    QList<QString> ids;
    manager->loadToolbars(element, ids);
}
```

### Step 4: Ensure `LAY_ToolbarInfo` Is Non-Default

`TULayoutFrame::showViewToolBar()` (ToonBoomLayout.dll `0x7ffa0be4bb70`) will hide/reset the view-toolbar when `TULayoutView::m_toolbarInfo.m_isDefault != 0`.

For a custom view, you must ensure:
- `LAY_ToolbarInfo::m_name` is set to the toolbar element’s `"id"` (used as the toolbar instance identity when `createEmptyToolBar(name)` creates `m_viewToolbar`)
- `LAY_ToolbarInfo::m_buttonConfig` (and typically `m_buttonDefaultConfig`) are populated, so `m_isDefault` flips to `0`

Built-in views typically get this via `TULayoutArea::add()` reading config from `TULayoutStorage` (see the section above). If your view has no stored config, the toolbar will remain hidden until you seed it.

Minimal pattern (conceptual):
```cpp
LAY_ToolbarInfo info = this->getToolbarInfo();
info.setName(QString("MyCustomViewToolbar"));

QList<QString> buttons = {/* item ids in desired order */};
info.setButtonConfig(&buttons);        // flips m_isDefault -> 0
info.setButtonDefaultConfig(&buttons); // used for reset/customize UI

this->setToolbarInfo(info);
```

## Key Memory Offsets

### TULayoutView (from TULayoutView*)
- `+0x00`: vptr
- `+0x08`: QString m_internalName
- `+0x20`: LAY_ToolbarInfo m_toolbarInfo (104 bytes)
- `+0x88`: AC_Menu* m_menuByType[2]
- `+0xA0`: QString m_caption

### TULayoutFrame (from TULayoutFrame*)
- `+0x28`: TULayoutManager* m_layoutManager
- `+0x48`: TULayoutMainWindow* m_mainWindow (toolbar host)
- `+0x90`: AC_Toolbar* m_toolbar
- `+0xC0`: AC_Toolbar* m_viewToolbar

### AC_ToolbarImpl (from QToolBar base)
- `+0x28`: AC_ContainerImpl m_container (88 bytes)
- `+0x80`: AC_Toolbar vptr
- `+0x98`: QString m_responderIdentity
- `+0xC8`: bool m_isCustomizable
- `+0xE0`: QObject* m_owner

## Database Reference

### HarmonyPremium.exe
| Symbol | Address | Description |
|--------|---------|-------------|
| TULayoutManager__setupToolbars | `0x14002F840` | Initializes/shows global toolbars by name |
| VL_BaseDrawingViewQt__toolbar | `0x1403B9880` | Returns `DrawingViewToolbar` element |
| CV_CameraViewQt__toolbar | `0x1403BC450` | Returns `CameraViewToolbar` element |
| VL_ModelViewQt__toolbar | `0x1403C8A20` | Returns `ModelViewToolbar` element (Reference View) |
| TL_DockWindowQT__toolbar | `0x14011C5F0` | Returns `TimelineViewToolbar` element |

### ToonBoomActionManager.dll
| Symbol | Address | Description |
|--------|---------|-------------|
| AC_ManagerImpl__toolbarElement | `0x180016240` | `AC_Manager::toolbarElement(name)` implementation (returns `QDomElement` by value) |
| AC_ToolbarImpl__changeContent_fromElement | `0x1800332F0` | `AC_Toolbar::changeContent(element, config, defaultConfig)` |
| AC_ToolbarImpl::create | `0x180033910` | Build toolbar from XML element |
| AC_ToolbarImpl::create (with config) | `0x180033D50` | Build toolbar from XML element + config list |

### ToonBoomLayout.dll (Toolbar Infrastructure)
| Symbol | Address | Description |
|--------|---------|-------------|
| TULayoutArea::add | `0x7ffa0be41c10` | Initializes `TULayoutView::m_toolbarInfo` from `view->toolbar().attribute(\"id\")` + `TULayoutStorage` configs |
| TULayoutFrame::showViewToolBar | `0x7ffa0be4bb70` | Core view-toolbar update flow (calls view->toolbar(), then `changeContent` + docking) |
| TULayoutFrame::createEmptyToolBar | `0x7ffa0be47730` | Lazily creates/clears `m_viewToolbar` |
| TULayoutFrame::removeViewToolBar | `0x7ffa0be4a700` | Hides `m_viewToolbar` |
| TULayoutFrame::getCurToolbarState | `0x7ffa0be48b20` | Captures geometry/visibility from `m_viewToolbar->toQToolBar()` |
| TULayoutFrame::toolbarWasCustomized | `0x7ffa0be4be20` | Persists button order into `TULayoutStorage` (flips `m_isDefault -> 0`) |
| TULayoutView::toolbar | `0x7ffa0be610d0` | Base implementation (returns empty `QDomElement`) |
| TULayoutView::setToolbarInfo | `0x7ffa0be610c0` | Stores `LAY_ToolbarInfo` into the view |
| TULayoutView::getToolbarInfo | `0x7ffa0be60e80` | Returns pointer to embedded `LAY_ToolbarInfo` |
| TULayoutStorage::getViewToolbarConfig | `0x7ffa0be5cff0` | Returns stored `QList<QString>` pointer for a view-toolbar config |
| TULayoutStorage::setViewToolbarConfig | `0x7ffa0be5fa20` | Stores a view-toolbar config list (current or default) |
| LAY_ToolbarInfo::setButtonConfig | `0x7ffa0be6a9a0` | Copies config + flips `m_isDefault` |

## Toolbar Names Reference

### Global Toolbars
- `FileToolbar` - File operations
- `EditToolbar` - Edit operations  
- `DrawingToolToolbar` - Drawing tools
- `ArtLayerToolbar` - Art layer selection
- `SceneplanningToolToolbar` - Scene planning tools
- `OnionSkinToolbar` - Onion skin controls
- `FlipToolbar` - Flip/mirror tools
- `GameToolbar` - Game export tools
- `AlignmentGuidesToolbar` - Alignment guides
- `MarkDrawingToolBar` - Mark drawing tools

### View-Specific Toolbars
- `DrawingViewToolbar` - Drawing view
- `CameraViewToolbar` - Camera view
- `TimelineViewToolbar` - Timeline view
- `XsheetViewToolbar` - Xsheet view
- `NetworkViewToolbar` - Network/Node view
- `FunctionViewToolbar` - Function curve view
- `PlaybackViewToolbar` - Playback view
- `ModelViewToolbar` - Model view
- `LibraryViewToolbar` - Library view
- `ModuleLibraryViewToolbar` - Module library view
- `FreeViewToolbar` - Free-form view

## Troubleshooting

### Toolbar Not Appearing After raiseArea

If you call `TULayoutManager::raiseArea()` and the view's toolbar doesn't appear:

1. **`raiseArea` doesn't trigger toolbars directly**
   - `raiseArea` only brings a view to the foreground
   - Toolbar display is handled by `TULayoutFrame::showViewToolBar()` (typically reached via tab/view changes or `TULayoutManager::showViewToolBars()`)

2. **Manual toolbar trigger**
   ```cpp
   TULayoutView* view = layoutManager->raiseArea("MyArea", nullptr, true, QPoint(0,0));
   if (view) {
       TULayoutFrame* frame = view->getLayoutFrame(view->getWidget());
       if (frame) {
            // Direct refresh options:
            frame->showViewToolBar();
            layoutManager->showViewToolBars();
        }
   }
   ```

3. **Set focus to trigger natural flow**
   ```cpp
   TULayoutView* view = layoutManager->raiseArea("MyArea", nullptr, true, QPoint(0,0));
   if (view && view->getWidget()) {
       view->getWidget()->setFocus(Qt::OtherFocusReason);
   }
   ```

### View toolbar() Returns Empty Element

If your custom view's `toolbar()` method doesn't work:

1. **Check AC_Manager access**
   - For `TUWidgetLayoutView` subclasses: `AC_Manager* mgr = TULayoutView_getActionManager(this);`
   - The manager must be non-null

2. **Verify toolbar is registered**
   - The toolbar XML must be loaded via `AC_Manager::loadToolbars(path, outIds)` or `loadToolbars(element, outIds)`
   - Check that the toolbar `id` attribute matches what you're requesting

3. **Debug the vtable call**
   ```cpp
   QDomElement MyView::toolbar() {
       AC_Manager* manager = TULayoutView_getActionManager(this);
       if (!manager) {
           qDebug() << "No AC_Manager available";
           return QDomElement();
       }
       
       // NOTE: AC_Manager::toolbarElement(...) returns QDomElement by value.
       // MSVC x64 member-function ABI uses a hidden return buffer pointer (RDX),
       // so a raw vtable call must include an explicit QDomElement* out.
       void** vtable = *reinterpret_cast<void***>(manager);
       using ToolbarElementFn = QDomElement* (__fastcall *)(AC_Manager* self, QDomElement* out, const QString* name);
       auto toolbarElement = reinterpret_cast<ToolbarElementFn>(vtable[52]);
       
       QDomElement element;
       QString name("MyViewToolbar");
       toolbarElement(manager, &element, &name);
       
       if (element.isNull()) {
           qDebug() << "Toolbar" << name << "not found in AC_Manager";
       }
       return element;
   }
   ```

### Focus/Frame Issues

1. **Frame not set as current**
   - The system uses `setCurrentLayoutFrame` to track which frame is active
   - `showViewToolBar()` is triggered by tab/view changes or `TULayoutManager::showViewToolBars()`, not directly by `setCurrentLayoutFrame`

2. **View not in frame's current tab**
   - The raised view may be in a tab but not the current one
   - Use `TULayoutFrame::setCurrentTab()` if needed

See **Database Reference** for verified addresses used in this document.

### Example: Complete Toolbar Integration

```cpp
// 1. During initialization - register your toolbar XML
void MyPlugin::initialize(AC_Manager* manager) {
    QString toolbarXml = R"(
        <toolbar id="MyViewToolbar" trContext="MyPlugin">
            <item id="MyResponder.DoSomething" />
            <separator />
            <item id="MyResponder.DoOther" />
        </toolbar>
    )";
    
    QDomDocument doc;
    doc.setContent(toolbarXml);
    
    // Register with AC_Manager (loadToolbars(element, outIds))
    QList<QString> ids;
    manager->loadToolbars(doc.documentElement(), ids);
}

// 2. In your view class - override toolbar()
QDomElement MyView::toolbar() {
    AC_Manager* manager = TULayoutView_getActionManager(this);
    if (!manager) return QDomElement();
    
    void** vtable = *reinterpret_cast<void***>(manager);
    auto getToolbarElement = reinterpret_cast<void(*)(AC_Manager*, QDomElement*, const QString&)>(vtable[52]);
    
    QDomElement element;
    getToolbarElement(manager, &element, QString("MyViewToolbar"));
    return element;
}

// 3. When raising your view and ensuring toolbar appears
void showMyView(TULayoutManager* layoutManager) {
    TULayoutView* view = layoutManager->raiseArea("MyView", nullptr, true, QPoint(0,0));
    if (view) {
        // Ensure toolbar displays by triggering focus flow
        if (QWidget* widget = view->getWidget()) {
            widget->setFocus(Qt::OtherFocusReason);
        }
        // Or explicitly trigger a refresh
        TULayoutFrame* frame = view->getLayoutFrame(view->getWidget());
        if (frame) {
            frame->showViewToolBar();
            layoutManager->showViewToolBars();
        }
    }
}
```
