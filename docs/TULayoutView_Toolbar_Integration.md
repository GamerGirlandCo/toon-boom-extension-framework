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

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      TULayoutManager                             │
│  - Manages all frames, areas, and global toolbars               │
│  - Holds reference to AC_Manager at offset +344                 │
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
// Loads toolbars.xml via AC_Manager::addToolbarFromFile (vtable+376)

UT_String toolbarsPath = RM_GetResourcePath("toolbars.xml");
actionManager->addToolbarFromFile(toolbarsPath.ToQString(), &toolbarList);
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
- TULayoutManager::showToolbar import: `0x1409c8ea0`

### 3. View-Specific Toolbar Definition

Each view class overrides `TULayoutView::toolbar()` to return its toolbar definition:

```cpp
// Example: CameraView::toolbar() at 0x1403BC450
QDomElement CameraView::toolbar() {
    // Get AC_Manager from embedded context (offset -56 from TULayoutView*)
    AC_Manager* manager = *(AC_Manager**)(this - 56);
    if (manager) {
        // Call AC_Manager::getToolbarElement (vtable+416 / offset 52*8)
        QDomElement element;
        manager->getToolbarElement(&element, "CameraViewToolbar");
        return element;
    }
    return QDomElement(); // Empty if no manager
}
```

**Known View Toolbar Implementations:**

| View Class | Toolbar Name | Address |
|------------|--------------|---------|
| DrawingView | "DrawingViewToolbar" | `0x1403B9880` |
| CameraView | "CameraViewToolbar" | `0x1403BC450` |
| TimelineView | "TimelineViewToolbar" | `0x14011C5F0` |
| SGV_Graph3DView | (custom impl) | `0x1409e1866` |

### 4. Toolbar Display Flow

When a view gains focus, the toolbar is displayed through this flow:

```
1. User clicks on a view widget
           │
           ▼
2. Application event handler detects focus change
   (sub_140059DE0 checks for TULayoutFrame inheritance)
           │
           ▼
3. TULayoutManager::setCurrentLayoutFrame(frame)
           │
           ▼
4. TULayoutFrame::showViewToolBar()
   - Gets current view from view holder
   - Calls view->toolbar() to get QDomElement
   - Creates AC_ToolbarImpl from element
   - Adds toolbar to TULayoutMainWindow
           │
           ▼
5. Toolbar becomes visible in frame's toolbar area
```

## AC_Toolbar Integration

### AC_Manager Methods Used

The AC_Manager provides these key toolbar methods:

| Method | Vtable Offset | Description |
|--------|---------------|-------------|
| `getToolbarElement` | +416 (52*8) | Returns QDomElement for toolbar by name |
| `addToolbarFromFile` | +376 (47*8) | Loads toolbar definitions from XML file |
| `createToolbar` | +368 (46*8) | Creates AC_ToolbarImpl from element |

### AC_ToolbarImpl Creation

When `TULayoutFrame::showViewToolBar()` needs to display a toolbar:

```cpp
// Pseudocode for toolbar creation
void TULayoutFrame::showViewToolBar() {
    TULayoutView* view = getCurrentView();
    if (!view) return;
    
    // Get toolbar XML element from view
    QDomElement toolbarElement = view->toolbar();
    if (toolbarElement.isNull()) return;
    
    // Create AC_ToolbarImpl via AC_Manager
    AC_Toolbar* toolbar = m_actionManager->createToolbar(
        this,                    // owner
        toolbarElement.attribute("text"),
        m_mainWindow,            // parent QMainWindow
        toolbarElement.attribute("id").toUtf8().constData(),
        toolbarElement,
        nullptr                  // default config
    );
    
    // Add to frame's main window
    m_mainWindow->addToolBar(Qt::TopToolBarArea, toolbar->toQToolBar());
    
    // Store for later removal
    m_viewToolbar = toolbar;
}
```

### LAY_ToolbarInfo Configuration

Each view can have stored toolbar configuration via `LAY_ToolbarInfo`:

```cpp
// Structure of LAY_ToolbarInfo (104 bytes)
class LAY_ToolbarInfo {
    int m_x;                              // +0x00 - X position
    int m_y;                              // +0x04 - Y position  
    int m_index;                          // +0x08 - Toolbar index
    int m_width;                          // +0x0C - Width
    int m_height;                         // +0x10 - Height
    bool m_newline;                       // +0x14 - Break to new line
    bool m_visible;                       // +0x15 - Visibility
    bool m_isDefault;                     // +0x16 - Using default config
    QString m_name;                       // +0x18 - Toolbar name
    Qt::Orientation m_orientation;        // +0x30 - Horizontal/Vertical
    Qt::ToolBarArea m_toolBarArea;        // +0x34 - Docking area
    QList<QString> m_buttonConfig;        // +0x38 - Current button order
    QList<QString> m_buttonDefaultConfig; // +0x50 - Default button order
};
```

The view receives its toolbar info via `TULayoutView::setToolbarInfo()`:
- Import: `0x1409c8f4e` (thunk to ToonBoomLayout.dll)
- Called by TULayoutFrame when loading saved layout preferences

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
        // Get AC_Manager - depends on your view's structure
        // For TUWidgetLayoutView subclasses, manager is at offset -56
        AC_Manager* manager = getActionManager();
        if (!manager) {
            return QDomElement();
        }
        
        // Use AC_Manager to get toolbar element by name
        QDomElement element;
        // Call via vtable[52] - getToolbarElement
        auto getToolbarElement = reinterpret_cast<void(*)(AC_Manager*, QDomElement*, const QString&)>(
            (*reinterpret_cast<void***>(manager))[52]
        );
        
        QString toolbarName("MyCustomViewToolbar");
        getToolbarElement(manager, &element, toolbarName);
        
        return element;
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
    // Use addToolbarFromElement (vtable+376)
    QDomDocument doc;
    doc.setContent(myToolbarXml);
    QDomElement element = doc.documentElement();
    
    // Call via vtable
    auto addToolbarFromElement = reinterpret_cast<void(*)(AC_Manager*, const QDomElement&, const QString&, bool)>(
        (*reinterpret_cast<void***>(manager))[47]
    );
    addToolbarFromElement(manager, element, "MyCustomViewToolbar", false);
}
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
| Session toolbar init | `0x14002F840` | Initializes all global toolbars |
| DrawingView::toolbar | `0x1403B9880` | Returns DrawingViewToolbar element |
| CameraView::toolbar | `0x1403BC450` | Returns CameraViewToolbar element |
| TimelineView::toolbar | `0x14011C5F0` | Returns TimelineViewToolbar element |
| TULayoutView::setToolbarInfo import | `0x1409c8f4e` | Sets toolbar configuration |
| TULayoutView::toolbar import | `0x1409c8fc0` | Returns toolbar QDomElement |
| TULayoutManager::showToolbar import | `0x1409c8ea0` | Shows/creates toolbar by name |
| TULayoutManager::addToolbar import | `0x1409c8e4c` | Adds toolbar to manager |

### ToonBoomActionManager.dll
| Symbol | Address | Description |
|--------|---------|-------------|
| AC_ToolbarImpl::AC_ToolbarImpl | `0x180032df0` | Constructor |
| AC_ToolbarImpl::~AC_ToolbarImpl | `0x180033080` | Destructor |
| AC_ToolbarImpl::create | `0x180033910` | Creates toolbar from XML |
| AC_ToolbarImpl::insert | `0x1800345e0` | Inserts item at position |
| AC_ToolbarImpl vtable | `0x180054eb0` | Main vtable |

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
