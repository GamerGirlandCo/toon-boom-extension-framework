# AC_Responder System - Toolbar Button Click Handling

This document explains how toolbar button clicks are routed to responders in Toon Boom Harmony Premium and Storyboard Pro, and how to create custom responders to handle actions.

**IDA Databases:**
- `RE/ToonBoomActionManager.dll.i64` - AC_Manager and AC_Responder implementations
- `RE/ToonBoomLayout.dll.i64` - Layout responder implementations
- `RE/HarmonyPremium.exe.i64` - Application-specific responders

## Overview

Toon Boom uses a **Responder Chain** pattern (similar to macOS/Cocoa) for handling toolbar button clicks:

1. **Toolbar XML** defines button items with a `responder` attribute and a `slot` attribute
2. When clicked, `AC_Manager` looks up the responder by name
3. The responder's slot is invoked via Qt's meta-object system
4. If the responder doesn't handle it, the action propagates up the chain

## Toolbar XML Structure

From `toolbars.xml`:

```xml
<toolbar id="DrawingToolToolbar" text="Tools">
  <!-- Simple item with responder and slot -->
  <item 
    checkable="true" 
    icon="drawingtool/select.png" 
    id="SelectTool" 
    responder="sceneUI" 
    slot="onActionChooseSelectToolInNormalMode()" 
    text="Select" />
  
  <!-- Item with owner responder (view that owns the toolbar) -->
  <item 
    icon="library/refresh.png" 
    id="LIBRARY_REFRESH" 
    responder="owner" 
    slot="onActionFolderRefresh()" 
    text="Refresh" />
    
  <!-- Item with script execution -->
  <item 
    icon="script/createkeyframeson.png" 
    id="CREATE_KEYFRAMES_ON" 
    itemParameter="TB_CreateKeyFramesOn in TB_CreateKeyFramesOn.js" 
    responder="scriptResponder" 
    slot="onActionExecuteScript(QString)" 
    text="Create Keyframes On" />
</toolbar>
```

### Key XML Attributes

| Attribute | Description |
|-----------|-------------|
| `id` | Unique identifier for the toolbar item |
| `responder` | Name of the responder to handle clicks |
| `slot` | Qt slot signature to invoke |
| `icon` | Path to icon image |
| `text` | Display text / tooltip |
| `checkable` | If "true", button toggles on/off |
| `condition` | Expression for conditional visibility |
| `itemParameter` | Extra parameter passed to slot |
| `shortcut` | Keyboard shortcut name |

### Common Responder Names

| Responder | Description |
|-----------|-------------|
| `owner` | The QObject that "owns" the toolbar (usually the view) |
| `sceneUI` | Main scene UI responder (Harmony_SceneUI / SBoard_SceneUI) |
| `scene` | Scene data responder |
| `selection` | Selection responder (for cut/copy/paste) |
| `timelineView` | Timeline view responder |
| `xsheetView` | Xsheet view responder |
| `onionSkinResponder` | Onion skin feature responder |
| `scriptResponder` | Script execution responder |
| `artLayerResponder` | Art layer selection responder |

## The Responder Chain

When a toolbar button is clicked:

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. User clicks toolbar button                                   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ 2. AC_ToolbarItemImpl triggers AC_ActionInfo                    │
│    - Creates AC_ActionInfo with slot name, parameters           │
│    - Gets responder name from XML attribute                     │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ 3. AC_Manager::responder(name) looks up responder               │
│    - "owner" → toolbar's owner QObject cast to AC_Responder     │
│    - other names → registered responder by identity             │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ 4. AC_Responder::perform(AC_ActionInfo*) is called              │
│    - Sets responder in action info                              │
│    - Invokes slot via QMetaObject::invokeMethod                 │
│    - Returns AC_Result (Handled, NotHandled, Error)             │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ 5. If NotHandled, propagate up chain                            │
│    - Try parent responder via parentResponder()                 │
│    - Eventually reaches applicationResponder                    │
└─────────────────────────────────────────────────────────────────┘
```

## AC_Responder Interface

The `AC_Responder` abstract interface defines how objects participate in the responder chain:

```cpp
class AC_Responder {
public:
    virtual ~AC_Responder() = 0;
    
    // Identity
    virtual const QString& responderIdentity() const = 0;
    virtual const QString& responderDescription() const = 0;
    virtual void setResponderDescription(const QString& desc) = 0;
    
    // Chain navigation
    virtual AC_Responder* parentResponder() = 0;
    virtual AC_Responder* proxyResponder() = 0;
    
    // First responder status (keyboard focus)
    virtual bool acceptsFirstResponder() = 0;
    virtual bool becomeFirstResponder() = 0;
    virtual bool resignFirstResponder() = 0;
    
    // Selection responder status
    virtual bool acceptsSelectionResponder() = 0;
    virtual bool becomeSelectionResponder() = 0;
    virtual bool resignSelectionResponder() = 0;
    
    // Action handling
    virtual AC_Result perform(AC_ActionInfo* info) = 0;
    virtual AC_Result performDownToChildren(AC_ActionInfo* info) = 0;
    virtual bool shouldReceiveMessages() const = 0;
    virtual bool handleShortcuts() const = 0;
    
    // Event handling
    virtual AC_Result handleEvent(QEvent* event) = 0;
    
    // Manager access
    virtual AC_Manager* actionManager() const = 0;
};
```

## AC_ResponderBase Helper Class

The framework provides `AC_ResponderBase`, a concrete helper class that implements the `AC_Responder` interface with sensible defaults. This makes it easy to create custom responders without implementing every virtual method:

```cpp
// From ac_manager.hpp
class AC_ResponderBase : public AC_Responder {
public:
    AC_ResponderBase(const QString& identity, AC_Manager* manager = nullptr,
                     AC_Responder* parent = nullptr);
    virtual ~AC_ResponderBase() = default;
    
    // Identity - stored internally
    const QString& responderIdentity() const override;
    const QString& responderDescription() const override;
    void setResponderDescription(const QString& desc) override;
    
    // Chain - returns parent passed to constructor
    AC_Responder* parentResponder() override;
    AC_Responder* proxyResponder() override;  // returns nullptr
    
    // First responder - all return false by default
    bool acceptsFirstResponder() override;
    bool becomeFirstResponder() override;
    bool resignFirstResponder() override;
    
    // Selection responder - all return false by default
    bool acceptsSelectionResponder() override;
    bool becomeSelectionResponder() override;
    bool resignSelectionResponder() override;
    
    // Action handling - return NotHandled by default
    AC_Result perform(AC_ActionInfo* info) override;
    AC_Result performDownToChildren(AC_ActionInfo* info) override;
    
    // Message handling - return true by default
    bool shouldReceiveMessages() const override;
    bool handleShortcuts() const override;
    
    // Event handling - returns NotHandled
    AC_Result handleEvent(QEvent* event) override;
    
    // Manager access
    AC_Manager* actionManager() const override;
    void setActionManager(AC_Manager* manager);

protected:
    QString m_identity;
    QString m_description;
    AC_Manager* m_manager;
    AC_Responder* m_parentResponder;
};
```

## Creating a Custom Responder

### Method 1: Using AC_ResponderBase with QObject

The simplest approach is to create a class that inherits from both `QObject` (for Qt slots) and `AC_ResponderBase` (for responder functionality):

```cpp
#include <toon_boom/ac_manager.hpp>

class MyResponder : public QObject, public AC_ResponderBase {
    Q_OBJECT
    
public:
    MyResponder(const QString& identity, AC_Manager* manager, QObject* parent = nullptr)
        : QObject(parent)
        , AC_ResponderBase(identity, manager)
    {
        // Register with AC_Manager
        manager->registerResponder(this, nullptr);
    }
    
    ~MyResponder() {
        if (AC_Manager* mgr = actionManager()) {
            mgr->unregisterResponder(this);
        }
    }

public slots:
    // Slot that matches toolbar XML slot signature
    void onActionMyCustomAction() {
        qDebug() << "My custom action triggered!";
    }
    
    void onActionWithParameter(const QString& param) {
        qDebug() << "Action with parameter:" << param;
    }
    
    // Validate slot - called before action to update enabled/checked state
    void onActionMyCustomActionValidate(AC_ActionInfo* info) {
        // Enable based on some condition
        info->setEnabled(canPerformAction());
        info->setChecked(isActionActive());
    }
    
private:
    bool canPerformAction() const { return true; }
    bool isActionActive() const { return false; }
};
```

### Method 2: Widget-Based Responder

For a widget that also acts as a responder:

```cpp
#include <toon_boom/ac_manager.hpp>
#include <QWidget>

class MyCustomWidget : public QWidget, public AC_ResponderBase {
    Q_OBJECT
    
public:
    MyCustomWidget(const QString& identity, AC_Manager* manager, QWidget* parent = nullptr)
        : QWidget(parent)
        , AC_ResponderBase(identity, manager)
    {
        // Register with AC_Manager, passing 'this' as the associated widget
        manager->registerResponder(this, this);
    }
    
    ~MyCustomWidget() {
        if (AC_Manager* mgr = actionManager()) {
            mgr->unregisterResponder(this);
        }
    }

public slots:
    void onActionWidgetAction() {
        qDebug() << "Widget action triggered!";
    }
    
    void onActionWidgetActionValidate(AC_ActionInfo* info) {
        info->setEnabled(isEnabled());
    }
};
```

### Method 3: Implementing AC_Responder Directly

For full control over the responder interface:

```cpp
class MyFullResponder : public QObject, public AC_Responder {
    Q_OBJECT
    
public:
    MyFullResponder(const QString& identity, AC_Manager* manager)
        : m_identity(identity), m_manager(manager)
    {
        manager->registerResponder(this, nullptr);
    }
    
    // AC_Responder interface - implement all methods
    const QString& responderIdentity() const override { return m_identity; }
    const QString& responderDescription() const override { return m_description; }
    void setResponderDescription(const QString& desc) override { m_description = desc; }
    
    AC_Responder* parentResponder() override { return nullptr; }
    AC_Responder* proxyResponder() override { return nullptr; }
    
    bool acceptsFirstResponder() override { return false; }
    bool becomeFirstResponder() override { return false; }
    bool resignFirstResponder() override { return false; }
    
    bool acceptsSelectionResponder() override { return false; }
    bool becomeSelectionResponder() override { return false; }
    bool resignSelectionResponder() override { return false; }
    
    AC_Result perform(AC_ActionInfo* info) override {
        // Use Qt meta-object to invoke the slot by name
        // AC_ActionInfo contains slot name and parameters
        return AC_Result::NotHandled;
    }
    
    AC_Result performDownToChildren(AC_ActionInfo* info) override {
        return AC_Result::NotHandled;
    }
    
    bool shouldReceiveMessages() const override { return true; }
    bool handleShortcuts() const override { return true; }
    
    AC_Result handleEvent(QEvent*) override { return AC_Result::NotHandled; }
    
    AC_Manager* actionManager() const override { return m_manager; }

public slots:
    void onActionDoSomething() {
        // Handle the action
    }

private:
    QString m_identity;
    QString m_description;
    AC_Manager* m_manager;
};
```

## AC_ActionInfo Structure

When an action is triggered, an `AC_ActionInfo` object is created:

```cpp
class AC_ActionInfo : public QObject {
    // Key methods
    const QString& slot() const;      // Slot signature to invoke
    const QString& text() const;      // Action display text
    QVariant itemParameter() const;   // Extra parameter from XML
    
    // State methods
    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool isChecked() const;
    void setChecked(bool checked);
    
    // Responder info
    AC_Responder* responder() const;
    void setResponder(AC_Responder* resp);
};
```

## Validation Pattern

Before displaying a menu or when the UI updates, Toon Boom calls validation methods:

1. For each toolbar item, look for a slot named `<slotName>Validate(AC_ActionInfo*)`
2. If found, invoke it to update enabled/checked state
3. Display the item according to the updated state

Example from `TULayoutManager`:

```cpp
// Address: 0x7ffa0be52e60
void TULayoutManager::onActionFullscreenValidate(AC_ActionInfo* info) {
    info->setVisible(true);  // vtable[7]
    info->setEnabled(true);  // AC_ActionData::setEnabled
}
```

## Registration with AC_Manager

Responders must be registered to be found by name:

```cpp
AC_Manager* manager = PLUG_Services::getActionManager();

// Register a responder
bool success = manager->registerResponder(myResponder, myWidget);

// Unregister when done
manager->unregisterResponder(myResponder);

// Find a responder by identity
AC_Responder* resp = manager->responder("myResponderIdentity");

// Get responder for a widget
AC_Responder* widgetResp = manager->responderForWidget(someWidget);
```

## Example: Adding a Custom Toolbar Button

### Step 1: Define in XML (or load programmatically)

```xml
<toolbar id="MyToolbar" text="My Tools">
  <item 
    id="MyAction" 
    icon="my/icon.png"
    responder="myCustomResponder" 
    slot="onActionMyAction()" 
    text="My Action" />
</toolbar>
```

### Step 2: Create and Register Responder

```cpp
class MyToolResponder : public QObject, public AC_ResponderBase {
    Q_OBJECT
    
public:
    MyToolResponder(AC_Manager* manager)
        : QObject()
        , AC_ResponderBase("myCustomResponder", manager)
    {
        manager->registerResponder(this, nullptr);
    }
    
    ~MyToolResponder() {
        if (AC_Manager* mgr = actionManager()) {
            mgr->unregisterResponder(this);
        }
    }

public slots:
    void onActionMyAction() {
        qDebug() << "My custom toolbar button clicked!";
        // Do your custom action here
    }
    
    void onActionMyActionValidate(AC_ActionInfo* info) {
        info->setEnabled(true);
    }
};
```

### Step 3: Initialize

```cpp
void initializeMyToolbar() {
    AC_Manager* manager = PLUG_Services::getActionManager();
    
    // Create and register responder
    MyToolResponder* responder = new MyToolResponder(manager);
    
    // Load toolbar XML
    QDomDocument doc;
    doc.setContent(myToolbarXmlString);
    QList<QString> ids;
    manager->loadToolbars(doc.documentElement(), ids);
    
    // Show the toolbar
    TULayoutManager* layoutManager = getLayoutManager();
    layoutManager->showToolbar("MyToolbar", true);
}
```

## Owner Responder Pattern

When `responder="owner"` is used, the toolbar's owner QObject is used:

```cpp
// In AC_ToolbarImpl, owner is stored at offset +0xE0
void AC_ToolbarImpl::setOwner(QObject* owner) {
    m_owner = owner;
    connect(owner, &QObject::destroyed, this, &AC_ToolbarImpl::ownerDestroyed);
}

// When resolving "owner" responder:
if (responderName == "owner") {
    QObject* owner = toolbar->owner();
    return qobject_cast<AC_Responder*>(owner);
}
```

This pattern is commonly used for view-specific toolbars where the view itself handles the actions.

## Key Addresses

### ToonBoomActionManager.dll
| Symbol | Address | Description |
|--------|---------|-------------|
| AC_ManagerImpl::registerResponder | (via vtable) | Registers responder |
| AC_ManagerImpl::unregisterResponder | (via vtable) | Unregisters responder |
| AC_ManagerImpl::responder | (via vtable) | Finds responder by name |

## Summary

To respond to toolbar button clicks:

1. **Create a class** that inherits from `QObject` and `AC_ResponderBase` (or implement `AC_Responder` directly)
2. **Register** with `AC_Manager::registerResponder(responder, widget)`
3. **Define slots** matching the toolbar XML `slot` attribute signatures
4. **Optionally define validate slots** (`<slotName>Validate(AC_ActionInfo*)`) for state updates
5. **Reference your responder** by identity in the toolbar XML `responder` attribute
