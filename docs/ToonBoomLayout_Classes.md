# ToonBoomLayout.dll Class Analysis

This document contains reverse engineering analysis of key classes from `ToonBoomLayout.dll` used in Toon Boom Harmony Premium and Storyboard Pro.

## Overview

The layout system in Toon Boom uses a hierarchy of classes:
- **TULayoutView** - Base abstract class representing a view/panel that can be displayed
- **TULayoutViewHolder** - Widget container that can hold 1-2 TULayoutView instances with a splitter
- **TULayoutFrame** - Top-level frame (inherits QFrame) that contains tabs of view holders

## Class: TULayoutView

### Purpose
Abstract base class for all layout views in Toon Boom applications. Represents a dockable/tabbable view panel.

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr | Virtual function table pointer |
| 0x08 | 24 | QString | m_internalName | Internal identifier (default: "View" + uniqueId) |
| 0x20 | 104 | LAY_ToolbarInfo | m_toolbarInfo | Toolbar configuration for this view |
| 0x88 | 16 | AC_Menu*[2] | m_menuByType | Menus indexed by MenuType enum |
| 0x98 | 1 | bool | m_initializedFromCopy | True if initialized via copy constructor |
| 0xA0 | 24 | QString | m_caption | User-visible caption/title |

**sizeof(TULayoutView) ≈ 0xB8 (184 bytes)**

### Virtual Function Table

| VTable Idx | Method | Signature |
|------------|--------|-----------|
| 0 | destructor | `virtual ~TULayoutView()` |
| 1 | (pure virtual) | `virtual QWidget* widget() = 0` |
| 2 | initiate | `virtual TULayoutView* initiate(QWidget* parent)` |
| 3 | (pure virtual) | Unknown |
| 4 | (pure virtual) | Unknown |
| 5 | getParentHolderWidget (const) | `virtual const TULayoutViewHolder* getParentHolderWidget() const` |
| 6 | getParentHolderWidget | `virtual TULayoutViewHolder* getParentHolderWidget()` |
| 7 | hasMenu | `virtual bool hasMenu()` |
| 8 | setMenu | `virtual void setMenu(AC_Manager*, const char*, MenuType)` |
| 9 | setMenu | `virtual void setMenu(AC_Menu*, MenuType)` |
| 10 | menu | `virtual AC_Menu* menu(MenuType)` |
| 11 | toolbar | `virtual QDomElement toolbar()` |
| 12 | setToolbarInfo | `virtual void setToolbarInfo(const LAY_ToolbarInfo&)` |
| 13-14 | (other virtuals) | Implementation-specific |
| 15 | initializedFromCopy | `virtual bool initializedFromCopy()` |
| 16 | getCaption | `virtual QString getCaption(bool includeAdvanced) const` |
| 17 | getDynamicTextForCaption | `virtual QString getDynamicTextForCaption() const` |
| 18 | wantEditionStack | `virtual bool wantEditionStack() const` |
| 19 | displayName | `virtual QString displayName() const` |

### Key Methods

- **Constructor**: Generates unique internal name "View{N}" using static counter `TULayoutView::_uniqueId`
- **Copy Constructor**: Copies all members, sets `m_initializedFromCopy = true`
- **getCaption()**: Returns caption, optionally appending dynamic text in brackets if advanced display enabled
- **getParentHolderWidget()**: Traverses widget hierarchy to find parent TULayoutViewHolder

---

## Class: TULayoutViewHolder

### Purpose
Container widget that can hold 1-2 TULayoutView instances with an optional splitter for split view.

### Inheritance
`QWidget` → `TULayoutViewHolder`

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr (QObject) | QObject vtable |
| 0x10 | 8 | ptr | vptr (QPaintDevice) | QPaintDevice vtable |
| 0x18-0x27 | | | (QWidget members) | Inherited from QWidget |
| 0x28 | 8 | ptr | m_views.begin | std::vector<TULayoutView*> start |
| 0x30 | 8 | ptr | m_views.end | std::vector<TULayoutView*> end |
| 0x38 | 8 | ptr | m_views.capacity | std::vector<TULayoutView*> capacity |
| 0x40 | 8 | double | m_savedSplitterRatio | Splitter ratio (default: 0.5) |
| 0x48 | 8 | ptr | m_splitter | UI_Splitter* (vertical orientation) |
| 0x50 | 8 | ptr | m_leftLayout | WID_VBoxLayout* for left side |
| 0x58 | 8 | ptr | m_rightLayout | WID_VBoxLayout* for right side |
| 0x60 | 8 | ptr | m_leftFrame | QFrame* for left pane |
| 0x68 | 8 | ptr | m_rightFrame | QFrame* for right pane |

**sizeof(TULayoutViewHolder) = 0x70 (112 bytes)**

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x180031150 | Constructor | Creates holder with parent widget, sets up splitter |
| 0x180031360 | Destructor | Frees view vector, destroys widget |
| 0x180031480 | addView | Adds a view (max 2), returns false if full |
| 0x180031610 | nbViews | Returns count of views in holder |
| 0x180031620 | removeView | Removes view from holder, adjusts layout |
| 0x180031850 | setFocusToChild | Sets focus to first view's widget |
| 0x180031890 | splitterRatio | Returns current splitter ratio |
| 0x1800319b0 | updateWidgets | Updates layout after view changes |

### Behavior

- When empty (0 views): Splitter hidden
- When 1 view: View added directly to main layout, splitter hidden
- When 2 views: First view in left frame, second in right frame, splitter visible
- Maximum capacity: 2 views (addView returns false if full)

---

## Class: TULayoutFrame

### Purpose
Top-level dockable/floatable frame that contains a tabbed interface of TULayoutViewHolder widgets.

### Inheritance
`QFrame` → `TULayoutFrame`

### Constructor Signature
```cpp
TULayoutFrame(
    AC_Manager* manager,        // Action/menu manager
    QWidget* parent,            // Parent widget
    const QString& name,        // Object name
    TULayoutManager* layoutMgr, // Layout manager
    bool floating,              // True if floating window
    bool unknown,               // Unknown flag
    bool singleViewMode         // True for single view mode
);
```

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr (QObject) | QObject vtable |
| 0x10 | 8 | ptr | vptr (QPaintDevice) | QPaintDevice vtable |
| 0x18-0x27 | | | (QFrame members) | Inherited from QFrame |
| 0x28 | 8 | ptr | m_layoutManager | TULayoutManager* |
| 0x30 | 8 | ptr | m_mainLayout | WID_VBoxLayout* main vertical layout |
| 0x38 | 8 | ptr | m_topHBoxLayout | WID_HBoxLayout* top row |
| 0x40 | 8 | ptr | m_menuButton | QToolButton* "View Menu" button |
| 0x48 | 8 | ptr | m_mainWindow | TULayoutMainWindow* (contains toolbar area) |
| 0x50 | 8 | ptr | m_displayTools | TULayoutDisplayTools* display selector |
| 0x58 | 8 | ptr | m_compositeCombo | QComboBox* composite selector |
| 0x60 | 8 | ptr | m_tabBar | GUT_TabBar* tab bar |
| 0x68 | 8 | ptr | m_stack | QStackedWidget* for tab content |
| 0x70 | 8 | ptr | m_editionStackWidget | Symbol/edition stack widget |
| 0x78 | 8 | ptr | m_closeButton | QToolButton* close button |
| 0x80 | 8 | ptr | m_createViewMenuButton | QToolButton* "Create View Menu" |
| 0x88 | 1 | bool | m_isCurrent | Is this the current/focused frame |
| 0x89 | 1 | bool | m_isDocked | True if docked (not floating) |
| 0x8A | 1 | bool | m_isInSingleViewMode | Single view mode flag |
| 0x90 | 8 | ptr | m_toolbar | AC_Toolbar* view-specific toolbar |
| 0x98 | 4 | int | m_unknown | Initialized to -1 |
| 0xA0 | 16 | | m_toolbarInfoMap | Map/list for toolbar info per view |
| 0xB0 | 8 | ptr | m_parentWidget | Parent widget reference |
| 0xB8 | 8 | ptr | m_layoutFrameMenu | AC_Menu* context menu |
| 0xC0 | 8 | ptr | m_viewToolbar | AC_Toolbar* active toolbar |

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x180015DD0 | Constructor | Full initialization with all UI elements |
| 0x180016D10 | Destructor | Cleanup menu and toolbar info |
| 0x180010D60 | getLayoutManager | Returns m_layoutManager |
| 0x180010D70 | getStack | Returns m_stack (QStackedWidget) |
| 0x18001B660 | setCurrent | Sets m_isCurrent, triggers repaint |
| 0x180010DB0 | isDocked | Returns m_isDocked |
| 0x1800190C0 | isInSingleViewMode | Returns m_isInSingleViewMode |
| 0x180018F70 | getTab | Returns m_tabBar |
| 0x180018F80 | getToolbar | Returns m_viewToolbar |
| 0x180017FB0 | currentTab | Returns current tab index |
| 0x180017FC0 | currentTabName | Returns current tab name |
| 0x180018000 | currentTabViewHolder | Returns TULayoutViewHolder* for current tab |
| 0x1800172B0 | addTab | Adds tab with view holder |
| 0x1800180E0 | delTab | Removes tab by view holder |
| 0x180018170 | delView | Removes view widget |
| 0x180018280 | delViewHolder | Removes view holder |
| 0x18001B670 | setCurrentTab(int) | Sets current tab by index |
| 0x18001B6B0 | setCurrentTab(ViewHolder*) | Sets current tab by view holder |
| 0x18001C2B0 | updateTabName | Updates tab display text |

### Signals/Slots

Connected signals (from constructor analysis):
- `TULayoutDisplayTools::activated(int)` → `TULayoutFrame::onDisplayChanged(int)`
- `QComboBox::textActivated(QString)` → `TULayoutFrame::compositeChanged(QString)`
- `GUT_TabBar::clicked()` → `TULayoutFrame::updateFocus()`
- `GUT_TabBar::currentChanged(int)` → `TULayoutFrame::setCurrentTab(int)`
- `QToolButton::clicked()` → `TULayoutFrame::handleCloseViewButton()`

---

## Supporting Class: LAY_ToolbarInfo

### Purpose
Stores toolbar configuration and state for a view.

### Memory Layout

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 4 | int | m_x | X position |
| 0x04 | 4 | int | m_y | Y position |
| 0x08 | 4 | int | m_index | Toolbar index |
| 0x0C | 4 | int | m_width | Width |
| 0x10 | 4 | int | m_height | Height |
| 0x14 | 1 | bool | m_newline | Starts new row |
| 0x15 | 1 | bool | m_visible | Is visible |
| 0x16 | 1 | bool | m_isDefault | Is default config |
| 0x18 | 24 | QString | m_name | Toolbar name |
| 0x30 | 4 | Qt::Orientation | m_orientation | Horizontal/Vertical |
| 0x34 | 4 | Qt::ToolBarArea | m_toolBarArea | Dock area |
| 0x38 | 24 | QList<QString> | m_buttonConfig | Button configuration |
| 0x50 | 24 | QList<QString> | m_buttonDefaultConfig | Default button config |

**sizeof(LAY_ToolbarInfo) = 0x68 (104 bytes)**

---

## Analysis Methodology

1. **Constructor Analysis**: Traced member initialization order and sizes
2. **Accessor Analysis**: Used simple getter methods to confirm member offsets
3. **Destructor Analysis**: Verified cleanup order and dynamic allocations
4. **VTable Extraction**: Read vtable memory and resolved function addresses
5. **Signal/Slot Tracing**: Identified Qt connections in constructor

## Database Locations

- **TULayoutView vtable**: `0x180056F38`
- **TULayoutViewHolder vtable (QObject)**: `0x18005D600`
- **TULayoutViewHolder vtable (QPaintDevice)**: `0x18005D770`
- **TULayoutFrame vtable (QObject)**: `0x180058E18`
- **TULayoutFrame vtable (QPaintDevice)**: `0x180058F90`
- **TULayoutView::staticMetaObject**: N/A (not a QObject)
- **TULayoutViewHolder::staticMetaObject**: `0x1800646E0`
- **TULayoutFrame::staticMetaObject**: `0x180062AF0`
- **TULayoutManager vtable (QObject)**: `0x18005A1C8`
- **TULayoutManager vtable (TULayoutStorage)**: `0x18005A428`
- **TULayoutManager vtable (PLUG_ToolbarService)**: `0x18005A458`
- **TULayoutManager vtable (PLUG_MenuService)**: `0x18005A490`
- **TULayoutManager::staticMetaObject**: `0x180062BF0`
- **TULayoutMainWindow vtable**: `0x180058C60`
- **TUWidgetLayoutView vtable (QObject)**: `0x18005C0F8`
- **TUWidgetLayoutView vtable (QPaintDevice)**: `0x18005C268`
- **TUWidgetLayoutView vtable (AC_ResponderTemplateWidget)**: `0x18005C2A8`
- **TUWidgetLayoutView vtable (TULayoutView)**: `0x18005C348`
- **TUWidgetLayoutView::staticMetaObject**: `0x180064040`

---

## Class: TULayoutManager

### Purpose
Central manager for the entire layout system. Manages all frames, areas, splitters, toolbars, and layout persistence. Inherits from multiple base classes to provide comprehensive functionality.

### Inheritance
`QMainWindow` + `TULayoutStorage` + `PLUG_ToolbarService` + `PLUG_MenuService` → `TULayoutManager`

### Constructor Signature
```cpp
TULayoutManager(QString layoutPath);
```

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr (QObject) | QObject vtable |
| 0x10 | 8 | ptr | vptr (QPaintDevice) | QPaintDevice vtable |
| 0x18-0x27 | | | (QMainWindow members) | Inherited from QMainWindow |
| 0x28 | 136 | TULayoutStorage | (base class) | Layout storage at +40 |
| 0xB0 | 8 | PLUG_ToolbarService | (base class) | Toolbar service interface |
| 0xB8 | 8 | PLUG_MenuService | (base class) | Menu service interface |
| 0xC0 | 1 | bool | m_unknown | Unknown flag |
| 0xC8 | 8 | ptr | m_private | TULayoutManager_Private* |

### TULayoutManager_Private Structure (0x110 = 272 bytes)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr | VTable pointer |
| 0x08-0x0F | | | (QObject members) | Inherited |
| 0x10 | 8 | ptr | m_mainFrame | QFrame* central frame |
| 0x18 | 8 | ptr | m_mainLayout | WID_VBoxLayout* |
| 0x20-0x5F | | | (reserved) | Initialized to null |
| 0x60 | 24 | vector | m_splitters | std::vector<TULayoutSplitter*> |
| 0x78 | 24 | vector | m_frames | std::vector<TULayoutFrame*> |
| 0x90 | 24 | vector | m_areas | std::vector<TULayoutArea*> |
| 0xA8 | 24 | vector | m_pluginAreas | std::vector<TULayoutArea*> |
| 0xC0 | 8 | QPoint | m_savedPos | Saved window position |
| 0xC8 | 8 | QSize | m_savedSize | Saved window size |
| 0xD0 | 4 | int | m_stateFlags | State flags |
| 0xD8 | 8 | ptr | m_currentLayoutFrame | TULayoutFrame* currently focused |
| 0xF0 | 8 | ptr | m_owner | TULayoutManager* back pointer |
| 0xF8 | 8 | ptr | m_actionManager | AC_Manager* action manager |

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x18001CBA0 | Constructor | Creates manager, initializes storage, creates private data |
| 0x18001D360 | Destructor | Cleans up all views, areas, frames |
| 0x18001DDB0 | addFrame(QString) | Creates new TULayoutFrame |
| 0x18001E030 | addFrame(QWidget*) | Creates frame with parent |
| 0x18001DB50 | addArea | Adds new TULayoutArea |
| 0x18001EC00 | addView | Adds view to frame |
| 0x180020130 | delFrame | Removes and deletes frame |
| 0x18001F4F0 | closeFrame | Closes frame, may delete |
| 0x180020D90 | findFrame(int) | Find frame by index |
| 0x180020DC0 | findFrame(QWidget*) | Find frame containing widget |
| 0x180020EA0 | findInstance | Find view instance by name |
| 0x180021FE0 | getCurrentLayoutFrame | Returns m_currentLayoutFrame |
| 0x1800220D0 | getMainFrame | Returns m_mainFrame |
| 0x1800220C0 | getFrames | Returns frames vector |
| 0x180021F40 | getAreas | Returns areas vector |
| 0x180022270 | getSplitters | Returns splitters vector |
| 0x1800241D0 | setCurrentLayoutFrame | Sets focused frame |
| 0x1800241C0 | setActionManager | Sets AC_Manager |
| 0x18001FE60 | currentView | Returns currently visible view |
| 0x18001F1C0 | changeLayout | Changes to named layout |

### Signals

- `backgroundImageChanged()`
- `fullScreenStateChanged()`
- `layoutChanged(TULayout*)`
- `preferencesChange()`
- `sceneSaved()`

---

## Class: TULayoutMainWindow

### Purpose
Lightweight QMainWindow subclass used inside TULayoutFrame to provide toolbar docking area for view-specific toolbars.

### Inheritance
`QMainWindow` → `TULayoutMainWindow`

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr (QObject) | QObject vtable |
| 0x10 | 8 | ptr | vptr (QPaintDevice) | QPaintDevice vtable |
| 0x18-0x27 | | | (QMainWindow members) | Inherited |
| 0x28 | 8 | ptr | m_manager | AC_Manager* reference |

**sizeof(TULayoutMainWindow) = 0x30 (48 bytes)**

### Notes

- Created inside TULayoutFrame constructor
- Stores reference to AC_Manager for toolbar creation
- The QStackedWidget is set as central widget
- Provides toolbar docking areas (top, bottom, left, right)

---

## Class: TULayoutStorage

### Purpose
Base class providing layout persistence and toolbar configuration management.

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr | Virtual function table |
| 0x08 | 8 | ptr | m_currentLayout | TULayout* current active layout |
| 0x10 | 8 | ptr | m_previousLayout | TULayout* previous layout |
| 0x18 | 24 | vector | m_layouts | std::vector<TULayout*> all layouts |
| 0x30 | 24 | vector | m_toolbarLayouts | std::vector<TULayout*> toolbar layouts |
| 0x50 | 24 | QString | m_layoutPath | Path to layout storage |
| 0x68 | 8 | ptr | m_globalToolbarConfig | std::map for global toolbar configs |
| 0x70 | 8 | | (unused) | |
| 0x78 | 8 | ptr | m_viewToolbarConfig | std::map for view toolbar configs |
| 0x80 | 4 | int | m_flags | State flags |

**sizeof(TULayoutStorage) = 0x88 (136 bytes)**

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x18002B790 | Constructor | Initializes with layout path |
| 0x18002B890 | Destructor | Cleans up layouts and configs |
| 0x180009220 | getCurrentLayout | Returns m_currentLayout |
| 0x18002D0E0 | loadLayout | Loads layout from file |
| 0x18002E3B0 | loadLayouts | Loads all layouts from directory |
| 0x18002E8C0 | saveLayout | Saves layout to file |
| 0x18002C8F0 | findLayout | Finds layout by name |
| 0x18002BA70 | addLayout | Creates new layout |
| 0x18002C5D0 | delLayout | Deletes layout |

---

## Class: TUWidgetLayoutView

### Purpose
Primary concrete implementation of TULayoutView for wrapping QWidget content in the layout system. Inherits from `AC_ResponderTemplateWidget<QWidget>` and embeds `TULayoutView` at offset +104.

### Inheritance
`AC_ResponderTemplateWidget<QWidget>` + `TULayoutView` (embedded) → `TUWidgetLayoutView`

### Constructor Signature
```cpp
TUWidgetLayoutView::TUWidgetLayoutView(
    AC_Manager* manager,       // Action manager for menus/toolbars
    const QString& viewName,   // Internal name (e.g., "paletteView")
    QWidget* parent,           // Parent widget (usually nullptr)
    const char* objectName,    // QObject name (e.g., "PaletteLayoutView")
    Qt::WindowFlags flags      // Window flags (usually 0)
);
```

### Memory Layout (x64 MSVC)

| Offset | Size | Type | Member Name | Description |
|--------|------|------|-------------|-------------|
| 0x00 | 8 | ptr | vptr (QObject) | From AC_ResponderTemplateWidget<QWidget> |
| 0x10 | 8 | ptr | vptr (QPaintDevice) | |
| 0x18-0x27 | | | (QWidget members) | Inherited from QWidget |
| 0x28 | 8 | ptr | vptr (AC_ResponderTemplateWidget) | AC_Responder interface |
| 0x30 | 8 | ptr | m_actionManager | AC_Manager* |
| 0x38 | 24 | QString | m_responderIdentity | Responder ID |
| 0x50 | 24 | QString | m_responderDescription | Description |
| 0x68 | 8 | ptr | vptr (TULayoutView) | **TULayoutView interface starts here** |
| 0x70 | 24 | QString | m_internalName | Internal name from TULayoutView |
| 0x88 | 104 | LAY_ToolbarInfo | m_toolbarInfo | Toolbar info from TULayoutView |
| 0xF0 | 16 | AC_Menu*[2] | m_menuByType | Menus from TULayoutView |
| 0x100 | 1 | bool | m_initializedFromCopy | Copy flag from TULayoutView |
| 0x108 | 24 | QString | m_caption | Caption from TULayoutView |

**sizeof(TUWidgetLayoutView) ≈ 0x120 (288 bytes)**

### VTables

- **QObject vtable**: `0x18005C0F8`
- **QPaintDevice vtable**: `0x18005C268`
- **AC_ResponderTemplateWidget<QWidget> vtable**: `0x18005C2A8`
- **TULayoutView vtable**: `0x18005C348`
- **staticMetaObject**: `0x180064040`

### Key Methods

| Address | Method | Description |
|---------|--------|-------------|
| 0x1800300A0 | Constructor | Initializes widget, responder, and TULayoutView |
| 0x180030480 | Destructor | Destroys TULayoutView members, then base class |
| 0x180030E90 | getWidget() const | Returns `this - 104` (QWidget* from TULayoutView*) |
| 0x180030F10 | mousePressEvent | Calls base, sets focus if event not accepted |
| 0x180031140 | triggerMenuChanged | Emits menuChanged() signal |
| 0x18004A6D0 | menuChanged | Qt signal emission |

### Critical: The +104 Offset

When working with TUWidgetLayoutView and TULayoutManager:

1. **TULayoutView* is at offset +104 (0x68)** from TUWidgetLayoutView base
2. **getWidget() returns `this - 104`** when called on TULayoutView interface
3. When registering with `addArea`, pass `layoutView()`:

```cpp
TUWidgetLayoutView* widget = new MyCustomView(...);
TULayoutView* layoutView = widget->layoutView();  // returns this + 104
layoutManager->addArea("MyView", displayName, layoutView, ...);
```

### Constructor Analysis (0x1800300A0)

```cpp
TUWidgetLayoutView::TUWidgetLayoutView(
    _QWORD* this, AC_Manager* manager, QString& viewName,
    QWidget* parent, const char* objectName, int flags)
{
    // Create objectName QString
    QString objNameStr(objectName);
    
    // Initialize AC_ResponderTemplateWidget<QWidget> base
    AC_ResponderTemplateWidget<QWidget>::AC_ResponderTemplateWidget(
        this, parent, objectName, objNameStr, flags);
    
    // Initialize embedded TULayoutView at this+13*8 = this+104
    TULayoutView::TULayoutView((TULayoutView*)(this + 13));
    
    // Set up vtables
    this[0] = &TUWidgetLayoutView::vftable_QObject;
    this[2] = &TUWidgetLayoutView::vftable_QPaintDevice;
    this[5] = &TUWidgetLayoutView::vftable_AC_ResponderTemplateWidget;
    this[13] = &TUWidgetLayoutView::vftable_TULayoutView;
    
    // Set minimum width
    QWidget::setMinimumWidth(this, 150);
    
    // Initialize action manager
    if (parent && manager) {
        AC_ResponderTemplateWidget::initActionManager(this, manager);
    } else {
        this[6] = manager;  // m_actionManager at +0x30
    }
}
```

### Related Classes

- **TUVBoxLayoutView**: Similar to TUWidgetLayoutView but uses QVBoxLayout
- **TUScrollViewLayoutView**: Uses QScrollArea as base
- **TUTextEditLayoutView**: Uses QTextEdit as base
- **TUCanvasViewLayoutView**: Uses QGraphicsView as base
- **TUFrameLayoutView**: Uses QFrame as base