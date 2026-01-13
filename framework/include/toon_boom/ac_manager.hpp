/**
 * @file ac_manager.hpp
 * @brief Reconstructed header for Toon Boom AC_Manager class hierarchy
 *
 * This header was reverse-engineered from ToonBoomActionManager.dll
 * IDA Database: RE/ToonBoomActionManager.dll.i64
 *
 * AC_Manager is the central action/command management system for Toon Boom
 * applications. It handles:
 * - Responder chain management (first responder, selection responder)
 * - Menu creation and management
 * - Toolbar creation and registration
 * - Shortcut/keyboard handling
 * - Action triggering and validation
 *
 * @note All offsets and structures derived from decompilation analysis.
 *       This is NOT official Toon Boom code.
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtXml/QDomElement>
#include <vector>

// Forward declarations
class AC_Manager;
class AC_ManagerImpl;
class AC_Responder;
class AC_Menu;
class AC_Toolbar;
class AC_ToolbarImpl;
class AC_ToolbarItemGenerator;
class AC_ShortcutManager;
class AC_ActionInfo;
class AC_Object;
class AC_Item;
class AC_Container;
class AC_ContainerImpl;
class AC_ToolbarMultiButton;

/**
 * @brief Result code for action command handling
 *
 * Used as return value from perform(), trigger(), and validation methods.
 */
enum class AC_Result : int {
    NotHandled = 0,  ///< Action was not handled by any responder
    Handled = 1,     ///< Action was successfully handled
    Error = 2        ///< An error occurred during handling
};

/**
 * @brief Manager options for AC_Manager configuration
 */
enum class AC_ManagerOption : int {
    TrimShortcuts = 0  ///< Whether to trim whitespace from shortcut strings
    // Additional options may exist
};

/**
 * @brief Action information passed to responders
 *
 * Contains information about the action being performed, including
 * the slot name, parameters, and enabled/checked state.
 * 
 * This is an opaque class - the actual implementation is in Toon Boom DLLs.
 * The interface is provided for type safety in method signatures.
 */
class AC_ActionInfo : public QObject {
    Q_OBJECT
public:
    virtual ~AC_ActionInfo() = default;
    
    // State accessors - implemented in Toon Boom DLLs
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isChecked() const = 0;
    virtual void setChecked(bool checked) = 0;
    virtual bool isVisible() const = 0;
    virtual void setVisible(bool visible) = 0;
    
    // Action info
    virtual const QString& slot() const = 0;
    virtual const QString& text() const = 0;
    virtual QVariant itemParameter() const = 0;
    
    // Responder
    virtual AC_Responder* responder() const = 0;
    virtual void setResponder(AC_Responder* responder) = 0;
};

/**
 * @brief Abstract interface for objects that can respond to actions
 *
 * AC_Responder defines the responder chain pattern used by Toon Boom.
 * Responders receive actions from toolbar buttons, menus, and shortcuts,
 * and can either handle them or pass them up the chain.
 *
 * To create a custom responder:
 * 1. Inherit from AC_Responder (and QObject if you need Qt signals/slots)
 * 2. Implement all pure virtual methods
 * 3. Register with AC_Manager::registerResponder()
 *
 * Example:
 * @code
 * class MyResponder : public QObject, public AC_Responder {
 *     Q_OBJECT
 * public:
 *     MyResponder(const QString& id, AC_Manager* mgr) 
 *         : m_identity(id), m_manager(mgr) {
 *         mgr->registerResponder(this, nullptr);
 *     }
 *     
 *     // Implement AC_Responder interface...
 *     const QString& responderIdentity() const override { return m_identity; }
 *     AC_Manager* actionManager() const override { return m_manager; }
 *     // ... other methods
 *     
 * public slots:
 *     void onActionMyAction() { // handle action }
 * };
 * @endcode
 */
class AC_Responder {
public:
    virtual ~AC_Responder() = default;

    // =========================================================================
    // Identity
    // =========================================================================
    
    /**
     * @brief Get the unique identity string for this responder
     * @return Reference to identity QString (must remain valid)
     */
    virtual const QString& responderIdentity() const = 0;
    
    /**
     * @brief Get the human-readable description
     * @return Reference to description QString
     */
    virtual const QString& responderDescription() const = 0;
    
    /**
     * @brief Set the description
     * @param description New description
     */
    virtual void setResponderDescription(const QString& description) = 0;

    // =========================================================================
    // Responder Chain Navigation
    // =========================================================================
    
    /**
     * @brief Get the parent responder in the chain
     * @return Parent responder or nullptr if this is the root
     */
    virtual AC_Responder* parentResponder() = 0;
    
    /**
     * @brief Get proxy responder that should handle actions instead
     * @return Proxy responder or nullptr
     */
    virtual AC_Responder* proxyResponder() = 0;

    // =========================================================================
    // First Responder (Keyboard Focus)
    // =========================================================================
    
    /**
     * @brief Check if this responder can become first responder
     * @return true if this responder accepts first responder status
     */
    virtual bool acceptsFirstResponder() = 0;
    
    /**
     * @brief Called when becoming first responder
     * @return true if successfully became first responder
     */
    virtual bool becomeFirstResponder() = 0;
    
    /**
     * @brief Called when resigning first responder status
     * @return true if successfully resigned
     */
    virtual bool resignFirstResponder() = 0;

    // =========================================================================
    // Selection Responder
    // =========================================================================
    
    /**
     * @brief Check if this responder can become selection responder
     * @return true if this responder accepts selection responder status
     */
    virtual bool acceptsSelectionResponder() = 0;
    
    /**
     * @brief Called when becoming selection responder
     * @return true if successfully became selection responder
     */
    virtual bool becomeSelectionResponder() = 0;
    
    /**
     * @brief Called when resigning selection responder status
     * @return true if successfully resigned
     */
    virtual bool resignSelectionResponder() = 0;

    // =========================================================================
    // Action Handling
    // =========================================================================
    
    /**
     * @brief Perform an action
     * 
     * This is called when an action targets this responder. The implementation
     * should invoke the appropriate slot based on AC_ActionInfo::slot().
     *
     * @param info Action information including slot name and parameters
     * @return AC_Result::Handled if action was handled, NotHandled to pass up chain
     */
    virtual AC_Result perform(AC_ActionInfo* info) = 0;
    
    /**
     * @brief Perform action on child responders
     * @param info Action information
     * @return Result from children
     */
    virtual AC_Result performDownToChildren(AC_ActionInfo* info) = 0;
    
    /**
     * @brief Check if this responder should receive messages
     * @return true if responder is active and should receive messages
     */
    virtual bool shouldReceiveMessages() const = 0;
    
    /**
     * @brief Check if this responder handles keyboard shortcuts
     * @return true if shortcuts should be processed by this responder
     */
    virtual bool handleShortcuts() const = 0;

    // =========================================================================
    // Event Handling
    // =========================================================================
    
    /**
     * @brief Handle a Qt event
     * @param event Event to handle
     * @return AC_Result indicating if event was handled
     */
    virtual AC_Result handleEvent(QEvent* event) = 0;

    // =========================================================================
    // Manager Access
    // =========================================================================
    
    /**
     * @brief Get the action manager
     * @return AC_Manager instance or nullptr
     */
    virtual AC_Manager* actionManager() const = 0;
};

/**
 * @brief Simple base class for implementing AC_Responder
 *
 * Provides default implementations for most AC_Responder methods.
 * Subclass this for simpler responder creation.
 *
 * @note This is a convenience class for users creating custom responders.
 *       It is NOT the same as Toon Boom's internal AC_ResponderTemplate.
 */
class AC_ResponderBase : public AC_Responder {
public:
    AC_ResponderBase(const QString& identity, AC_Manager* manager = nullptr)
        : m_identity(identity), m_manager(manager) {}
    
    virtual ~AC_ResponderBase() = default;
    
    // Identity
    const QString& responderIdentity() const override { return m_identity; }
    const QString& responderDescription() const override { return m_description; }
    void setResponderDescription(const QString& desc) override { m_description = desc; }
    
    // Chain - override in subclasses if needed
    AC_Responder* parentResponder() override { return nullptr; }
    AC_Responder* proxyResponder() override { return nullptr; }
    
    // First responder - typically not needed for simple responders
    bool acceptsFirstResponder() override { return false; }
    bool becomeFirstResponder() override { return false; }
    bool resignFirstResponder() override { return true; }
    
    // Selection responder - typically not needed for simple responders
    bool acceptsSelectionResponder() override { return false; }
    bool becomeSelectionResponder() override { return false; }
    bool resignSelectionResponder() override { return true; }
    
    // Action handling - override perform() in subclasses
    AC_Result perform(AC_ActionInfo* /*info*/) override { return AC_Result::NotHandled; }
    AC_Result performDownToChildren(AC_ActionInfo* /*info*/) override { return AC_Result::NotHandled; }
    
    // Messages
    bool shouldReceiveMessages() const override { return true; }
    bool handleShortcuts() const override { return true; }
    
    // Events
    AC_Result handleEvent(QEvent* /*event*/) override { return AC_Result::NotHandled; }
    
    // Manager
    AC_Manager* actionManager() const override { return m_manager; }
    void setActionManager(AC_Manager* manager) { m_manager = manager; }

protected:
    QString m_identity;
    QString m_description;
    AC_Manager* m_manager;
};

/**
 * @brief Initialization parameters for AC_ManagerImpl constructor
 *
 * sizeof(AC_ManagerInitParams) = 0x20 (32 bytes) on x64
 */
struct AC_ManagerInitParams {
    void* keywords;          ///< +0x00: Pointer to keywords QList<QString>
    void* keywordsEnd;       ///< +0x08: End pointer for keywords
    void* keywordsCapacity;  ///< +0x10: Capacity pointer for keywords
    bool trimShortcuts;      ///< +0x18: Initial value for TrimShortcuts option
};

/**
 * @brief Abstract base class for action/command management
 *
 * AC_Manager is a pure virtual interface that defines the contract for
 * the action management system. The concrete implementation is AC_ManagerImpl.
 *
 * Key responsibilities:
 * - Responder chain management for first responder and selection responder
 * - Menu and toolbar creation from XML definitions
 * - Action triggering and validation
 * - Shortcut/keyboard event handling
 * - Image/icon loading for UI elements
 *
 * vtable at 0x18004e508 (76 virtual methods)
 */
class AC_Manager {
public:
    virtual ~AC_Manager() = 0;

    // =========================================================================
    // Image/Icon Management
    // =========================================================================

    /**
     * @brief Add a directory to search for images
     * @param dir Directory path to add
     */
    virtual void addImageDir(const QString& dir) = 0;

    /**
     * @brief Add multiple image directories from a path string
     * @param dirs Semicolon-separated directory paths
     */
    virtual void addImageDirs(const QString& dirs) = 0;

    /**
     * @brief Find an image file by name
     * @param name Image name to find
     * @return Full path to the image, or empty string if not found
     */
    virtual QString findImage(const QString& name) = 0;

    /**
     * @brief Load an icon with optional color blending
     * @param name Icon name to load
     * @param blendColor Color to blend with the icon
     * @param useGeneric If true, fall back to generic image if not found
     * @return Loaded QIcon
     */
    virtual QIcon loadImage(const QString& name, const QColor& blendColor, bool useGeneric) = 0;

    /**
     * @brief Create a QAction-compatible icon
     * @param icon Source icon
     * @return Icon suitable for use with QAction
     */
    virtual QIcon createQActionCompatibleIcon(const QIcon& icon) = 0;

    /**
     * @brief Set the generic/fallback image name
     * @param name Name of the generic image
     */
    virtual void setGenericImage(const QString& name) = 0;

    // =========================================================================
    // Identity and Options
    // =========================================================================

    /**
     * @brief Generate a unique identity string
     * @return Unique identifier in format "__uuid_%1_%2__"
     */
    virtual QString generateIdentity() = 0;

    /**
     * @brief Get hover ID for current UI element
     * @return Current hover identifier
     */
    virtual QString hoverId() const = 0;

    /**
     * @brief Set hover ID
     * @param id New hover identifier
     */
    virtual void setHoverId(const QString& id) = 0;

    /**
     * @brief Get string option value
     * @param name Option name
     * @return Option value as string
     */
    virtual QString option(const QString& name) const = 0;

    /**
     * @brief Get integer option value
     * @param option Option enum value
     * @return Option value as integer
     */
    virtual int option(AC_ManagerOption option) const = 0;

    /**
     * @brief Set string option
     * @param name Option name
     * @param value Option value
     * @return true if option was set successfully
     */
    virtual bool setOption(const QString& name, const QString& value) = 0;

    /**
     * @brief Set integer option
     * @param option Option enum value
     * @param value Option value
     * @return true if option was set successfully
     */
    virtual bool setOption(AC_ManagerOption option, int value) = 0;

    // =========================================================================
    // Responder Management
    // =========================================================================

    /**
     * @brief Get the application-level responder
     * @return Application responder or nullptr
     */
    virtual AC_Responder* applicationResponder() const = 0;

    /**
     * @brief Set the application-level responder
     * @param responder New application responder
     */
    virtual void setApplicationResponder(AC_Responder* responder) = 0;

    /**
     * @brief Get the current first responder
     * @return First responder at top of stack, or nullptr
     */
    virtual AC_Responder* firstResponder() const = 0;

    /**
     * @brief Set the first responder
     * @param responder New first responder
     * @return true if successfully set
     */
    virtual bool setFirstResponder(AC_Responder* responder) = 0;

    /**
     * @brief Get the selection responder
     * @return Current selection responder or nullptr
     */
    virtual AC_Responder* selectionResponder() const = 0;

    /**
     * @brief Set the selection responder
     * @param responder New selection responder
     * @return true if successfully set
     */
    virtual bool setSelectionResponder(AC_Responder* responder) = 0;

    /**
     * @brief Get the mouse responder (responder under mouse cursor)
     * @return Mouse responder or nullptr
     */
    virtual AC_Responder* mouseResponder() const = 0;

    /**
     * @brief Find responder by identity string
     * @param identity Responder identity to find
     * @return Responder or nullptr if not found
     */
    virtual AC_Responder* responder(const QString& identity) const = 0;

    /**
     * @brief Find responder by C-string identity
     * @param identity Responder identity to find
     * @return Responder or nullptr if not found
     */
    virtual AC_Responder* responder(const char* identity) const = 0;

    /**
     * @brief Find responder by identity list with fallback
     * @param identities List of identities to try
     * @param fallback Fallback responder if none found
     * @return Found responder or fallback
     */
    virtual AC_Responder* responder(const QList<QString>& identities, AC_Responder* fallback) const = 0;

    /**
     * @brief Get responder associated with a widget
     * @param widget Widget to find responder for
     * @return Associated responder or nullptr
     */
    virtual AC_Responder* responderForWidget(QWidget* widget) const = 0;

    /**
     * @brief Register a responder
     * @param responder Responder to register
     * @param widget Associated widget (may be nullptr)
     * @return true if successfully registered
     */
    virtual bool registerResponder(AC_Responder* responder, QWidget* widget) = 0;

    /**
     * @brief Unregister a responder
     * @param responder Responder to unregister
     */
    virtual void unregisterResponder(AC_Responder* responder) = 0;

    /**
     * @brief Register a factory function to create responders for widgets
     * @param widget Widget class to register factory for
     * @param factory Function that creates AC_Responder for the widget
     */
    virtual void registerResponderFactoryFnc(QWidget* widget,
        AC_Responder* (*factory)(AC_Manager*, QWidget*)) = 0;

    /**
     * @brief Push a responder up the responder chain
     * @param responder Responder to push
     */
    virtual void pushUp(AC_Responder* responder) = 0;

    /**
     * @brief Push a responder out of the chain
     * @param responder Responder to remove
     */
    virtual void pushOut(AC_Responder* responder) = 0;

    /**
     * @brief Notify that selection was cleared on a responder
     * @param responder Responder whose selection was cleared
     */
    virtual void selectionCleared(AC_Responder* responder) = 0;

    /**
     * @brief Ignore a widget for responder purposes
     * @param widget Widget to ignore
     */
    virtual void ignoreWidget(QWidget* widget) = 0;

    /**
     * @brief Check if a widget is ignored
     * @param widget Widget to check
     * @param checkParents If true, also check parent widgets
     * @return true if widget is ignored
     */
    virtual bool isWidgetIgnored(QWidget* widget, bool checkParents) const = 0;

    /**
     * @brief Get all responder identities
     * @return List of all registered responder identities
     */
    virtual QList<QString> allResponderIdentities() const = 0;

    /**
     * @brief Get slot list for a responder
     * @param identity Responder identity
     * @param includeInherited Include inherited slots
     * @return List of slot names
     */
    virtual QList<QString> responderSlotList(const QString& identity, bool includeInherited) const = 0;

    // =========================================================================
    // Menu Management
    // =========================================================================

    /**
     * @brief Get the main menu bar
     * @return Main menu bar or nullptr
     */
    virtual AC_Menu* menuBar() const = 0;

    /**
     * @brief Set the main menu bar
     * @param menu New main menu bar
     */
    virtual void setMenuBar(AC_Menu* menu) = 0;

    /**
     * @brief Create a menu bar from XML element
     * @param element XML element defining the menu
     * @param menuBar QMenuBar to populate
     * @return Created AC_Menu
     */
    virtual AC_Menu* createMenuBar(const QDomElement& element, QMenuBar* menuBar) = 0;

    /**
     * @brief Create a menu bar from XML element on a widget
     * @param element XML element defining the menu
     * @param parent Parent widget
     * @return Created AC_Menu
     */
    virtual AC_Menu* createMenuBar(const QDomElement& element, QWidget* parent) = 0;

    /**
     * @brief Create a menu bar from menu name
     * @param name Menu name to load
     * @param menuBar QMenuBar to populate
     * @return Created AC_Menu
     */
    virtual AC_Menu* createMenuBar(const QString& name, QMenuBar* menuBar) = 0;

    /**
     * @brief Create a menu bar from menu name on a widget
     * @param name Menu name to load
     * @param parent Parent widget
     * @return Created AC_Menu
     */
    virtual AC_Menu* createMenuBar(const QString& name, QWidget* parent) = 0;

    /**
     * @brief Create a popup menu from XML element
     * @param element XML element defining the menu
     * @param parent Parent widget
     * @param owner Owner QObject for signal connections
     * @return Created AC_Menu
     */
    virtual AC_Menu* createPopupMenu(const QDomElement& element, QWidget* parent, QObject* owner) = 0;

    /**
     * @brief Create a popup menu from name
     * @param name Menu name to load
     * @param parent Parent widget
     * @param owner Owner QObject
     * @return Created AC_Menu
     */
    virtual AC_Menu* createPopupMenu(const QString& name, QWidget* parent, QObject* owner) = 0;

    /**
     * @brief Create a popup menu with icons
     * @param name Menu name to load
     * @param parent Parent widget
     * @param owner Owner QObject
     * @return Created AC_Menu with icons
     */
    virtual AC_Menu* createPopupMenuWithIcons(const QString& name, QWidget* parent, QObject* owner) = 0;

    /**
     * @brief Load menus from XML element
     * @param element Root XML element containing menu definitions
     */
    virtual void loadMenus(const QDomElement& element) = 0;

    /**
     * @brief Load menus from XML element with flags
     * @param element Root XML element
     * @param flags Loading flags
     */
    virtual void loadMenus(const QDomElement& element, int flags) = 0;

    /**
     * @brief Load menus from file
     * @param path Path to menu definition file
     */
    virtual void loadMenus(const QString& path) = 0;

    /**
     * @brief Load plugin menus
     * @param name Plugin name
     * @param placeholders List of placeholder names
     * @param element XML element with menu definitions
     */
    virtual void loadPluginMenus(const QString& name, QList<QString>& placeholders,
                                  const QDomElement& element) = 0;

    /**
     * @brief Get menu element by name
     * @param name Menu name
     * @return XML element for the menu
     */
    virtual QDomElement menuElement(const QString& name) = 0;

    // =========================================================================
    // Toolbar Management
    // =========================================================================

    /**
     * @brief Create a toolbar from XML element
     * @param element XML element defining the toolbar
     * @param ids Output list of item IDs (may be nullptr)
     * @param mainWindow Main window to add toolbar to
     * @param area Toolbar area (Qt::ToolBarArea)
     * @param objectName Object name for the toolbar (may be nullptr)
     * @param owner Owner QObject
     * @return Created AC_Toolbar
     */
    virtual AC_Toolbar* createToolbar(const QDomElement& element, QList<QString>* ids,
                                       QMainWindow* mainWindow, int area,
                                       const char* objectName, QObject* owner) = 0;

    /**
     * @brief Create a toolbar from name
     * @param name Toolbar name to load
     * @param ids Output list of item IDs
     * @param mainWindow Main window
     * @param area Toolbar area
     * @param objectName Object name
     * @param owner Owner QObject
     * @return Created AC_Toolbar
     */
    virtual AC_Toolbar* createToolbar(const QString& name, QList<QString>* ids,
                                       QMainWindow* mainWindow, int area,
                                       const char* objectName, QObject* owner) = 0;

    /**
     * @brief Create a multi-button toolbar item
     * @param element XML element defining the multi-button
     * @param container Container for the button
     * @param parent Parent widget
     * @return Created AC_ToolbarMultiButton
     */
    virtual AC_ToolbarMultiButton* createToolbarMultiButton(const QDomElement& element,
                                                             AC_ContainerImpl* container,
                                                             QWidget* parent) = 0;

    /**
     * @brief Load toolbars from XML element
     * @param element Root XML element
     * @param ids Output list of loaded toolbar IDs
     */
    virtual void loadToolbars(const QDomElement& element, QList<QString>& ids) = 0;

    /**
     * @brief Load toolbars from file
     * @param path Path to toolbar definition file
     * @param ids Output list of loaded toolbar IDs
     */
    virtual void loadToolbars(const QString& path, QList<QString>& ids) = 0;

    /**
     * @brief Get toolbar element by name
     * @param name Toolbar name
     * @return XML element for the toolbar
     */
    virtual QDomElement toolbarElement(const QString& name) = 0;

    /**
     * @brief Get toolbar item generator
     * @return Current item generator or nullptr
     */
    virtual AC_ToolbarItemGenerator* itemGenerator() const = 0;

    /**
     * @brief Set toolbar item generator
     * @param generator New item generator
     */
    virtual void setItemGenerator(AC_ToolbarItemGenerator* generator) = 0;

    /**
     * @brief Register a toolbar with the manager
     * @param toolbar Toolbar to register
     */
    virtual void registerToolbar(AC_ToolbarImpl* toolbar) = 0;

    /**
     * @brief Get toolbar customize button image name
     * @return Image name for customize button
     */
    virtual QString toolbarCustomizeImage() const = 0;

    /**
     * @brief Set toolbar customize button image
     * @param name Image name
     */
    virtual void setToolbarCustomizeImage(const QString& name) = 0;

    /**
     * @brief Update all toolbar states
     */
    virtual void updateToolbars() = 0;

    /**
     * @brief Update toolbar tooltip text
     */
    virtual void updateToolbarText() = 0;

    // =========================================================================
    // Shortcut/Keyboard Management
    // =========================================================================

    /**
     * @brief Get the shortcut manager
     * @return Shortcut manager or nullptr
     */
    virtual AC_ShortcutManager* shortcutManager() const = 0;

    /**
     * @brief Set the shortcut manager
     * @param manager New shortcut manager
     */
    virtual void setShortcutManager(AC_ShortcutManager* manager) = 0;

    /**
     * @brief Load shortcuts from XML element
     * @param element XML element with shortcut definitions
     */
    virtual void loadShortcuts(const QDomElement& element) = 0;

    /**
     * @brief Load shortcuts from file
     * @param path Path to shortcut definition file
     */
    virtual void loadShortcuts(const QString& path) = 0;

    /**
     * @brief Get key code for a shortcut
     * @param shortcut Shortcut name
     * @return Qt key code
     */
    virtual int keyCodeForShortcut(const QString& shortcut) const = 0;

    /**
     * @brief Get key sequence for a shortcut
     * @param shortcut Shortcut name
     * @return QKeySequence for the shortcut
     */
    virtual QKeySequence keySequenceForShortcut(const QString& shortcut) const = 0;

    /**
     * @brief Check if a key event matches a shortcut
     * @param shortcut Shortcut name
     * @param event Key event to check
     * @return true if event matches shortcut
     */
    virtual bool isShortcut(const char* shortcut, QKeyEvent* event) const = 0;

    /**
     * @brief Handle a key event
     * @param event Key event to handle
     * @param isRelease true if this is a key release event
     * @return Result indicating if event was handled
     */
    virtual AC_Result handleKeyEvent(QKeyEvent* event, bool isRelease) = 0;

    // =========================================================================
    // Action Triggering and Validation
    // =========================================================================

    /**
     * @brief Trigger an action by name
     * @param responderIdentity Target responder identity
     * @param actionName Action name to trigger
     * @param forEachResponder If true, trigger on each matching responder
     * @return Result of the action
     */
    virtual AC_Result trigger(const QString& responderIdentity, const QString& actionName,
                               bool forEachResponder) = 0;

    /**
     * @brief Trigger an action with arguments
     * @param responderIdentity Target responder identity
     * @param actionName Action name to trigger
     * @param args Action arguments
     * @param forEachResponder If true, trigger on each matching responder
     * @return Result of the action
     */
    virtual AC_Result trigger(const QString& responderIdentity, const QString& actionName,
                               const std::vector<QVariant>& args, bool forEachResponder) = 0;

    /**
     * @brief Trigger an action on each matching responder
     * @param responderIdentity Target responder identity
     * @param actionName Action name to trigger
     * @param forEachResponder Iteration flag
     * @return Result of the action
     */
    virtual AC_Result triggerForEach(const QString& responderIdentity, const QString& actionName,
                                      bool forEachResponder) = 0;

    /**
     * @brief Trigger an action on each responder with arguments
     * @param responderIdentity Target responder identity
     * @param actionName Action name to trigger
     * @param args Action arguments
     * @param forEachResponder Iteration flag
     * @return Result of the action
     */
    virtual AC_Result triggerForEach(const QString& responderIdentity, const QString& actionName,
                                      const std::vector<QVariant>& args, bool forEachResponder) = 0;

    /**
     * @brief Perform validation for an action
     * @param responderIdentity Target responder identity
     * @param actionName Action name to validate
     * @param enabled Output: whether action is enabled
     * @param checked Output: whether action is checked
     * @return Result of validation
     */
    virtual AC_Result performValidation(const QString& responderIdentity, const QString& actionName,
                                         bool* enabled, bool* checked) = 0;

    // =========================================================================
    // Static Factory
    // =========================================================================

    /**
     * @brief Create a responder for a widget
     * @param manager AC_Manager instance
     * @param widget Widget to create responder for
     * @return Created responder or nullptr
     */
    static AC_Responder* createResponderForWidget(AC_Manager* manager, QWidget* widget);
};

/**
 * @brief Concrete implementation of AC_Manager
 *
 * AC_ManagerImpl inherits from QObject and AC_Manager, providing the
 * actual implementation of the action management system.
 *
 * sizeof(AC_ManagerImpl) = 0x158 (344 bytes) on x64
 *
 * Memory layout:
 * - +0x00: QObject (vptr + members)
 * - +0x10: AC_Manager vptr
 * - +0x18: QList<QString>* m_keywords (shared with init params)
 * - +0x20: (keywords internal)
 * - +0x28: (keywords internal)
 * - +0x30: bool m_trimShortcuts
 * - +0x38: Internal tree for responder name->list mapping
 * - +0x48: Internal tree (continued)
 * - +0x60: Internal tree for menus
 * - +0x70: (menus continued)
 * - +0x78: AC_ShortcutManager* m_shortcutManager
 * - +0x80: (shortcut internal)
 * - +0x88: QString m_hoverId
 * - +0xA0: QString m_genericImage
 * - +0xB8: QString m_toolbarCustomizeImage
 * - +0xC8: Internal tree for toolbars
 * - +0xD8: (toolbars continued)
 * - +0xE0: std::vector<AC_Responder*> m_registeredResponders
 * - +0xF8: std::vector<AC_Responder*> m_responderStack
 * - +0x110: (responder stack continued)
 * - +0x118: AC_Menu* m_menuBar
 * - +0x120: AC_Responder* m_firstResponder (via stack)
 * - +0x128: AC_Responder* m_applicationResponder
 * - +0x130: AC_Responder* m_selectionResponder
 * - +0x138: AC_ToolbarItemGenerator* m_itemGenerator
 * - +0x140: int m_uniqueIdCounter
 * - +0x144: int m_flags
 * - +0x148: void* m_internalData
 * - +0x150: bool m_option_trimShortcuts (at offset 304 from AC_Manager base)
 *
 * Signals:
 * - firstResponderChanged()
 * - selectionResponderChanged()
 * - updateToolbarsSignal()
 * - updateToolbarsText()
 */
class AC_ManagerImpl : public QObject, public AC_Manager {
    Q_OBJECT

public:
    /**
     * @brief Construct AC_ManagerImpl with initialization parameters
     * @param params Initialization parameters
     */
    explicit AC_ManagerImpl(const AC_ManagerInitParams& params);
    virtual ~AC_ManagerImpl() override;

    // All AC_Manager virtual methods are implemented...
    // See AC_Manager for documentation

    // Additional non-virtual methods

    /**
     * @brief Get the keywords list
     * @return Reference to keywords list
     */
    const QList<QString>& keywords() const;

    /**
     * @brief Check if shortcuts should be trimmed
     * @return true if trimming is enabled
     */
    bool trimShortcuts() const;

    /**
     * @brief Check if a responder is registered
     * @param responder Responder to check
     * @return true if registered
     */
    bool isRegistred(AC_Responder* responder) const;

    /**
     * @brief Get all responders matching identity list
     * @param identities Identity list to match
     * @param fallback Fallback responder
     * @return Vector of matching responders
     */
    std::vector<AC_Responder*> responders(const QList<QString>& identities,
                                           AC_Responder* fallback) const;

    /**
     * @brief Get all responders matching single identity
     * @param identity Identity to match
     * @return Vector of matching responders
     */
    std::vector<AC_Responder*> responders(const QString& identity) const;

    /**
     * @brief Normalize a string for comparison
     * @param str String to normalize
     * @return Normalized string
     */
    static QString normalize(const QString& str);

    

signals:
    /**
     * @brief Emitted when the first responder changes
     */
    void firstResponderChanged();

    /**
     * @brief Emitted when the selection responder changes
     */
    void selectionResponderChanged();

    /**
     * @brief Emitted to signal toolbar update needed
     */
    void updateToolbarsSignal();

    /**
     * @brief Emitted to signal toolbar text update needed
     */
    void updateToolbarsText();

protected:
    /**
     * @brief Fire the first responder changed signal
     */
    void fireFirstResponderChanged();

    /**
     * @brief Fire the selection responder changed signal
     */
    void fireSelectionResponderChanged();

    /**
     * @brief Fire toolbar update signal
     */
    void fireUpdateToolbars();

    /**
     * @brief Fire toolbar text update signal
     */
    void fireUpdateToolbarText();

    /**
     * @brief Fire toolbar button clicked notification
     * @param identity Button identity
     */
    void fireToolbarButtonClicked(const QString& identity);

protected slots:
    /**
     * @brief Handle ignored widget destruction
     * @param object Destroyed object
     */
    void ignoredWidgetDestroyed(QObject* object);
};

// ============================================================================
// Offset Constants (for reference/debugging)
// ============================================================================
namespace AC_ManagerImpl_Offsets {
    // Offsets from AC_Manager base (this + 16 from QObject base)
    constexpr size_t Keywords              = 0x08;   // 8 - QList<QString>* to keywords
    constexpr size_t TrimShortcutsInit     = 0x18;   // 24 - Initial trimShortcuts value
    constexpr size_t ResponderNameMap      = 0x28;   // 40 - Internal tree for name->responders
    constexpr size_t MenuTree              = 0x50;   // 80 - Internal tree for menus
    constexpr size_t ShortcutManager       = 0x68;   // 104 - AC_ShortcutManager*
    constexpr size_t HoverId               = 0x78;   // 120 - QString m_hoverId
    constexpr size_t GenericImage          = 0x90;   // 144 - QString m_genericImage
    constexpr size_t ToolbarCustomizeImage = 0xA8;   // 168 - QString m_toolbarCustomizeImage
    constexpr size_t ToolbarTree           = 0xB8;   // 184 - Internal tree for toolbars
    constexpr size_t RegisteredResponders  = 0xD0;   // 208 - std::vector<AC_Responder*>
    constexpr size_t ResponderStack        = 0xE8;   // 232 - std::vector<AC_Responder*>
    constexpr size_t MenuBar               = 0x108;  // 264 - AC_Menu*
    constexpr size_t ApplicationResponder  = 0x118;  // 280 - AC_Responder*
    constexpr size_t SelectionResponder    = 0x120;  // 288 - AC_Responder*
    constexpr size_t ItemGenerator         = 0x128;  // 296 - AC_ToolbarItemGenerator*
    constexpr size_t TrimShortcutsOption   = 0x130;  // 304 - bool (option value)
}

