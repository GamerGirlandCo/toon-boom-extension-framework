/**
 * @file toolbar.hpp
 * @brief Reconstructed header for Toon Boom AC_Toolbar class hierarchy
 *
 * This header was reverse-engineered from ToonBoomActionManager.dll
 * IDA Database: RE/ToonBoomActionManager.dll.i64
 *
 * AC_Toolbar provides customizable toolbar functionality for Toon Boom
 * applications. It supports:
 * - Dynamic item insertion (buttons, multi-buttons, comboboxes, separators)
 * - Placeholder-based customization
 * - Configuration save/restore
 * - Integration with AC_Manager for action handling
 *
 * @note All offsets and structures derived from decompilation analysis.
 *       This is NOT official Toon Boom code.
 */

#pragma once

#include "./ac_manager.hpp"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtGui/QAction>
#include <QtXml/QDomElement>

// Forward declarations
class AC_Object;
class AC_Item;
class AC_Separator;
class AC_Container;
class AC_ContainerImpl;
class AC_Toolbar;
class AC_ToolbarImpl;
class AC_ToolbarItemGenerator;
class AC_Help;

/**
 * @brief Abstract base class for toolbar objects (items, separators, etc.)
 *
 * AC_Object is the base interface for all items that can be contained
 * within an AC_Container (toolbar, menu, etc.).
 *
 * vtable at 0x18004ed88 (16 virtual methods, most pure virtual)
 */
class AC_Object {
public:
    virtual ~AC_Object() = 0;

    /**
     * @brief Get the identity string for this object
     * @return Reference to identity QString
     */
    virtual const QString& identity() const = 0;

    /**
     * @brief Check if this object is visible
     * @return true if visible
     */
    virtual bool isVisible() const = 0;

    /**
     * @brief Set visibility of this object
     * @param visible New visibility state
     */
    virtual void setVisible(bool visible) = 0;

    /**
     * @brief Check if this object is enabled
     * @return true if enabled
     */
    virtual bool isEnabled() const = 0;

    /**
     * @brief Set enabled state of this object
     * @param enabled New enabled state
     */
    virtual void setEnabled(bool enabled) = 0;

    /**
     * @brief Cast to AC_Item if this is an item
     * @return Pointer to AC_Item or nullptr
     */
    virtual const AC_Item* toItem() const = 0;

    /**
     * @brief Cast to AC_Separator if this is a separator
     * @return Pointer to AC_Separator or nullptr
     */
    virtual const AC_Separator* toSeparator() const = 0;

    /**
     * @brief Cast to AC_Toolbar if this is a toolbar
     * @return Pointer to AC_Toolbar or nullptr
     */
    virtual const AC_Toolbar* toToolbar() const = 0;

    // Additional virtual methods exist but are mostly pure virtual stubs
};

/**
 * @brief Abstract interface for toolbar items (buttons, etc.)
 *
 * AC_Item extends AC_Object with item-specific functionality like
 * checked state and action association.
 */
class AC_Item : public AC_Object {
public:
    virtual ~AC_Item() = 0;

    /**
     * @brief Check if this item is checked (for toggle buttons)
     * @return true if checked
     */
    virtual bool isChecked() const = 0;

    /**
     * @brief Set the checked state
     * @param checked New checked state
     */
    virtual void setChecked(bool checked) = 0;

    /**
     * @brief Get the associated QAction
     * @return QAction pointer or nullptr
     */
    virtual QAction* action() const = 0;
};

/**
 * @brief Abstract interface for toolbar separators
 */
class AC_Separator : public AC_Object {
public:
    virtual ~AC_Separator() = 0;
};

/**
 * @brief Abstract interface for containers that hold AC_Objects
 *
 * AC_Container provides methods for managing child objects within
 * a toolbar or similar container.
 *
 * vtable at 0x180054ef8 (AC_ToolbarImpl's AC_Container vtable)
 */
class AC_Container {
public:
    virtual ~AC_Container() = 0;

    /**
     * @brief Get the number of objects in this container
     * @return Object count
     */
    virtual int objectCount() const = 0;

    /**
     * @brief Get object at specified index
     * @param index Zero-based index
     * @return AC_Object at index or nullptr
     */
    virtual AC_Object* objectAtIndex(int index) const = 0;

    /**
     * @brief Find object by identity
     * @param identity Identity string to find
     * @return Found object or nullptr
     */
    virtual AC_Object* findObject(const char* identity) const = 0;

    /**
     * @brief Find item by identity
     * @param identity Identity string to find
     * @return Found item or nullptr
     */
    virtual AC_Item* findItem(const char* identity) const = 0;

    /**
     * @brief Get index of an object
     * @param object Object to find
     * @return Zero-based index or -1 if not found
     */
    virtual int indexOf(AC_Object* object) const = 0;

    /**
     * @brief Get index of object by identity
     * @param identity Identity string to find
     * @return Zero-based index or -1 if not found
     */
    virtual int indexOf(const char* identity) const = 0;

    /**
     * @brief Check if item is checked
     * @param identity Item identity
     * @return true if item exists and is checked
     */
    virtual bool isChecked(const char* identity) const = 0;

    /**
     * @brief Set checked state of item
     * @param identity Item identity
     * @param checked New checked state
     */
    virtual void setChecked(const char* identity, bool checked) = 0;
};

/**
 * @brief Context-sensitive help interface
 *
 * AC_Help provides methods for context-sensitive help in toolbars.
 *
 * vtable at 0x1800550f8 (AC_ToolbarImpl's AC_Help vtable)
 */
class AC_Help {
public:
    virtual ~AC_Help();

    /**
     * @brief Help type enumeration
     */
    enum cshHelpType {
        Type0 = 0,
        Type1 = 1,
        Type2 = 2,
        Type3 = 3,
        Type4 = 4,
        ToolbarHelp = 5  // Used by AC_ToolbarImpl::cshHelpId
    };

    /**
     * @brief Get context-sensitive help ID
     * @return Help ID string
     */
    virtual QString cshHelpId() = 0;

    /**
     * @brief Get help prefix for a type
     * @param type Help type
     * @return Prefix string
     */
    static QString cshHelpPref(cshHelpType type);

    /**
     * @brief Get help shortcut string
     * @return Shortcut string
     */
    static QString cshHelpShortcut();
};

/**
 * @brief Abstract base class for toolbars
 *
 * AC_Toolbar is the abstract interface for toolbar functionality.
 * The concrete implementation is AC_ToolbarImpl.
 *
 * Key features:
 * - Item management (insert, remove, find)
 * - Configuration save/restore
 * - Placeholder system for customization
 * - Integration with AC_Manager
 *
 * IMPORTANT: This class does NOT use C++ multiple inheritance even though
 * conceptually it includes AC_Object, AC_Container, and AC_Help functionality.
 * The vtable is a flat structure with all 66 methods in the exact order
 * shown below. DO NOT change the order of virtual methods.
 *
 * vtable at 0x180054ef8 (66 entries, accessed at offset +128 from AC_ToolbarImpl*)
 *
 * When you get an AC_Toolbar* from AC_Manager::createToolbar(), it points
 * to offset +128 within the AC_ToolbarImpl object.
 */
class AC_Toolbar {
public:
    // =========================================================================
    // Vtable slots 0-5: Destructor and basic AC_Object properties
    // =========================================================================

    /** @brief Slot 0: Virtual destructor */
    virtual ~AC_Toolbar() = 0;

    /** @brief Slot 1: Get the toolbar identity string */
    virtual const QString& identity() const = 0;

    /** @brief Slot 2: Check if toolbar is enabled */
    virtual bool isEnabled() const = 0;

    /** @brief Slot 3: Set toolbar enabled state */
    virtual void setEnabled(bool enabled) = 0;

    /** @brief Slot 4: Check if toolbar is visible */
    virtual bool isVisible() const = 0;

    /** @brief Slot 5: Set toolbar visibility */
    virtual void setVisible(bool visible) = 0;

    // =========================================================================
    // Vtable slots 6-12: Type checking (AC_Object type system)
    // =========================================================================

    /** @brief Slot 6: Check if this is a toolbar (returns true) */
    virtual bool isToolbar() const = 0;

    /** @brief Slot 7: Reserved (returns false) */
    virtual bool _reserved_slot7() const = 0;

    /** @brief Slot 8: Reserved (returns false) */
    virtual bool _reserved_slot8() const = 0;

    /** @brief Slot 9: Reserved (returns false) */
    virtual bool _reserved_slot9() const = 0;

    /** @brief Slot 10: Reserved (returns false) */
    virtual bool _reserved_slot10() const = 0;

    /** @brief Slot 11: Reserved (returns false) */
    virtual bool _reserved_slot11() const = 0;

    /** @brief Slot 12: Check if this is a container (returns true) */
    virtual bool isContainer() const = 0;

    // =========================================================================
    // Vtable slots 13-26: Type conversion (AC_Object casting)
    // =========================================================================

    /** @brief Slot 13: Cast to AC_Toolbar (const) */
    virtual const AC_Toolbar* toToolbar_const1() const = 0;

    /** @brief Slot 14: Cast to AC_Toolbar (const variant) */
    virtual const AC_Toolbar* toToolbar_const2() const = 0;

    /** @brief Slot 15-24: Cast to AC_Separator (returns nullptr, 10 variants) */
    virtual const AC_Separator* toSeparator_v1() const = 0;
    virtual const AC_Separator* toSeparator_v2() const = 0;
    virtual const AC_Separator* toSeparator_v3() const = 0;
    virtual const AC_Separator* toSeparator_v4() const = 0;
    virtual const AC_Separator* toSeparator_v5() const = 0;
    virtual const AC_Separator* toSeparator_v6() const = 0;
    virtual const AC_Separator* toSeparator_v7() const = 0;
    virtual const AC_Separator* toSeparator_v8() const = 0;
    virtual const AC_Separator* toSeparator_v9() const = 0;
    virtual const AC_Separator* toSeparator_v10() const = 0;

    /** @brief Slot 25: Cast to AC_Toolbar (non-const) */
    virtual const AC_Toolbar* toToolbar_v1() const = 0;

    /** @brief Slot 26: Cast to AC_Toolbar (non-const variant) */
    virtual const AC_Toolbar* toToolbar_v2() const = 0;

    // =========================================================================
    // Vtable slots 27-32: Core toolbar operations
    // =========================================================================

    /** @brief Slot 27: Get the associated AC_Manager */
    virtual AC_Manager* manager() const = 0;

    /** @brief Slot 28: Remove the toolbar from its parent */
    virtual void remove() = 0;

    /** @brief Slot 29: Get as QToolBar */
    virtual QToolBar* toQToolBar() = 0;

    /**
     * @brief Slot 30: Insert an object from XML element
     * @param beforeObject Insert before this object (nullptr = end)
     * @param element XML element defining the item
     * @return Inserted AC_Object or nullptr
     */
    virtual AC_Object* insert(AC_Object* beforeObject, const QDomElement& element) = 0;

    /**
     * @brief Slot 31: Insert placeholder contents
     * @param beforeObject Insert before this object
     * @param responderIdentity Responder identity
     * @param actionName Action name
     * @param objectName Object name
     * @param keywords Keywords list
     * @param visible Visibility
     * @param flag Additional flag
     * @return Inserted AC_Item or nullptr
     */
    virtual AC_Item* insertPlaceHolderContents(AC_Object* beforeObject,
                                                const QString& responderIdentity,
                                                const QString& actionName,
                                                const char* objectName,
                                                const QList<QString>* keywords,
                                                bool visible,
                                                bool flag) = 0;

    /**
     * @brief Slot 32: Insert a separator before an object
     * @param beforeObject Insert before this object
     * @return Created separator or nullptr
     */
    virtual AC_Separator* insertSeparator(AC_Object* beforeObject) = 0;

    // =========================================================================
    // Vtable slots 33-41: AC_Container interface
    // =========================================================================

    /**
     * @brief Slot 33: Find object by identity
     * @param identity Identity string to find
     * @return Found object or nullptr
     */
    virtual AC_Object* findObject(const char* identity) const = 0;

    /**
     * @brief Slot 34: Find item by identity
     * @param identity Identity string to find
     * @return Found item or nullptr
     */
    virtual AC_Item* findItem(const char* identity) const = 0;

    /**
     * @brief Slot 35: Remove an object by identity
     * @param identity Object identity to remove
     * @return true if removed successfully
     */
    virtual bool removeObject(const char* identity) = 0;

    /**
     * @brief Slot 36: Get index of object by identity
     * @param identity Identity string to find
     * @return Zero-based index or -1 if not found
     */
    virtual int indexOf(const char* identity) const = 0;

    /**
     * @brief Slot 37: Get index of an object
     * @param object Object to find
     * @return Zero-based index or -1 if not found
     */
    virtual int indexOf(AC_Object* object) const = 0;

    /**
     * @brief Slot 38: Get the number of objects in this container
     * @return Object count
     */
    virtual int objectCount() const = 0;

    /**
     * @brief Slot 39: Get object at specified index
     * @param index Zero-based index
     * @return AC_Object at index or nullptr
     */
    virtual AC_Object* objectAtIndex(int index) const = 0;

    /**
     * @brief Slot 40: Check if item is checked
     * @param identity Item identity
     * @return true if item exists and is checked
     */
    virtual bool isChecked(const char* identity) const = 0;

    /**
     * @brief Slot 41: Set checked state of item
     * @param identity Item identity
     * @param checked New checked state
     */
    virtual void setChecked(const char* identity, bool checked) = 0;

    // =========================================================================
    // Vtable slots 42-49: Validation and configuration
    // =========================================================================

    /** @brief Slot 42: Validate all content (enable/disable states) */
    virtual void validateContent() = 0;

    /**
     * @brief Slot 43: Open customization dialog
     * @param parent Parent widget for dialog
     */
    virtual void customize(QWidget* parent) = 0;

    /**
     * @brief Slot 44: Set configuration from list
     * @param config List of item identities
     */
    virtual void setConfig(const QList<QString>& config) = 0;

    /**
     * @brief Slot 45: Set default configuration
     * @param config List of item identities for default
     */
    virtual void setDefaultConfig(const QList<QString>& config) = 0;

    /**
     * @brief Slot 46: Get current configuration
     * @return List of item identities in current order
     */
    virtual QList<QString> config() const = 0;

    /**
     * @brief Slot 47: Get default configuration
     * @return List of item identities in default order
     */
    virtual QList<QString> defaultConfig() const = 0;

    /**
     * @brief Slot 48: Check if current config matches default
     * @return true if config is default
     */
    virtual bool isDefaultConfig() const = 0;

    /**
     * @brief Slot 49: Check if a button is in default config
     * @param identity Button identity
     * @return true if button is default
     */
    virtual bool isDefaultButton(const QString& identity) const = 0;

    // =========================================================================
    // Vtable slots 50-57: Text, generators, and ownership
    // =========================================================================

    /** @brief Slot 50: Get the toolbar text/title */
    virtual QString text() const = 0;

    /** @brief Slot 51: Set the toolbar text/title */
    virtual void setText(const QString& text) = 0;

    /** @brief Slot 52: Get the toolbar item generator */
    virtual AC_ToolbarItemGenerator* itemGenerator() const = 0;

    /** @brief Slot 53: Set the toolbar item generator */
    virtual void setItemGenerator(AC_ToolbarItemGenerator* generator) = 0;

    /** @brief Slot 54: Check if toolbar is customizable */
    virtual bool isCustomizable() const = 0;

    /** @brief Slot 55: Get as QToolBar (duplicate of slot 29) */
    virtual QToolBar* toQToolBar_v2() = 0;

    /** @brief Slot 56: Get the owner object */
    virtual QObject* owner() const = 0;

    /** @brief Slot 57: Set the owner object */
    virtual void setOwner(QObject* owner) = 0;

    // =========================================================================
    // Vtable slots 58-62: Updates and content changes
    // =========================================================================

    /**
     * @brief Slot 58: Connect update signals
     * @param receiver Receiver object
     * @param slot Slot to connect
     */
    virtual void connectUpdate(QObject* receiver, const char* slot) = 0;

    /**
     * @brief Slot 59: Change toolbar content from XML
     * @param element New XML definition
     * @param addedIds Output list of added IDs
     * @param removedIds Output list of removed IDs
     * @return true if changed successfully
     */
    virtual bool changeContent(const QDomElement& element,
                                QList<QString>* addedIds,
                                QList<QString>* removedIds) = 0;

    /**
     * @brief Slot 60: Change toolbar content from name
     * @param name Toolbar definition name
     * @param addedIds Output list of added IDs
     * @param removedIds Output list of removed IDs
     * @return true if changed successfully
     */
    virtual bool changeContent(const QString& name,
                                QList<QString>* addedIds,
                                QList<QString>* removedIds) = 0;

    /**
     * @brief Slot 61: Translate a string using toolbar's context
     * @param text Text to translate
     * @return Translated string
     */
    virtual QString translate(const QString& text) = 0;

    /**
     * @brief Slot 62: Update customized button states
     * @param identity Button identity
     * @param oldName Old name
     * @param newName New name
     */
    virtual void updateCustomizedButtons(const QString& identity,
                                          const QString& oldName,
                                          const QString& newName) = 0;

    // =========================================================================
    // Vtable slots 63-65: Validation helpers and help
    // =========================================================================

    /** @brief Slot 63: Validate content if toolbar is visible */
    virtual void validateContentIfVisible() = 0;

    /** @brief Slot 64: Validate tooltip text if toolbar is visible */
    virtual void validateTooltipTextIfVisible() = 0;

    /** @brief Slot 65: Get context-sensitive help ID */
    virtual QString cshHelpId() = 0;

    // =========================================================================
    // Convenience methods (non-virtual, implemented in terms of above)
    // =========================================================================

    /** @brief Get as QWidget (calls toQToolBar and casts) */
    inline QWidget* toQWidget() { return static_cast<QWidget*>(toQToolBar()); }

    /** @brief Cast to AC_Toolbar (returns this) */
    inline const AC_Toolbar* toToolbar() const { return toToolbar_const1(); }

    /** @brief Cast to AC_Separator (returns nullptr for toolbars) */
    inline const AC_Separator* toSeparator() const { return toSeparator_v1(); }
};

/**
 * @brief Concrete implementation of AC_Container
 *
 * AC_ContainerImpl provides the base container functionality used by
 * AC_ToolbarImpl and other container classes.
 *
 * sizeof(AC_ContainerImpl) = 0x58 (88 bytes) on x64
 *
 * Memory layout:
 * - +0x00: vptr (AC_ContainerImpl vtable)
 * - +0x08: AC_ManagerImpl* m_manager
 * - +0x10: QString m_identity (24 bytes)
 * - +0x28: bool m_enabled
 * - +0x29: bool m_isToolbar (flag)
 * - +0x30: std::vector<AC_Object*> m_objects
 * - +0x48: Internal tree/map for placeholders
 */
class AC_ContainerImpl {
public:
    /**
     * @brief Construct container with manager
     * @param manager Owning manager
     * @param isToolbar true if this is a toolbar container
     */
    AC_ContainerImpl(AC_ManagerImpl* manager, bool isToolbar);
    AC_ContainerImpl(const AC_ContainerImpl& other);
    virtual ~AC_ContainerImpl();

    AC_ContainerImpl& operator=(const AC_ContainerImpl& other);

    // AC_Container interface implementation
    int objectCount() const;
    AC_Object* objectAtIndex(int index) const;
    AC_Object* findObject(const char* identity) const;
    AC_Item* findItem(const char* identity) const;
    int indexOf(AC_Object* object) const;
    int indexOf(const char* identity) const;
    bool isChecked(const char* identity) const;
    void setChecked(const char* identity, bool checked);

    /**
     * @brief Get all objects
     * @return Reference to objects vector
     */
    const std::vector<AC_Object*>& objects() const;

    /**
     * @brief Update objects from external vector
     * @param newObjects New objects to set
     */
    void updateObjects(const std::vector<AC_Object*>& newObjects);

    /**
     * @brief Handle child object destruction
     * @param object Destroyed object
     */
    virtual void childDestroyed(AC_Object* object);

    /**
     * @brief Handle child QObject destruction
     * @param object Destroyed QObject
     */
    virtual void childDestroyed(QObject* object);

    /**
     * @brief Handle child insertion
     * @param object Inserted object
     * @param index Insertion index
     */
    virtual void childInsert(AC_Object* object, int index);
};

/**
 * @brief Concrete implementation of AC_Toolbar
 *
 * AC_ToolbarImpl inherits from QToolBar and implements AC_Toolbar functionality.
 * It provides full toolbar functionality including:
 * - Dynamic item management
 * - Customization support
 * - Configuration persistence
 * - Context-sensitive help
 *
 * sizeof(AC_ToolbarImpl) = 0x118 (280 bytes) on x64
 *
 * Memory layout (from constructor analysis at 0x180032df0):
 * - +0x00: QToolBar base (QObject vptr)
 * - +0x10: QPaintDevice vptr
 * - +0x28: AC_ContainerImpl embedded (has its own vptr at 0x180054eb0)
 * - +0x80: AC_Toolbar interface vptr (0x180054ef8) <- AC_Toolbar* points here
 * - +0x88: AC_Help vptr (0x1800550f8)
 * - +0x98: QString m_responderIdentity (24 bytes)
 * - +0xB0: std::vector<AC_Object*> m_defaultConfig (24 bytes)
 * - +0xC8: bool m_isCustomizable
 * - +0xD0: void* m_customizeButton
 * - +0xD8: void* m_unknown1
 * - +0xE0: QObject* m_owner
 * - +0xE8: QWeakPointer<QWidget> m_mainWindow (16 bytes)
 * - +0xF8: float m_scaleFactor (default 1.0f = 0x3F800000)
 * - +0x100: QString m_translationContext (24 bytes)
 *
 * Vtable addresses (from constructor):
 * - QObject vtable:        0x180054c90 (at offset +0x00)
 * - QPaintDevice vtable:   0x180054e70 (at offset +0x10)
 * - AC_ContainerImpl vptr: 0x180054eb0 (at offset +0x28, internal use)
 * - AC_Toolbar vtable:     0x180054ef8 (at offset +0x80, returned by createToolbar)
 * - AC_Help vtable:        0x1800550f8 (at offset +0x88)
 *
 * Signals:
 * - customized(QString) - emitted when toolbar is customized
 */
class AC_ToolbarImpl : public QToolBar {
    Q_OBJECT

public:
    /**
     * @brief Construct AC_ToolbarImpl
     * @param owner Owner QObject
     * @param title Toolbar title
     * @param mainWindow Parent main window
     * @param objectName Object name (C-string)
     * @param manager Owning AC_ManagerImpl
     * @param element XML element defining toolbar content
     * @param defaultConfig Default configuration (may be nullptr)
     */
    AC_ToolbarImpl(QObject* owner, const QString& title, QMainWindow* mainWindow,
                    const char* objectName, AC_ManagerImpl* manager,
                    const QDomElement& element, const QList<QString>* defaultConfig);
    virtual ~AC_ToolbarImpl() override;

    // =========================================================================
    // AC_Object interface (note: setVisible overrides QWidget::setVisible)
    // =========================================================================
    const QString& identity() const;
    bool isVisible() const;
    void setVisible(bool visible) override;  // Overrides QWidget::setVisible
    bool isEnabled() const;
    void setEnabled(bool enabled);
    const AC_Item* toItem() const { return nullptr; }
    const AC_Separator* toSeparator() const { return nullptr; }
    const AC_Toolbar* toToolbar() const;

    // =========================================================================
    // AC_Container interface
    // =========================================================================
    int objectCount() const;
    AC_Object* objectAtIndex(int index) const;
    AC_Object* findObject(const char* identity) const;
    AC_Item* findItem(const char* identity) const;
    int indexOf(AC_Object* object) const;
    int indexOf(const char* identity) const;
    bool isChecked(const char* identity) const;
    void setChecked(const char* identity, bool checked);

    // =========================================================================
    // AC_Help interface
    // =========================================================================
    QString cshHelpId();

    // =========================================================================
    // AC_Toolbar interface
    // =========================================================================
    QString text() const;
    void setText(const QString& text);
    bool isCustomizable() const;
    AC_Manager* manager() const;
    QObject* owner() const;
    void setOwner(QObject* owner);
    AC_ToolbarItemGenerator* itemGenerator() const;
    void setItemGenerator(AC_ToolbarItemGenerator* generator);

    AC_Object* insert(AC_Object* beforeObject, const QDomElement& element);
    AC_Separator* insertSeparator(AC_Object* beforeObject);
    AC_Object* insertAtPlaceholder(const char* placeholder, const QDomElement& element);
    AC_Object* insertAtPlaceholder(const char* placeholder,
                                    const QString& responderIdentity,
                                    const QString& actionName,
                                    const char* objectName,
                                    const QList<QString>* keywords,
                                    bool visible);
    AC_Item* insertPlaceHolderContents(AC_Object* beforeObject,
                                        const QString& responderIdentity,
                                        const QString& actionName,
                                        const char* objectName,
                                        const QList<QString>* keywords,
                                        bool visible,
                                        bool flag);

    bool removeObject(const char* identity);
    void removeAction(QAction* action);
    void removeChild(QObject* child);
    void clear();
    void clearPlaceholder(const char* placeholder);
    void clearPlaceholders();
    void hideAllObjects();
    void remove();

    QList<QString> config() const;
    void setConfig(const QList<QString>& config);
    QList<QString> defaultConfig() const;
    void setDefaultConfig(const QList<QString>& config);
    bool isDefaultConfig() const;
    bool isDefaultButton(const QString& identity) const;
    bool changeContent(const QDomElement& element,
                        QList<QString>* addedIds,
                        QList<QString>* removedIds);
    bool changeContent(const QString& name,
                        QList<QString>* addedIds,
                        QList<QString>* removedIds);

    void validateContent();
    void validateContentIfVisible();
    void validateTooltipText();
    void validateTooltipTextIfVisible();
    void updateSeparators();
    void updateCustomizedButtons(const QString& identity,
                                  const QString& oldName,
                                  const QString& newName);

    void customize(QWidget* parent);
    void connectUpdate(QObject* receiver, const char* slot);

    QWidget* toQWidget();
    QToolBar* toQToolBar();
    int visibleObjects();
    QString translate(const QString& text);

    // =========================================================================
    // Static helpers
    // =========================================================================

    /**
     * @brief Translation helper
     */

signals:
    /**
     * @brief Emitted when toolbar is customized
     * @param identity Toolbar identity
     */
    void customized(QString identity);

protected:
    /**
     * @brief Handle show events
     */
    virtual void showEvent(QShowEvent* event) override;

    /**
     * @brief Create toolbar content from XML
     * @param element XML element defining content
     */
    void create(const QDomElement& element);

    /**
     * @brief Create toolbar content with default config
     * @param element XML element
     * @param defaultConfig Default configuration
     */
    void create(const QDomElement& element, const QList<QString>& defaultConfig);

    /**
     * @brief Create the customize button
     */
    void createCustomizeButton();

protected slots:
    /**
     * @brief Handle owner destruction
     */
    void ownerDestroyed();
};

/**
 * @brief Interface for generating custom toolbar items
 *
 * Implement this interface to provide custom item creation logic
 * for toolbars managed by AC_Manager.
 */
class AC_ToolbarItemGenerator {
public:
    virtual ~AC_ToolbarItemGenerator() = default;

    /**
     * @brief Create an item for the given identity
     * @param identity Item identity
     * @param parent Parent toolbar
     * @return Created item or nullptr
     */
    virtual AC_Item* createItem(const QString& identity, AC_ToolbarImpl* parent) = 0;
};

// ============================================================================
// Offset Constants (for reference/debugging)
// ============================================================================
namespace AC_ToolbarImpl_Offsets {
    // Offsets from QToolBar base (verified from constructor at 0x180032df0)
    constexpr size_t QObjectVtable        = 0x00;   // 0 - QObject vptr
    constexpr size_t QPaintDeviceVtable   = 0x10;   // 16 - QPaintDevice vptr
    constexpr size_t ContainerImpl        = 0x28;   // 40 - AC_ContainerImpl embedded (internal vptr)
    constexpr size_t ToolbarVtable        = 0x80;   // 128 - AC_Toolbar interface vptr (returned by createToolbar)
    constexpr size_t HelpVtable           = 0x88;   // 136 - AC_Help vptr
    constexpr size_t ResponderIdentity    = 0x98;   // 152 - QString
    constexpr size_t DefaultConfigVector  = 0xB0;   // 176 - std::vector<AC_Object*>
    constexpr size_t IsCustomizable       = 0xC8;   // 200 - bool
    constexpr size_t CustomizeButton      = 0xD0;   // 208 - void*
    constexpr size_t Unknown1             = 0xD8;   // 216 - void*
    constexpr size_t Owner                = 0xE0;   // 224 - QObject*
    constexpr size_t MainWindowWeakPtr    = 0xE8;   // 232 - QWeakPointer<QWidget>
    constexpr size_t ScaleFactor          = 0xF8;   // 248 - float (default 1.0f)
    constexpr size_t TranslationContext   = 0x100;  // 256 - QString
}

namespace AC_ContainerImpl_Offsets {
    constexpr size_t Manager              = 0x08;   // 8 - AC_ManagerImpl*
    constexpr size_t Identity             = 0x10;   // 16 - QString
    constexpr size_t Enabled              = 0x28;   // 40 - bool
    constexpr size_t IsToolbar            = 0x29;   // 41 - bool
    constexpr size_t Objects              = 0x30;   // 48 - std::vector<AC_Object*>
    constexpr size_t Placeholders         = 0x48;   // 72 - Internal tree/map
}

