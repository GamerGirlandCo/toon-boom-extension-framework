# TUWidgetLayoutView Class Analysis

This document contains the reverse engineering analysis of the `TUWidgetLayoutView` class from `ToonBoomLayout.dll` used in Toon Boom Harmony Premium and Storyboard Pro.

## Overview

`TUWidgetLayoutView` is a concrete implementation that combines:
- **QWidget** - for Qt UI rendering
- **AC_ResponderTemplateWidget<QWidget>** - for action/responder chain handling  
- **TULayoutView** - for the Toon Boom layout system integration

This class serves as the base for most view types in Toon Boom's panel/window system.

## Class Hierarchy

```
QObject
└── QWidget
    └── AC_ResponderTemplateWidget<QWidget>
        └── TUWidgetLayoutView
            └── [TULayoutView embedded at offset +104]
```

Note: `TULayoutView` is not a base class in the C++ inheritance sense but is **embedded** as a sub-object at offset +104. This is a form of composition that allows `TULayoutView` to have its own vtable and virtual methods.

## Memory Layout (x64 MSVC)

| Offset (hex) | Offset (dec) | Size | Member |
|--------------|--------------|------|--------|
| +0x00 | +0 | 8 | vptr (QObject) |
| +0x08 | +8 | 8 | QObjectData* d_ptr |
| +0x10 | +16 | 8 | vptr (QPaintDevice) |
| +0x18 | +24 | 16 | QWidget internal data |
| +0x28 | +40 | 8 | vptr (AC_ResponderTemplateWidget<QWidget>) |
| +0x30 | +48 | 8 | AC_Manager* m_actionManager |
| +0x38 | +56 | 24 | QString m_responderIdentity |
| +0x50 | +80 | 24 | QString m_responderDescription |
| **+0x68** | **+104** | 8 | **vptr (TULayoutView)** - TULayoutView starts here |
| +0x70 | +112 | 24 | QString m_internalName (TULayoutView) |
| +0x88 | +136 | 104 | LAY_ToolbarInfo m_toolbarInfo (TULayoutView) |
| +0xF0 | +240 | 16 | AC_Menu* m_menuByType[2] (TULayoutView) |
| +0x100 | +256 | 1+7 | bool m_initializedFromCopy + padding (TULayoutView) |
| +0x108 | +264 | 24 | QString m_caption (TULayoutView) |

**Total size: 0x120 (288 bytes)**

## VTable Structure

TUWidgetLayoutView has **4 vtables** due to multiple inheritance:

### vptr[0] at +0x00: QObject vtable
Contains all QObject and QWidget virtual methods including:
- `metaObject()`, `qt_metacast()`, `qt_metacall()`
- `event()`, `eventFilter()`
- `paintEvent()`, `mousePressEvent()`, etc.

Address in ToonBoomLayout.dll: `??_7TUWidgetLayoutView@@6BQObject@@@`

### vptr[1] at +0x10: QPaintDevice vtable  
Contains QPaintDevice virtuals:
- `devType()`
- `paintEngine()`
- `metric()`

Address in ToonBoomLayout.dll: `??_7TUWidgetLayoutView@@6BQPaintDevice@@@`

### vptr[2] at +0x28: AC_ResponderTemplateWidget<QWidget> vtable
Contains AC_Responder interface methods:
- `perform(AC_ActionInfo*)`
- `performDownToChildren(AC_ActionInfo*)`
- `parentResponder()`
- `proxyResponder()`
- `acceptsFirstResponder()`
- `becomeFirstResponder()`
- `resignFirstResponder()`
- `handleShortcuts()`
- `shouldReceiveMessages()`
- `responderIdentity()`
- `responderDescription()`
- `actionManager()`
- `handleEvent(QEvent*)`

Address in ToonBoomLayout.dll: `??_7TUWidgetLayoutView@@6B?$AC_ResponderTemplateWidget@VQWidget@@@@@`

### vptr[3] at +0x68: TULayoutView vtable
Contains TULayoutView virtual methods (32 slots total):

| Slot | Method | TUWidgetLayoutView Implementation |
|------|--------|-----------------------------------|
| 0 | ~TULayoutView() | Thunk to TUWidgetLayoutView dtor |
| 1 | widget() | _purecall (pure virtual) |
| 2 | initiate(QWidget*) | TULayoutView::initiate |
| 3 | getWidget() const | **Returns (this - 104)** |
| 4 | getWidget() | **Returns (this - 104)** |
| 5 | getParentHolderWidget() const | TULayoutView impl |
| 6 | getParentHolderWidget() | TULayoutView impl |
| 7 | hasMenu() | TULayoutView impl |
| 8 | setMenu(AC_Manager*, const char*, MenuType) | TULayoutView impl |
| 9 | setMenu(AC_Menu*, MenuType) | TULayoutView impl |
| 10 | menu(MenuType) | TULayoutView impl |
| 11 | toolbar() | TULayoutView impl |
| 12 | setToolbarInfo(LAY_ToolbarInfo&) | TULayoutView impl |
| 13 | connectView() | Empty |
| 14 | disconnectView() | Empty |
| 15 | initializedFromCopy() | TULayoutView impl |
| 16 | getCaption(bool) | TULayoutView impl |
| 17 | getDynamicTextForCaption() | TULayoutView impl |
| 18 | wantEditionStack() | Returns false |
| 19 | displayName() | TULayoutView impl |
| 20 | compositeChanged(QString&) | Empty |
| 21 | dropOverComposite(QDropEvent*, QString&) | Empty |
| 22 | wantComposites() | Returns false |
| 23 | initActionManager(AC_Manager*) | Empty |
| 24 | wantDisplaySelector() | Returns false |
| 25 | isUsingDefaultDisplay() | Returns false |
| 26 | storeViewPreferences(QDomElement&) | Returns false |
| 27 | loadViewPreferences(QDomElement&) | Empty |
| 28 | cshHelpId() | TULayoutView impl |
| 29 | **triggerMenuChanged()** | **Emits menuChanged() signal** |
| 30 | copy(TULayoutView&) | TULayoutView impl |
| 31 | **isTULayoutView()** | **Empty (RTTI marker)** |

Address in ToonBoomLayout.dll: `??_7TUWidgetLayoutView@@6BTULayoutView@@@`

## Key Methods

### Constructor
```cpp
TUWidgetLayoutView(
    AC_Manager* manager,         // Action manager (stored at +0x30)
    const QString& objectName,   // Qt object name
    QWidget* parent,             // Parent widget
    const char* className,       // Class name string
    Qt::WindowFlags flags        // Window flags
)
```

**Construction sequence:**
1. Call `AC_ResponderTemplateWidget<QWidget>` ctor with (parent, flags, objectName)
2. Call `TULayoutView` default ctor at `this + 104`
3. Install all 4 TUWidgetLayoutView vtables
4. Set minimum width to 150 pixels
5. If parent != nullptr && manager != nullptr:
   - Call `initActionManager(manager)` 
6. Else:
   - Store manager directly at offset +0x30

### Destructor
**Destruction sequence:**
1. Reset vtables to TUWidgetLayoutView vtables
2. Destroy `TULayoutView::m_caption` (QString at +0x108)
3. Destroy `TULayoutView::m_toolbarInfo` (LAY_ToolbarInfo at +0x88)
4. Destroy `TULayoutView::m_internalName` (QString at +0x70)
5. Jump to `AC_ResponderTemplateWidget<QWidget>::~AC_ResponderTemplateWidget`

### getWidget()
```cpp
QWidget* getWidget() {
    return reinterpret_cast<QWidget*>(
        reinterpret_cast<char*>(this) - 104);
}
```
Called from TULayoutView* context (at +104), returns the containing QWidget* (at +0).

### triggerMenuChanged()
```cpp
void triggerMenuChanged() {
    // 'this' is TULayoutView* at +104
    // Emit signal on containing TUWidgetLayoutView (this - 104)
    TUWidgetLayoutView* widget = (TUWidgetLayoutView*)((char*)this - 104);
    widget->menuChanged();
}
```

### mousePressEvent()
```cpp
void mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    if (!event->isAccepted()) {
        event->accept();
        setFocus(Qt::MouseFocusReason);  // Focus reason = 7
    }
}
```

## Qt Meta-Object System

TUWidgetLayoutView participates in the Qt meta-object system:

- `staticMetaObject` - Static meta-object for the class
- `metaObject()` - Returns `&staticMetaObject` (or dynamic for QML)
- `qt_metacast(const char* className)`:
  - "TUWidgetLayoutView" → returns `this`
  - "TULayoutView" → returns `this + 104`
  - Otherwise → delegates to AC_ResponderTemplateWidget<QWidget>::qt_metacast
- `tr()` - Static translation method

### Signals
- `menuChanged()` - Emitted when view menu changes (slot 0 in staticMetaObject signals)

## Exported Symbols

From ToonBoomLayout.dll:

| Symbol | Description |
|--------|-------------|
| `??0TUWidgetLayoutView@@QEAA@...@Z` | Constructor |
| `??1TUWidgetLayoutView@@UEAA@XZ` | Destructor |
| `?getWidget@TUWidgetLayoutView@@UEAAPEAVQWidget@@XZ` | Non-const getWidget |
| `?getWidget@TUWidgetLayoutView@@UEBAPEBVQWidget@@XZ` | Const getWidget |
| `?mousePressEvent@TUWidgetLayoutView@@MEAAXPEAVQMouseEvent@@@Z` | Mouse handler |
| `?triggerMenuChanged@TUWidgetLayoutView@@MEAAXXZ` | Trigger menu signal |
| `?menuChanged@TUWidgetLayoutView@@QEAAXXZ` | Signal implementation |
| `?metaObject@TUWidgetLayoutView@@UEBAPEBUQMetaObject@@XZ` | Meta-object |
| `?qt_metacast@TUWidgetLayoutView@@UEAAPEAXPEBD@Z` | Runtime cast |
| `?qt_metacall@TUWidgetLayoutView@@UEAAHW4Call@QMetaObject@@HPEAPEAX@Z` | Meta-call |
| `?qt_static_metacall@TUWidgetLayoutView@@...@Z` | Static meta-call |
| `?tr@TUWidgetLayoutView@@SA?AVQString@@PEBD0H@Z` | Translation |

## Helper Functions

The header provides inline helper functions for pointer conversion:

```cpp
// Get TULayoutView* from TUWidgetLayoutView*
TULayoutView* TUWidgetLayoutView_getLayoutView(TUWidgetLayoutView* widget);

// Get TUWidgetLayoutView* from TULayoutView* (must be embedded in TUWidgetLayoutView)
TUWidgetLayoutView* TULayoutView_getWidgetLayoutView(TULayoutView* view);

// Get QWidget* from TUWidgetLayoutView* (same address, different type)
QWidget* TUWidgetLayoutView_getWidget(TUWidgetLayoutView* widget);

// Get AC_Manager* from TULayoutView* embedded in TUWidgetLayoutView
AC_Manager* TULayoutView_getActionManager(TULayoutView* view);
```

## Subclasses

Several classes in ToonBoomLayout.dll inherit from TUWidgetLayoutView:

- `TUCanvasViewLayoutView` - Canvas/drawing view
- `TUFrameLayoutView` - Frame view
- `TUScrollViewLayoutView` - Scrollable view
- `TUTextEditLayoutView` - Text editing view  
- `TUVBoxLayoutView` - Vertical box layout view

These share the same `getWidget()` implementation (returning `this - 104`).

## Analysis Methodology

This analysis was performed using:
1. IDA Pro decompilation of constructor (`0x7ffa0be600a0`) and destructor (`0x7ffa0be60480`)
2. Disassembly verification of exact byte offsets
3. Cross-reference analysis of vtable addresses
4. Examination of related methods (getWidget, triggerMenuChanged, etc.)
5. Qt meta-object method analysis (qt_metacast, etc.)

## References

- `toon_boom_layout.hpp` - Header file with class declarations
- `TULayoutView_Toolbar_Integration.md` - Toolbar integration documentation
- `AC_Manager` - Action manager class documentation
