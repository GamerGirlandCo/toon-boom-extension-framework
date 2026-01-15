/**
 * @file PLUG_Services.hpp
 * @brief Reconstructed header for Toon Boom PLUG_Services class hierarchy
 * 
 * This header was reverse-engineered from ToonBoomPlugInManager.dll
 * IDA Database: RE/ToonBoomPlugInManager.dll.i64
 * 
 * @note All offsets and structures derived from decompilation analysis.
 *       This is NOT official Toon Boom code.
 */

#pragma once

#include "./toon_boom_layout.hpp"
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtWidgets/QWidget>

// Forward declarations of service interfaces
struct AT_ExprScriptEngine;
struct CM_Services;
struct PLUG_DragDropInterface;
struct PLUG_ImportEngine;
struct PLUG_InteractiveViewManager;
struct PLUG_KeyStateInterface;
struct PLUG_MenuService;
struct PLUG_ModuleLibraryInterface;
struct PLUG_OGLRenderPlaybackInterface;
struct PLUG_PlaybackRange;
struct PLUG_PreferenceUI;
struct PLUG_ScriptingInterface;
struct PLUG_SetupModeQueryInterface;
struct PLUG_ToolbarService;
struct PLUG_VectorizationInterface;
struct PLUG_WidgetFactoryRegistry;
struct SC_CurrentFrameInterface;
struct SC_CVDataToolInterface;
struct SC_HttpAPI;
struct SC_InteractiveRenderManagerInterface;
struct SC_NetworkViewInterface;
struct SC_SceneEditionStackInterface;
struct SC_SessionContext;
struct SL_Selection;

/**
 * @class PLUG_Services
 * @brief Static class providing access to all Toon Boom plugin service interfaces
 * 
 * This class provides static accessor methods to retrieve various service
 * interfaces from the global PLUG_ManagerImpl singleton. All methods are static.
 * 
 * The singleton is initialized during application startup and must be valid
 * before calling any getters (except getModuleLibraryInterface which uses
 * a separate global).
 */
class PLUG_Services {
public:
    // Delete constructors - this is a static-only class
    PLUG_Services() = delete;
    PLUG_Services(const PLUG_Services&) = delete;
    PLUG_Services& operator=(const PLUG_Services&) = delete;

    /**
     * @brief Get the action/command manager
     * @return AC_Manager* or nullptr if singleton not initialized
     * @note Offset: +0x1A0 (416) in PLUG_ManagerImpl
     */
    static AC_Manager* getActionManager();

    /**
     * @brief Get color manager services
     * @return CM_Services* or nullptr if singleton not initialized
     * @note Offset: +0x218 (536) in PLUG_ManagerImpl
     */
    static CM_Services* getColorManagerServices();

    /**
     * @brief Get current frame interface
     * @return SC_CurrentFrameInterface* or nullptr if singleton not initialized
     * @note Offset: +0x1B0 (432) in PLUG_ManagerImpl
     */
    static SC_CurrentFrameInterface* getCurrentFrameInterface();

    /**
     * @brief Get data tool interface
     * @return SC_CVDataToolInterface* or nullptr if singleton not initialized
     * @note Offset: +0x220 (544) in PLUG_ManagerImpl
     */
    static SC_CVDataToolInterface* getDataToolInterface();

    /**
     * @brief Get drag and drop interface
     * @return PLUG_DragDropInterface* or nullptr if singleton not initialized
     * @note Offset: +0x1F8 (504) in PLUG_ManagerImpl
     */
    static PLUG_DragDropInterface* getDragDropInterface();

    /**
     * @brief Get scene edition stack interface
     * @return SC_SceneEditionStackInterface* or nullptr if singleton not initialized
     * @note Offset: +0x1B8 (440) in PLUG_ManagerImpl
     */
    static SC_SceneEditionStackInterface* getEditionStackInterface();

    /**
     * @brief Get expression scripting engine
     * @return AT_ExprScriptEngine* or nullptr if singleton not initialized or no session context
     * @note Requires SC_SessionContext (+0x1C0) to be non-null
     * @note Offset: +0x200 (512) in PLUG_ManagerImpl
     */
    static AT_ExprScriptEngine* getExpressionScriptingInterface();

    /**
     * @brief Get HTTP API interface
     * @return SC_HttpAPI* or nullptr if singleton not initialized
     * @note Offset: +0x228 (552) in PLUG_ManagerImpl
     */
    static SC_HttpAPI* getHttpAPI();

    /**
     * @brief Get import engine
     * @return PLUG_ImportEngine* or nullptr if singleton not initialized
     * @note Offset: +0x1E0 (480) in PLUG_ManagerImpl
     */
    static PLUG_ImportEngine* getImportEngine();

    /**
     * @brief Get interactive render manager
     * @return SC_InteractiveRenderManagerInterface* or nullptr if singleton not initialized
     * @note Offset: +0x198 (408) in PLUG_ManagerImpl
     */
    static SC_InteractiveRenderManagerInterface* getInteractiveRenderManager();

    /**
     * @brief Get interactive view manager (lazily created)
     * @return PLUG_InteractiveViewManager* - creates new instance if null
     * @note Lazily creates PLUG_InteractiveViewManagerImpl if not set
     * @note Offset: +0x210 (528) in PLUG_ManagerImpl
     */
    static PLUG_InteractiveViewManager* getInteractiveViewManager();

    /**
     * @brief Get keyboard state interface
     * @return PLUG_KeyStateInterface* or nullptr if singleton not initialized
     * @note Offset: +0x1E8 (488) in PLUG_ManagerImpl
     */
    static PLUG_KeyStateInterface* getKeyStateInterface();

    /**
     * @brief Get layout manager
     * @return TULayoutManager* or nullptr if singleton not initialized
     * @note Offset: +0x1F0 (496) in PLUG_ManagerImpl
     */
    static TULayoutManager* getLayoutManager();

    /**
     * @brief Get menu service
     * @return PLUG_MenuService* or nullptr if singleton not initialized
     * @note Offset: +0x1D8 (472) in PLUG_ManagerImpl
     */
    static PLUG_MenuService* getMenuService();

    /**
     * @brief Get module library interface
     * @return PLUG_ModuleLibraryInterface* from separate global (not singleton)
     * @note Uses g_PLUG_ModuleLibraryInterface global, not main singleton
     */
    static PLUG_ModuleLibraryInterface* getModuleLibraryInterface();

    /**
     * @brief Get network view interface
     * @return SC_NetworkViewInterface* or nullptr if singleton not initialized
     * @note Offset: +0x240 (576) in PLUG_ManagerImpl
     */
    static SC_NetworkViewInterface* getNetworkViewInterface();

    /**
     * @brief Get OpenGL render playback interface
     * @return PLUG_OGLRenderPlaybackInterface* or nullptr if singleton not initialized
     * @note Offset: +0x230 (560) in PLUG_ManagerImpl
     */
    static PLUG_OGLRenderPlaybackInterface* getOGLRenderPlaybackInterface();

    /**
     * @brief Get plugin path
     * @param relativePath Relative path within plugin directory
     * @return QString with full path
     * @note Uses virtual call through embedded interface at +0x10
     */
    static QString getPluginPath(const QString& relativePath);

    /**
     * @brief Get preference UI singleton
     * @return PLUG_PreferenceUI* - uses separate singleton pattern with TLS
     * @note This is a thunk to PLUG_PreferenceUIImpl::instance()
     */
    static PLUG_PreferenceUI* getPreference();

    /**
     * @brief Get scripting interface
     * @return PLUG_ScriptingInterface* or nullptr if singleton not initialized
     * @note Offset: +0x1C8 (456) in PLUG_ManagerImpl
     */
    static PLUG_ScriptingInterface* getScriptingInterface();

    /**
     * @brief Get selection manager
     * @return SL_Selection* or nullptr if singleton not initialized
     * @note Offset: +0x1A8 (424) in PLUG_ManagerImpl
     */
    static SL_Selection* getSelection();

    /**
     * @brief Get session context
     * @return SC_SessionContext* or nullptr if singleton not initialized
     * @note Offset: +0x1C0 (448) in PLUG_ManagerImpl
     */
    static SC_SessionContext* getSessionContext();

    /**
     * @brief Get toolbar service
     * @return PLUG_ToolbarService* or nullptr if singleton not initialized
     * @note Offset: +0x1D0 (464) in PLUG_ManagerImpl
     */
    static PLUG_ToolbarService* getToolbarService();

    /**
     * @brief Get vectorization interface
     * @return PLUG_VectorizationInterface* or nullptr if singleton not initialized
     * @note Offset: +0x238 (568) in PLUG_ManagerImpl
     */
    static PLUG_VectorizationInterface* getVectorizationInterface();

    /**
     * @brief Get widget factory registry
     * @return PLUG_WidgetFactoryRegistry* or nullptr if singleton not initialized
     * @note Offset: +0x208 (520) in PLUG_ManagerImpl
     */
    static PLUG_WidgetFactoryRegistry* getWidgetFactoryRegistry();
};

/**
 * @class PLUG_ServicesPrivate
 * @brief Extended static class with additional private functionality
 * 
 * Provides setters and additional query methods not exposed through PLUG_Services.
 */
class PLUG_ServicesPrivate {
public:
    PLUG_ServicesPrivate() = delete;
    PLUG_ServicesPrivate(const PLUG_ServicesPrivate&) = delete;
    PLUG_ServicesPrivate& operator=(const PLUG_ServicesPrivate&) = delete;

    /**
     * @brief Get plugin binary file path
     * @param relativePath Relative path within plugin binary directory
     * @return QString with full path
     */
    static QString getPluginBinFilePath(const QString& relativePath);

    /**
     * @brief Get playback range (const)
     * @return const PLUG_PlaybackRange* static instance
     */
    static const PLUG_PlaybackRange* getPlaybackRange();

    /**
     * @brief Check if application is in setup mode
     * @return true if setup mode is active
     * @note Queries g_PLUG_SetupModeQueryInterface via virtual call
     */
    static bool isSetupMode();

    /**
     * @brief Set the module library interface global
     * @param iface Pointer to module library interface
     * @return true always
     */
    static bool setModuleLibraryInterface(PLUG_ModuleLibraryInterface* iface);

    /**
     * @brief Set the setup mode query interface global
     * @param iface Pointer to setup mode query interface
     */
    static void setSetupModeQueryInterface(PLUG_SetupModeQueryInterface* iface);
};

/**
 * @class PLUG_ScriptingInterface
 * @brief Interface for script execution functionality
 * 
 * Abstract base class providing scripting capabilities for plugins.
 */
class PLUG_ScriptingInterface {
public:
    PLUG_ScriptingInterface();
    virtual ~PLUG_ScriptingInterface();

    /**
     * @struct Program
     * @brief Represents a script program
     */
    struct Program {
        QString path;           ///< Script path/name (offset +0x00)
        QString description;    ///< Script description (offset +0x18)
        QString content;        ///< Script content (offset +0x30)
        QDateTime timestamp;    ///< Modification timestamp (offset +0x48)

        Program();
        Program(const QString& path, const QString& description, const QString& content);
        Program(const Program& other);
        Program(Program&& other);
        ~Program();

        Program& operator=(const Program& other);
        Program& operator=(Program&& other);
    };
};

/**
 * @class PLUG_ScriptingModuleInterface
 * @brief Interface for scripting modules that can be registered
 */
class PLUG_ScriptingModuleInterface {
public:
    PLUG_ScriptingModuleInterface();
    virtual ~PLUG_ScriptingModuleInterface();
};

/**
 * @class PLUG_ModuleLibraryInterface
 * @brief Interface for module library functionality
 */
class PLUG_ModuleLibraryInterface {
public:
    PLUG_ModuleLibraryInterface();
    virtual ~PLUG_ModuleLibraryInterface();
};

/**
 * @class PLUG_DragDropInterface
 * @brief Interface for drag and drop operations
 */
class PLUG_DragDropInterface {
public:
    PLUG_DragDropInterface();
    virtual ~PLUG_DragDropInterface();
};

/**
 * @class PLUG_PreferenceUI
 * @brief Abstract interface for preference panel UI
 */
class PLUG_PreferenceUI {
public:
    PLUG_PreferenceUI();
    virtual ~PLUG_PreferenceUI();

    /**
     * @brief Add a preference UI customizer
     * @param customizer Customizer interface to add
     */
    virtual void addCustomizer(const class PLUG_PreferenceUICustomizerInterface* customizer) = 0;
};

/**
 * @class PLUG_PreferenceUICustomizerInterface
 * @brief Interface for customizing preference panels
 */
class PLUG_PreferenceUICustomizerInterface {
public:
    /**
     * @enum eAppContext
     * @brief Application context for preference panels
     */
    enum eAppContext {
        // Values to be determined via further RE
    };

    PLUG_PreferenceUICustomizerInterface();
    virtual ~PLUG_PreferenceUICustomizerInterface();

    /**
     * @brief Called when creating preference panel
     * @param context Application context
     * @param parent Parent widget
     */
    virtual void onCreatePreferencePanel(eAppContext context, QWidget* parent) const;
};

/**
 * @class PLUG_InteractiveViewManager
 * @brief Abstract base class for interactive view management
 */
class PLUG_InteractiveViewManager {
public:
    virtual ~PLUG_InteractiveViewManager();

    // Pure virtual methods (to be discovered)
    virtual void invalidateAllViews(int flags) = 0;
    virtual void invalidateTimeline() = 0;
    virtual void registerDelegate(class PLUG_InteractiveViewDelegate* delegate) = 0;
    virtual void unregisterDelegate(class PLUG_InteractiveViewDelegate* delegate) = 0;
    virtual bool isDelegateTypeRegistered(class PLUG_InteractiveViewDelegate* delegate) = 0;
    virtual void releaseAllDelegates() = 0;
    virtual void clearAllDrawingSelection() = 0;

protected:
    PLUG_InteractiveViewManager();
};

/**
 * @class PLUG_Manager
 * @brief Base class for the plugin manager singleton
 */
class PLUG_Manager {
public:
    PLUG_Manager();
    virtual ~PLUG_Manager();
};

// ============================================================================
// Offset Constants (for reference/debugging)
// ============================================================================
namespace PLUG_ManagerImpl_Offsets {
    constexpr size_t CoreApp                     = 0x188;  // 392
    constexpr size_t UnknownInterface            = 0x190;  // 400
    constexpr size_t InteractiveRenderManager    = 0x198;  // 408
    constexpr size_t ActionManager               = 0x1A0;  // 416
    constexpr size_t Selection                   = 0x1A8;  // 424
    constexpr size_t CurrentFrameInterface       = 0x1B0;  // 432
    constexpr size_t EditionStackInterface       = 0x1B8;  // 440
    constexpr size_t SessionContext              = 0x1C0;  // 448
    constexpr size_t ScriptingInterface          = 0x1C8;  // 456
    constexpr size_t ToolbarService              = 0x1D0;  // 464
    constexpr size_t MenuService                 = 0x1D8;  // 472
    constexpr size_t ImportEngine                = 0x1E0;  // 480
    constexpr size_t KeyStateInterface           = 0x1E8;  // 488
    constexpr size_t LayoutManager               = 0x1F0;  // 496
    constexpr size_t DragDropInterface           = 0x1F8;  // 504
    constexpr size_t ExprScriptEngine            = 0x200;  // 512
    constexpr size_t WidgetFactoryRegistry       = 0x208;  // 520
    constexpr size_t InteractiveViewManager      = 0x210;  // 528
    constexpr size_t ColorManagerServices        = 0x218;  // 536
    constexpr size_t DataToolInterface           = 0x220;  // 544
    constexpr size_t HttpAPI                     = 0x228;  // 552
    constexpr size_t OGLRenderPlaybackInterface  = 0x230;  // 560
    constexpr size_t VectorizationInterface      = 0x238;  // 568
    constexpr size_t NetworkViewInterface        = 0x240;  // 576
    constexpr size_t UnknownString               = 0x248;  // 584
    constexpr size_t Flag                        = 0x260;  // 608
}
