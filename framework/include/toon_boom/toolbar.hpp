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
 * vtable at 0x180054eb0 (32 entries for main vtable)
 */
class AC_Toolbar : public AC_Object, public AC_Container, public AC_Help {
public:
    virtual ~AC_Toolbar() = 0;

    // =========================================================================
    // Identity and Basic Properties
    // =========================================================================

    /**
     * @brief Get the toolbar identity
     * @return Reference to identity QString (at offset -72 from AC_Toolbar*)
     */
    virtual const QString& identity() const override = 0;

    /**
     * @brief Get the toolbar text/title
     * @return Toolbar title
     */
    virtual QString text() const = 0;

    /**
     * @brief Set the toolbar text/title
     * @param text New title
     */
    virtual void setText(const QString& text) = 0;

    /**
     * @brief Check if toolbar is visible
     * @return true if visible
     */
    virtual bool isVisible() const override = 0;

    /**
     * @brief Set toolbar visibility
     * @param visible New visibility state
     */
    virtual void setVisible(bool visible) override = 0;

    /**
     * @brief Check if toolbar is enabled
     * @return true if enabled
     */
    virtual bool isEnabled() const override = 0;

    /**
     * @brief Set toolbar enabled state
     * @param enabled New enabled state
     */
    virtual void setEnabled(bool enabled) override = 0;

    /**
     * @brief Check if toolbar is customizable
     * @return true if customization is allowed
     */
    virtual bool isCustomizable() const = 0;

    // =========================================================================
    // Manager and Owner
    // =========================================================================

    /**
     * @brief Get the associated AC_Manager
     * @return Manager pointer or nullptr
     */
    virtual AC_Manager* manager() const = 0;

    /**
     * @brief Get the owner object
     * @return Owner QObject or nullptr
     */
    virtual QObject* owner() const = 0;

    /**
     * @brief Set the owner object
     * @param owner New owner
     */
    virtual void setOwner(QObject* owner) = 0;

    // =========================================================================
    // Item Generator
    // =========================================================================

    /**
     * @brief Get the toolbar item generator
     * @return Item generator or nullptr
     */
    virtual AC_ToolbarItemGenerator* itemGenerator() const = 0;

    /**
     * @brief Set the toolbar item generator
     * @param generator New generator
     */
    virtual void setItemGenerator(AC_ToolbarItemGenerator* generator) = 0;

    // =========================================================================
    // Content Management
    // =========================================================================

    /**
     * @brief Insert an object from XML element
     * @param beforeObject Insert before this object (nullptr = end)
     * @param element XML element defining the item
     * @return Inserted AC_Object or nullptr
     */
    virtual AC_Object* insert(AC_Object* beforeObject, const QDomElement& element) = 0;

    /**
     * @brief Insert a separator before an object
     * @param beforeObject Insert before this object
     * @return Created separator or nullptr
     */
    virtual AC_Separator* insertSeparator(AC_Object* beforeObject) = 0;

    /**
     * @brief Insert at a named placeholder
     * @param placeholder Placeholder name
     * @param element XML element defining the item
     * @return Inserted AC_Object or nullptr
     */
    virtual AC_Object* insertAtPlaceholder(const char* placeholder,
                                            const QDomElement& element) = 0;

    /**
     * @brief Insert at placeholder with full parameters
     * @param placeholder Placeholder name
     * @param responderIdentity Responder identity
     * @param actionName Action name
     * @param objectName Object name (may be nullptr)
     * @param keywords Keywords list (may be nullptr)
     * @param visible Initial visibility
     * @return Inserted AC_Object or nullptr
     */
    virtual AC_Object* insertAtPlaceholder(const char* placeholder,
                                            const QString& responderIdentity,
                                            const QString& actionName,
                                            const char* objectName,
                                            const QList<QString>* keywords,
                                            bool visible) = 0;

    /**
     * @brief Insert placeholder contents
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
     * @brief Remove an object by identity
     * @param identity Object identity to remove
     * @return true if removed successfully
     */
    virtual bool removeObject(const char* identity) = 0;

    /**
     * @brief Remove a QAction from the toolbar
     * @param action Action to remove
     */
    virtual void removeAction(QAction* action) = 0;

    /**
     * @brief Remove a child QObject
     * @param child Child to remove
     */
    virtual void removeChild(QObject* child) = 0;

    /**
     * @brief Clear all objects from toolbar
     */
    virtual void clear() = 0;

    /**
     * @brief Clear a specific placeholder
     * @param placeholder Placeholder name to clear
     */
    virtual void clearPlaceholder(const char* placeholder) = 0;

    /**
     * @brief Clear all placeholders
     */
    virtual void clearPlaceholders() = 0;

    /**
     * @brief Hide all objects
     */
    virtual void hideAllObjects() = 0;

    /**
     * @brief Remove the toolbar from its parent
     */
    virtual void remove() = 0;

    // =========================================================================
    // Configuration
    // =========================================================================

    /**
     * @brief Get current configuration
     * @return List of item identities in current order
     */
    virtual QList<QString> config() const = 0;

    /**
     * @brief Set configuration from list
     * @param config List of item identities
     */
    virtual void setConfig(const QList<QString>& config) = 0;

    /**
     * @brief Get default configuration
     * @return List of item identities in default order
     */
    virtual QList<QString> defaultConfig() const = 0;

    /**
     * @brief Set default configuration
     * @param config List of item identities for default
     */
    virtual void setDefaultConfig(const QList<QString>& config) = 0;

    /**
     * @brief Check if current config matches default
     * @return true if config is default
     */
    virtual bool isDefaultConfig() const = 0;

    /**
     * @brief Check if a button is in default config
     * @param identity Button identity
     * @return true if button is default
     */
    virtual bool isDefaultButton(const QString& identity) const = 0;

    /**
     * @brief Change toolbar content from XML
     * @param element New XML definition
     * @param addedIds Output list of added IDs
     * @param removedIds Output list of removed IDs
     * @return true if changed successfully
     */
    virtual bool changeContent(const QDomElement& element,
                                QList<QString>* addedIds,
                                QList<QString>* removedIds) = 0;

    /**
     * @brief Change toolbar content from name
     * @param name Toolbar definition name
     * @param addedIds Output list of added IDs
     * @param removedIds Output list of removed IDs
     * @return true if changed successfully
     */
    virtual bool changeContent(const QString& name,
                                QList<QString>* addedIds,
                                QList<QString>* removedIds) = 0;

    // =========================================================================
    // Validation and Updates
    // =========================================================================

    /**
     * @brief Validate all content (enable/disable states)
     */
    virtual void validateContent() = 0;

    /**
     * @brief Validate content if toolbar is visible
     */
    virtual void validateContentIfVisible() = 0;

    /**
     * @brief Validate tooltip text for all items
     */
    virtual void validateTooltipText() = 0;

    /**
     * @brief Validate tooltip text if toolbar is visible
     */
    virtual void validateTooltipTextIfVisible() = 0;

    /**
     * @brief Update separator visibility based on adjacent items
     */
    virtual void updateSeparators() = 0;

    /**
     * @brief Update customized button states
     * @param identity Button identity
     * @param oldName Old name
     * @param newName New name
     */
    virtual void updateCustomizedButtons(const QString& identity,
                                          const QString& oldName,
                                          const QString& newName) = 0;

    // =========================================================================
    // Customization
    // =========================================================================

    /**
     * @brief Open customization dialog
     * @param parent Parent widget for dialog
     */
    virtual void customize(QWidget* parent) = 0;

    /**
     * @brief Connect update signals
     * @param receiver Receiver object
     * @param slot Slot to connect
     */
    virtual void connectUpdate(QObject* receiver, const char* slot) = 0;

    // =========================================================================
    // Widget Conversion
    // =========================================================================

    /**
     * @brief Get as QWidget
     * @return QWidget pointer (this - 128 for AC_ToolbarImpl)
     */
    virtual QWidget* toQWidget() = 0;

    /**
     * @brief Get as QToolBar
     * @return QToolBar pointer
     */
    virtual QToolBar* toQToolBar() = 0;

    // =========================================================================
    // Visible Objects
    // =========================================================================

    /**
     * @brief Get count of visible objects
     * @return Number of visible objects
     */
    virtual int visibleObjects() = 0;

    // =========================================================================
    // Translation
    // =========================================================================

    /**
     * @brief Translate a string using toolbar's context
     * @param text Text to translate
     * @return Translated string
     */
    virtual QString translate(const QString& text) = 0;
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
 * AC_ToolbarImpl inherits from QToolBar and implements AC_Toolbar, AC_Container,
 * and AC_Help interfaces. It provides full toolbar functionality including:
 * - Dynamic item management
 * - Customization support
 * - Configuration persistence
 * - Context-sensitive help
 *
 * sizeof(AC_ToolbarImpl) = 0x118 (280 bytes) on x64
 *
 * Memory layout:
 * - +0x00: QToolBar (vptr for QObject at +0x00, QPaintDevice at +0x10)
 * - +0x28: AC_ContainerImpl m_container (88 bytes, ends at +0x80)
 * - +0x80: AC_Toolbar vptr (main toolbar interface)
 * - +0x88: AC_Container vptr
 * - +0x90: AC_Help vptr
 * - +0x98: QString m_responderIdentity (24 bytes)
 * - +0xB0: std::vector<AC_Object*> m_defaultConfig (24 bytes)
 * - +0xC8: bool m_isCustomizable
 * - +0xD0: void* m_customizeButton
 * - +0xD8: void* m_unknown1
 * - +0xE0: QObject* m_owner
 * - +0xE8: QWeakPointer<QWidget> m_mainWindow (16 bytes)
 * - +0xF8: float m_scaleFactor (default 1.0)
 * - +0x100: QString m_translationContext (24 bytes)
 *
 * Vtable addresses:
 * - QObject vtable:     0x180054c90
 * - QPaintDevice vtable: 0x180054e70
 * - AC_Toolbar vtable:  0x180054eb0
 * - AC_Container vtable: 0x180054ef8
 * - AC_Help vtable:     0x1800550f8
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
    // AC_Object interface
    // =========================================================================
    const QString& identity() const;
    bool isVisible() const;
    void setVisible(bool visible);
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

private slots:
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
    // Offsets from QToolBar base
    constexpr size_t ContainerImpl        = 0x28;   // 40 - AC_ContainerImpl embedded
    constexpr size_t ToolbarVtable        = 0x80;   // 128 - AC_Toolbar vptr
    constexpr size_t ContainerVtable      = 0x88;   // 136 - AC_Container vptr
    constexpr size_t HelpVtable           = 0x90;   // 144 - AC_Help vptr
    constexpr size_t ResponderIdentity    = 0x98;   // 152 - QString
    constexpr size_t DefaultConfigVector  = 0xB0;   // 176 - std::vector<AC_Object*>
    constexpr size_t IsCustomizable       = 0xC8;   // 200 - bool
    constexpr size_t CustomizeButton      = 0xD0;   // 208 - void*
    constexpr size_t Owner                = 0xE0;   // 224 - QObject*
    constexpr size_t MainWindowWeakPtr    = 0xE8;   // 232 - QWeakPointer<QWidget>
    constexpr size_t ScaleFactor          = 0xF8;   // 248 - float (default 1.0)
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

