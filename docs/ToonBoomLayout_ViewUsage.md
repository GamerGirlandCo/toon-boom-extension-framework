# Creating and Displaying TULayoutView in Toon Boom

This document explains how to create custom views and display them in Toon Boom Harmony/Storyboard Pro using the TULayoutView system.

## Overview

Toon Boom uses a hierarchical layout system:
1. **TULayoutManager** - Central manager for all views and frames
2. **TULayoutArea** - Metadata about a view type that can be instantiated
3. **TULayoutFrame** - A window/panel containing tabbed view holders
4. **TULayoutViewHolder** - Container holding 1-2 TULayoutView instances with optional splitter
5. **TULayoutView** - Abstract base class for actual view content

## Creating Custom Views by Subclassing TULayoutView

The recommended approach is to **directly subclass `TULayoutView`** and implement the required pure virtual methods.

### Critical Discovery: How widget() Works

**IMPORTANT**: The `widget()` method (vtable slot 1) is called by `TULayoutArea::add` and the return value is treated as a `TULayoutView*`, NOT a `QWidget*`. The actual displayable widget is obtained later via `getWidget()`.

This means:
- `widget()` should return `this` (the view itself, cast to QWidget*)
- `getWidget()` should return the actual QWidget for display

### Required Pure Virtual Methods

When subclassing `TULayoutView`, you must implement these 5 pure virtual methods:

```cpp
// Slot 1: Returns THIS view as the "widget" - treated as TULayoutView* by caller
virtual QWidget *widget() = 0;

// Slots 3-4: Return the actual displayable QWidget
virtual const QWidget *getWidget() const = 0;
virtual QWidget *getWidget() = 0;

// Slot 29: Called when menus need refresh
virtual void triggerMenuChanged() = 0;

// Slot 31 (protected): Marker method
virtual void isTULayoutView() = 0;
```

### Example: Complete Custom View Implementation

```cpp
// test_frame.hpp
#pragma once
#include "toon_boom_layout.hpp"

class TestView : public TULayoutView {
public:
    TestView();
    ~TestView() override;

    // Pure virtuals - MUST implement all 5
    QWidget *widget() override;
    const QWidget *getWidget() const override;
    QWidget *getWidget() override;
    void triggerMenuChanged() override {}

    // Override initiate to return this view
    TULayoutView *initiate(QWidget *parent) override;

    // Optional overrides
    QString displayName() const override;

protected:
    void isTULayoutView() override {}

private:
    QFrame *m_frame;  // The actual widget content
    QVBoxLayout *m_mainLayout;
};
```

```cpp
// test_frame.cpp
#include "test_frame.hpp"
#include <QtWidgets/QLabel>

TestView::TestView()
    : TULayoutView() {
    // Create the frame that will hold our content
    m_frame = new QFrame();
    m_frame->setMinimumSize(400, 300);
    m_mainLayout = new QVBoxLayout(m_frame);

    QLabel *label = new QLabel("Hello from custom view!");
    label->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(label);
}

QWidget *TestView::widget() {
    // CRITICAL: TULayoutArea::add calls this via vtable[1] and expects
    // a TULayoutView* return value, NOT QWidget*. The returned pointer
    // is then used to call getWidget() for the actual widget.
    // So we return `this` which IS-A TULayoutView*.
    return reinterpret_cast<QWidget*>(static_cast<TULayoutView*>(this));
}

const QWidget *TestView::getWidget() const {
    return m_frame;
}

QWidget *TestView::getWidget() {
    return m_frame;
}

TULayoutView *TestView::initiate(QWidget *parent) {
    // Return this view - it's already initialized
    if (parent && m_frame) {
        m_frame->setParent(parent);
    }
    return this;
}

QString TestView::displayName() const {
    return QString("My Custom View");
}

TestView::~TestView() {
    delete m_frame;
}
```

## Registering Views with TULayoutManager

### TULayoutManager::addArea Signature

```cpp
bool TULayoutManager::addArea(
    const char* typeName,           // Type identifier (e.g., "Colour", "Node View")
    const QString& displayName,     // Translated display name
    TULayoutView* view,             // View instance (your TestView*)
    bool visible,                   // Initially visible
    bool createFrame,               // Create new frame for this view
    bool docked,                    // Is docked (not floating)
    const QSize& minSize,           // Minimum size
    bool useMinSize,                // Whether to use minimum size
    bool isPlugin,                  // Is this a plugin area
    bool defaultVisible,            // Default visibility state
    bool unknown                    // Unknown flag (usually true)
);
```

### Complete Registration Example

```cpp
void showCustomView() {
    auto lm = PLUG_Services::getLayoutManager();
    if (!lm) {
        return;
    }

    // Create your custom view - it's a direct TULayoutView subclass
    TestView* myView = new TestView();

    // Register with the layout manager
    bool success = lm->addArea(
        "TestView",                    // typeName (unique ID)
        QString("My Test View"),       // displayName
        myView,                        // TULayoutView* - pass directly, no offset needed!
        true,                          // visible
        true,                          // createFrame
        true,                          // docked
        QSize(500, 400),               // minSize
        true,                          // useMinSize
        false,                         // isPlugin
        true,                          // defaultVisible
        true                           // unknown
    );

    if (success) {
        // Raise the view to show it
        auto area = lm->findArea(QString("TestView"));
        if (area) {
            lm->raiseArea(area, nullptr, false, QPoint(100, 100));
        }
    }
}
```

## The Call Flow Explained

When you call `TULayoutManager::addArea`, the following happens:

1. **addArea** stores your `TULayoutView*` in a new `TULayoutArea`
2. When the view needs to be displayed, **TULayoutArea::add** is called
3. `add` calls `view->widget()` (vtable slot 1) - **expects TULayoutView* return!**
4. The return value is passed to **TULayoutViewHolder::addView**
5. `addView` calls `view->getWidget()` to get the actual QWidget
6. The QWidget is reparented and displayed

This is why `widget()` must return `this` - the calling code treats the return as `TULayoutView*` to make further virtual calls.

## View Toolbars

Some built-in views display a view-specific toolbar when they are the active tab (e.g., Drawing and Reference View).

This is driven by:
- `TULayoutView::toolbar()` returning a non-null `QDomElement` for the toolbar definition
- `TULayoutView::m_toolbarInfo` (`LAY_ToolbarInfo`) being non-default so the frame actually shows/updates the toolbar

Details and concrete implementations are documented in `docs/TULayoutView_Toolbar_Integration.md`.

## Opening Views at Runtime

To programmatically show a view that's already registered:

```cpp
// Using TULayoutManager::raiseArea
TULayoutView* view = layoutManager->raiseArea(
    QString("TestView"),     // Area name
    targetFrame,             // TULayoutFrame* (or nullptr for current)
    true,                    // Create new instance if needed
    QPoint(0, 0)             // Position hint
);
```

## Key Patterns

### 1. DPI Scaling

For views with minimum size requirements, use `UT_DPI::scale()`:

```cpp
QSize baseSize(260, 450);
QSize scaledSize = UT_DPI::scale(baseSize);
```

### 2. Common View Sizes (from HarmonyPremium analysis)

| View Type | Base Size |
|-----------|-----------|
| Colour | 260 × 450 |
| Coord. And Control Points | 260 × 300 |
| Layer Properties | 260 × 450 |
| Onion Skin | 480 × 300 |
| Timeline | 800 × 400 |
| Tool Properties | 260 × 450 |
| Top | 300 × 400 |
| Camera, Drawing, Node View, etc. | 0 × 0 (no minimum) |

### 3. Menu Registration

Set menus on views using `TULayoutView::setMenu`:

```cpp
TULayoutView::setMenu(
    layoutView,
    actionManager,      // AC_Manager*
    "MENU_ID",          // Menu identifier
    MenuType::Primary   // 0 = Primary, 1 = Secondary
);
```

## TULayoutViewHolder Usage

The `TULayoutViewHolder` is a QWidget container that can hold 1-2 TULayoutView instances with an optional vertical splitter.

### How Views Get Into TULayoutViewHolder

When you call `TULayoutManager::addArea` or `TULayoutManager::raiseArea`, the system:

1. Creates a `TULayoutFrame` if needed
2. Creates a `TULayoutViewHolder` within the frame
3. Calls `TULayoutViewHolder::addView(view, splitterRatio)` to add your view
4. The view's `getWidget()` method is called to get the actual QWidget
5. The widget is reparented and added to the holder's internal layout

### TULayoutViewHolder::addView

```cpp
bool TULayoutViewHolder::addView(
    TULayoutView* view,     // View to add
    double splitterRatio    // Ratio for splitter (default 0.5)
);
```

**Returns**: `true` if added successfully, `false` if holder is full (max 2 views)

### Internal Structure (from ToonBoomLayout.dll)

```
TULayoutViewHolder (sizeof = 0x70 = 112 bytes)
├── QWidget base class (0x00-0x27)
├── std::vector<TULayoutView*> m_views    (+0x28, 24 bytes)
├── double m_savedSplitterRatio           (+0x40, default 0.5)
├── UI_Splitter* m_splitter               (+0x48, vertical orientation)
├── WID_VBoxLayout* m_leftLayout          (+0x50)
├── WID_VBoxLayout* m_rightLayout         (+0x58)
├── QFrame* m_leftFrame                   (+0x60)
└── QFrame* m_rightFrame                  (+0x68)
```

### Behavior

- **0 views**: Splitter hidden, empty container
- **1 view**: View widget added directly to main layout, splitter hidden
- **2 views**: First view in left frame, second in right frame, splitter visible

## Important Notes

1. **Direct Subclassing**: Subclass `TULayoutView` directly - no need for complex multiple inheritance.

2. **widget() Returns this**: The `widget()` method must return `this` (cast to QWidget*) because the calling code treats it as `TULayoutView*`.

3. **getWidget() Returns Content**: The `getWidget()` method returns the actual displayable QWidget.

4. **TULayoutManager Access**: Get the TULayoutManager via `PLUG_Services::getLayoutManager()`.

5. **Memory Management**: Views registered with `addArea` are managed by the layout system. Don't delete them manually.

## Database Locations (HarmonyPremium.exe)

- **TULayoutManager::addArea import**: `0x140b22668`
- **TULayoutManager::raiseArea import**: `0x140b22a18`
- **View creation function (example)**: `0x1400375C0` (main session init)

## Database Locations (ToonBoomLayout.dll)

- **TULayoutView vtable**: `0x180056f38`
- **TULayoutView constructor**: `0x18002fc80`
- **TULayoutViewHolder constructor**: `0x180031150`
- **TULayoutViewHolder::addView**: `0x180031480`
- **TULayoutViewHolder::removeView**: `0x180031620`
- **TULayoutViewHolder::nbViews**: `0x180031610`
- **TULayoutViewHolder::splitterRatio**: `0x180031890`
- **TULayoutViewHolder::updateWidgets**: `0x1800319b0`
