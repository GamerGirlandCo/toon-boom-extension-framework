/**
 * @file toon_boom_layout.hpp
 * @brief Reconstructed header files for ToonBoomLayout.dll classes
 *
 * These class definitions are reverse-engineered from ToonBoomLayout.dll
 * used in Toon Boom Harmony Premium and Storyboard Pro.
 *
 * WARNING: This is a reconstruction based on binary analysis. Member offsets
 * and sizes have been verified but exact types may differ from the original.
 *
 * @see RE/ToonBoomLayout_Classes.md for detailed analysis
 */

#ifndef TOON_BOOM_LAYOUT_HPP
#define TOON_BOOM_LAYOUT_HPP

#include "./toolbar.hpp"
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QEnterEvent>
#include <QtGui/QtGui>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QtWidgets>
#include <QtXml/QDomElement>
#include <map>
#include <vector>

// Forward declarations
class TULayoutView;
class TULayoutViewHolder;
class TULayoutFrame;
class TULayoutManager;
class TULayoutManager_Private;
class TULayoutArea;
class TULayoutSplitter;
class TULayoutMainWindow;
class TULayoutDisplayTools;
class TULayout;
class AC_Manager;
class AC_Menu;
class AC_ActionInfo;
class AC_Responder;
class AC_ResponderTemplate;
class TUWidgetLayoutView;
class GUT_TabBar;
class GUT_Tab;
class LAY_ToolbarInfo;
class UI_Splitter;
class WID_VBoxLayout;
class WID_HBoxLayout;

// =============================================================================
// AC_Responder - Defined in ac_manager.hpp
// =============================================================================
//
// The AC_Responder interface and AC_ResponderBase convenience class are
// defined in ac_manager.hpp. Include that header to create custom responders.
//
// To create a custom responder:
//   1. Inherit from AC_ResponderBase (provides default implementations)
//   2. Override perform() to handle actions via Qt slots
//   3. Register with AC_Manager::registerResponder()
//
// Example:
//   class MyResponder : public QObject, public AC_ResponderBase {
//       Q_OBJECT
//   public:
//       MyResponder(AC_Manager* mgr) : AC_ResponderBase("myResponder", mgr) {
//           mgr->registerResponder(this, nullptr);
//       }
//   public slots:
//       void onActionDoSomething() { /* handle action */ }
//   };
//
// Toon Boom's internal classes (for reference only - cannot be extended):
//   - AC_ResponderTemplate: Toon Boom's internal base class
//   - AC_ResponderTemplateWidget<T>: Template combining QWidget with responder
//
// Key vtable addresses (in ToonBoomLayout.dll):
//   - AC_Responder vtable: 0x18004cd28
//   - AC_ResponderTemplate vtable: 0x18004cdc8
//   - AC_ResponderTemplateWidget<QWidget> vtable: 0x18004ce68
// =============================================================================

/**
 * @brief Toolbar configuration and state for a view
 *
 * Stores position, visibility, and button configuration for a toolbar
 * associated with a TULayoutView.
 *
 * sizeof(LAY_ToolbarInfo) = 0x68 (104 bytes) on x64
 */
class LAY_ToolbarInfo {
public:
  LAY_ToolbarInfo();
  LAY_ToolbarInfo(const LAY_ToolbarInfo &other);
  LAY_ToolbarInfo(QString name, int x, int y, int width, int height, int index,
                  bool visible, bool isDefault);
  ~LAY_ToolbarInfo();

  LAY_ToolbarInfo &operator=(const LAY_ToolbarInfo &other);

  // Getters
  int getHeight() const;                         // +0x10
  const QString &getName() const;                // +0x18
  int getNewline() const;                        // +0x14 (bool as int)
  Qt::Orientation getOrientation() const;        // +0x30
  const Qt::ToolBarArea &getToolBarArea() const; // +0x34
  bool isDefault() const;                        // +0x16
  bool isVisible() const;                        // +0x15

  // Setters
  void setHeight(int height);                       // +0x10
  void setIndex(int index);                         // +0x08
  void setName(const QString &name);                // +0x18
  void setNewline(int newline);                     // +0x14
  void setOrientation(Qt::Orientation orientation); // +0x30
  void setToolBarArea(Qt::ToolBarArea area);        // +0x34
  void setVisible(bool visible);                    // +0x15
  void setWidth(int width);                         // +0x0C
  void setX(int x);                                 // +0x00
  void setY(int y);                                 // +0x04

  // Serialization
  void fromXml(const QDomElement &element);
  void toXml(QDomElement &element) const;

  // Button configuration
  const QList<QString> *getButtonConfig() const;             // +0x38
  void setButtonConfig(const QList<QString> *config);        // +0x38
  void setButtonDefaultConfig(const QList<QString> *config); // +0x50

private:
  // Member layout (x64):
  int m_x;                              // +0x00
  int m_y;                              // +0x04
  int m_index;                          // +0x08
  int m_width;                          // +0x0C
  int m_height;                         // +0x10
  bool m_newline;                       // +0x14
  bool m_visible;                       // +0x15
  bool m_isDefault;                     // +0x16
  char _padding1;                       // +0x17
  QString m_name;                       // +0x18 (24 bytes)
  Qt::Orientation m_orientation;        // +0x30
  Qt::ToolBarArea m_toolBarArea;        // +0x34
  QList<QString> m_buttonConfig;        // +0x38 (24 bytes)
  QList<QString> m_buttonDefaultConfig; // +0x50 (24 bytes)
};

/**
 * @brief Abstract base class for all layout views in Toon Boom
 *
 * Represents a dockable/tabbable view panel that can be displayed in a
 * TULayoutFrame. This is an abstract class - subclasses must implement
 * the pure virtual methods to provide actual functionality.
 *
 * LINKING: Requires ToonBoomLayout.lib - constructor/destructor are exported.
 *
 * sizeof(TULayoutView) = 0xB8 (184 bytes) on x64
 *
 * VTABLE ORDER (32 slots total):
 *   0:  ~TULayoutView()
 *   1:  widget() = 0
 *   2:  initiate()
 *   3:  getWidget() const = 0
 *   4:  getWidget() = 0
 *   5:  getParentHolderWidget() const
 *   6:  getParentHolderWidget()
 *   7:  hasMenu()
 *   8:  setMenu(AC_Manager*, ...)
 *   9:  setMenu(AC_Menu*, ...)
 *   10: menu()
 *   11: toolbar()
 *   12: setToolbarInfo()
 *   13: connectView()
 *   14: disconnectView()
 *   15: initializedFromCopy()
 *   16: getCaption()
 *   17: getDynamicTextForCaption()
 *   18: wantEditionStack()
 *   19: displayName()
 *   20: compositeChanged()
 *   21: dropOverComposite()
 *   22: wantComposites()
 *   23: initActionManager()
 *   24: wantDisplaySelector()
 *   25: isUsingDefaultDisplay()
 *   26: storeViewPreferences()
 *   27: loadViewPreferences()
 *   28: cshHelpId()
 *   29: triggerMenuChanged() = 0
 *   30: copy()
 *   31: isTULayoutView() = 0
 */
class TULayoutView {
public:
  /**
   * @brief Menu type enumeration for TULayoutView
   */
  enum class MenuType : int {
    Primary = 0,  // Main context menu
    Secondary = 1 // Secondary/overflow menu
  };

  // Constructors - implemented in ToonBoomLayout.dll
  TULayoutView();
  TULayoutView(const TULayoutView &other);
  virtual ~TULayoutView(); // slot 0

  TULayoutView &operator=(const TULayoutView &other);

  // ===== PURE VIRTUAL METHODS (MUST override all 5) =====
  virtual QWidget *widget() = 0;                   // slot 1
  virtual TULayoutView *initiate(QWidget *parent); // slot 2
  virtual const QWidget *getWidget() const = 0;    // slot 3
  virtual QWidget *getWidget() = 0;                // slot 4

  // ===== Virtual methods with default implementations =====
  virtual const TULayoutViewHolder *getParentHolderWidget() const; // slot 5
  virtual TULayoutViewHolder *getParentHolderWidget();             // slot 6
  virtual bool hasMenu();                                          // slot 7
  virtual void setMenu(AC_Manager *manager, const char *menuName,
                       MenuType type);                // slot 8
  virtual void setMenu(AC_Menu *menu, MenuType type); // slot 9
  virtual AC_Menu *menu(MenuType type);               // slot 10

  /**
   * @brief Returns the QDomElement defining this view's toolbar
   *
   * Override this method to provide a view-specific toolbar. The default
   * implementation returns an empty QDomElement (no toolbar).
   *
   * To access the AC_Manager for toolbar lookup, use:
   * @code
   * QDomElement MyView::toolbar() {
   *     AC_Manager* manager = TULayoutView_getActionManager(this);
   *     if (manager) {
   *         // manager->getToolbarElement() at vtable[52]
   *         // Returns QDomElement for a named toolbar
   *     }
   *     return QDomElement();
   * }
   * @endcode
   *
   * @return QDomElement defining toolbar, or empty element for no toolbar
   * @see TULayoutView_getActionManager()
   * @see docs/TULayoutView_Toolbar_Integration.md
   */
  virtual QDomElement toolbar(); // slot 11

  virtual void setToolbarInfo(const LAY_ToolbarInfo &info); // slot 12
  virtual void connectView() {}       // slot 13 (empty impl)
  virtual void disconnectView() {}    // slot 14 (empty impl)
  virtual bool initializedFromCopy(); // slot 15
  virtual QString getCaption(bool includeAdvanced) const; // slot 16
  virtual QString getDynamicTextForCaption() const;       // slot 17
  virtual bool wantEditionStack() const {
    return false;
  } // slot 18 (returns false)
  virtual QString displayName() const;              // slot 19
  virtual void compositeChanged(const QString &) {} // slot 20 (empty impl)
  virtual void dropOverComposite(QDropEvent *, const QString &) {
  } // slot 21 (empty)
  virtual bool wantComposites() const {
    return false;
  } // slot 22 (returns false)
  virtual void initActionManager(AC_Manager *) {} // slot 23 (empty impl)
  virtual bool wantDisplaySelector() const {
    return false;
  } // slot 24 (returns false)
  virtual bool isUsingDefaultDisplay() const {
    return false;
  } // slot 25 (returns false)
  virtual bool storeViewPreferences(QDomElement &) const {
    return false;
  } // slot 26
  virtual void loadViewPreferences(const QDomElement &) {
  } // slot 27 (empty impl)
  virtual QString cshHelpId(); // slot 28

  // ===== MORE PURE VIRTUALS =====
  virtual void triggerMenuChanged() = 0; // slot 29 - MUST override!

  // Non-virtual methods
  void setCaption(const QString &caption);
  const LAY_ToolbarInfo &getToolbarInfo();
  TULayoutFrame *getLayoutFrame(const QWidget *widget) const;

  // Static
  static bool inClosingState();

protected:
  virtual void copy(const TULayoutView &other); // slot 30
  virtual void isTULayoutView() = 0;            // slot 31 - MUST override!

private:
  // Member layout (x64):
  // vptr at +0x00 (8 bytes)
  QString m_internalName;        // +0x08 (24 bytes) - "View{N}" by default
  LAY_ToolbarInfo m_toolbarInfo; // +0x20 (104 bytes)
  AC_Menu *m_menuByType[2];      // +0x88 (16 bytes) - indexed by MenuType
  bool m_initializedFromCopy;    // +0x98 (1 byte)
  char _padding[7];              // +0x99
  QString m_caption;             // +0xA0 (24 bytes)

  static int _uniqueId; // Static counter for unique names
};

// Note: AC_ResponderTemplateWidget<T> is a template class defined in Toon Boom
// DLLs that combines a QWidget subclass with AC_Responder capabilities. Memory
// layout for AC_ResponderTemplateWidget<QWidget>:
// - +0x00: vptr (QObject)
// - +0x10: vptr (QPaintDevice)
// - +0x18-0x27: QWidget members
// - +0x28: vptr (AC_ResponderTemplateWidget)
// - +0x30: AC_Manager* m_actionManager
// - +0x38: QString m_responderIdentity
// - +0x50: QString m_responderDescription
// sizeof(AC_ResponderTemplateWidget<QWidget>) ≈ 0x68 (104 bytes) on x64

// =============================================================================
// TUWidgetLayoutView
// =============================================================================
//
// TUWidgetLayoutView is a concrete layout view class implemented in
// ToonBoomLayout.dll. It combines a QWidget (for UI display) with both
// AC_Responder capabilities (for action handling) and TULayoutView
// functionality (for the layout system).
//
// INHERITANCE HIERARCHY:
//   QWidget
//       └── AC_ResponderTemplateWidget<QWidget>  (multiple inheritance)
//               ├── QWidget base
//               └── AC_ResponderTemplate (mixin at +0x28)
//                       └── TUWidgetLayoutView
//                               └── TULayoutView (embedded at +0x68)
//
// VTABLE STRUCTURE (4 vtables due to multiple inheritance):
//   +0x00: vptr[0] - QObject vtable (includes QWidget virtuals)
//   +0x10: vptr[1] - QPaintDevice vtable
//   +0x28: vptr[2] - AC_ResponderTemplateWidget<QWidget> / AC_Responder vtable
//   +0x68: vptr[3] - TULayoutView vtable
//
// MEMORY LAYOUT (x64 MSVC, sizeof = 0x120 / 288 bytes):
//   +0x00: vptr (QObject)
//   +0x08: QObjectData* d_ptr (QObject)
//   +0x10: vptr (QPaintDevice)
//   +0x18: [QWidget internal data ~16 bytes]
//   +0x28: vptr (AC_ResponderTemplateWidget<QWidget>)
//   +0x30: AC_Manager* m_actionManager
//   +0x38: QString m_responderIdentity (24 bytes)
//   +0x50: QString m_responderDescription (24 bytes)
//   +0x68: vptr (TULayoutView) ─┐
//   +0x70: QString m_internalName     │ TULayoutView
//   +0x88: LAY_ToolbarInfo m_toolbarInfo   │ embedded
//   +0xF0: AC_Menu* m_menuByType[2]   │ (184 bytes)
//   +0x100: bool m_initializedFromCopy │
//   +0x108: QString m_caption        ─┘
//
// KEY VIRTUAL METHOD OVERRIDES IN TUWIDGETLAYOUTVIEW:
//   From TULayoutView:
//     - widget() - pure virtual, not implemented (returns _purecall)
//     - getWidget() const/non-const - returns (this - 104), i.e., the QWidget*
//     - triggerMenuChanged() - emits menuChanged() signal on QWidget
//     - isTULayoutView() - empty implementation (RTTI marker)
//   From QWidget:
//     - mousePressEvent() - accepts event and sets focus (Qt::MouseFocusReason)
//     - metaObject(), qt_metacast(), qt_metacall() - Qt meta-object support
//
// SIGNALS (Qt):
//   - menuChanged() - emitted by triggerMenuChanged()
//
// CONSTRUCTOR PARAMETERS:
//   TUWidgetLayoutView(AC_Manager* manager, const QString& objectName,
//                      QWidget* parent, const char* className,
//                      Qt::WindowFlags flags)
//
// CONSTRUCTION SEQUENCE:
//   1. AC_ResponderTemplateWidget<QWidget> ctor (parent, flags, objectName)
//   2. TULayoutView default ctor (at this+104)
//   3. Set all 4 vtables to TUWidgetLayoutView vtables
//   4. QWidget::setMinimumWidth(150)
//   5. If parent and manager: call initActionManager(manager)
//      Else: store manager at +0x30
//
// DESTRUCTION SEQUENCE:
//   1. Reset vtables
//   2. Destroy TULayoutView::m_caption at +0x108
//   3. Destroy TULayoutView::m_toolbarInfo at +0x88
//   4. Destroy TULayoutView::m_internalName at +0x70
//   5. Call AC_ResponderTemplateWidget<QWidget> dtor
//
// EXPORTED METHODS (ToonBoomLayout.dll):
//   - ??0TUWidgetLayoutView@@QEAA@... - Constructor
//   - ??1TUWidgetLayoutView@@UEAA@XZ - Destructor
//   - ?getWidget@TUWidgetLayoutView@@UEAA/UEBAPEAVQWidget@@XZ - Get QWidget*
//   - ?mousePressEvent@TUWidgetLayoutView@@MEAAXPEAVQMouseEvent@@@Z
//   - ?triggerMenuChanged@TUWidgetLayoutView@@MEAAXXZ
//   - ?menuChanged@TUWidgetLayoutView@@QEAAXXZ - Qt signal
//   - ?metaObject/qt_metacast/qt_metacall - Qt meta-object methods
//   - ?tr@TUWidgetLayoutView@@SA?AVQString@@PEBD0H@Z - Translation
//
// USAGE NOTES:
//   Since TUWidgetLayoutView has external base classes, you cannot directly
//   subclass it in user code. Instead:
//   1. Obtain TUWidgetLayoutView* from the layout system
//   2. Use helper functions to convert between pointer types
//   3. Access TULayoutView virtuals through TULayoutView_getLayoutView()
//   4. Access QWidget through TUWidgetLayoutView_getWidget()
//
// @see TULayoutView_getActionManager() for accessing AC_Manager*
// @see docs/TUWidgetLayoutView_Analysis.md for detailed analysis
// =============================================================================

// Forward declaration - implemented in ToonBoomLayout.dll
// Use the helper functions below for pointer conversion
class TUWidgetLayoutView;

/**
 * @brief Convert TUWidgetLayoutView* to its embedded TULayoutView*
 * @param widget Pointer to TUWidgetLayoutView
 * @return Pointer to embedded TULayoutView at offset +104
 */
inline TULayoutView *
TUWidgetLayoutView_getLayoutView(TUWidgetLayoutView *widget) {
  return reinterpret_cast<TULayoutView *>(reinterpret_cast<char *>(widget) +
                                          104);
}

/**
 * @brief Convert TULayoutView* back to containing TUWidgetLayoutView*
 * @param view Pointer to TULayoutView (must be embedded in TUWidgetLayoutView)
 * @return Pointer to containing TUWidgetLayoutView at offset -104
 */
inline TUWidgetLayoutView *
TULayoutView_getWidgetLayoutView(TULayoutView *view) {
  return reinterpret_cast<TUWidgetLayoutView *>(reinterpret_cast<char *>(view) -
                                                104);
}

/**
 * @brief Get the QWidget* from a TUWidgetLayoutView*
 * @param widget Pointer to TUWidgetLayoutView
 * @return The QWidget* (same address, different type)
 */
inline QWidget *TUWidgetLayoutView_getWidget(TUWidgetLayoutView *widget) {
  return reinterpret_cast<QWidget *>(widget);
}

/**
 * @brief Get AC_Manager* from a TULayoutView* embedded in TUWidgetLayoutView
 *
 * This function provides access to the AC_Manager instance for views that
 * inherit from TUWidgetLayoutView. The AC_Manager is stored in the
 * AC_ResponderTemplateWidget<QWidget> base class at offset +48, which is
 * offset -56 from the embedded TULayoutView*.
 *
 * @note This only works for TULayoutView instances that are embedded in
 *       TUWidgetLayoutView. Direct TULayoutView subclasses do NOT have
 *       an AC_Manager member.
 *
 * @param view Pointer to TULayoutView (must be embedded in TUWidgetLayoutView)
 * @return AC_Manager* or nullptr if invalid
 *
 * @code
 * // Example usage in a toolbar() override:
 * QDomElement MyView::toolbar() {
 *     AC_Manager* manager = TULayoutView_getActionManager(this);
 *     if (manager) {
 *         // Use manager->getToolbarElement(...) to get toolbar definition
 *     }
 *     return QDomElement();
 * }
 * @endcode
 */
inline AC_Manager *TULayoutView_getActionManager(TULayoutView *view) {
  // AC_Manager is at offset +48 in TUWidgetLayoutView
  // TULayoutView is embedded at offset +104 in TUWidgetLayoutView
  // So from TULayoutView*, AC_Manager** is at offset -56 (= 48 - 104)
  return *reinterpret_cast<AC_Manager **>(reinterpret_cast<char *>(view) - 56);
}

/**
 * @brief Container widget that holds 1-2 TULayoutView instances
 *
 * When holding 2 views, they are displayed in a vertical splitter.
 * Single views are displayed directly in the layout.
 *
 * sizeof(TULayoutViewHolder) = 0x70 (112 bytes) on x64
 */
class TULayoutViewHolder : public QWidget {
  Q_OBJECT

public:
  explicit TULayoutViewHolder(QWidget *parent = nullptr);
  virtual ~TULayoutViewHolder();

  /**
   * @brief Add a view to this holder
   * @param view The view to add
   * @param splitterRatio Initial splitter ratio (only used when adding 2nd
   * view)
   * @return true if added successfully, false if holder is full (max 2)
   */
  bool addView(TULayoutView *view, double splitterRatio = 0.5);

  /**
   * @brief Remove a view from this holder
   * @param view The view to remove
   * @return true if removed successfully
   */
  bool removeView(TULayoutView *view);

  /**
   * @brief Get the number of views in this holder
   * @return 0, 1, or 2
   */
  int nbViews() const;

  /**
   * @brief Get the current splitter ratio
   * @return Ratio between 0.0 and 1.0 (left/total)
   */
  double splitterRatio();

  /**
   * @brief Set focus to the first view's widget
   */
  void setFocusToChild();

  /**
   * @brief Update widget visibility and layout
   */
  void updateWidgets();

  // Static translation helper

private:
  bool removeWidgetFromLayout(QWidget *widget, QLayout *layout);

  // Member layout (x64):
  // QWidget members at +0x00 to +0x27
  std::vector<TULayoutView *> m_views; // +0x28 (24 bytes)
  double m_savedSplitterRatio;         // +0x40 (8 bytes) - default 0.5
  UI_Splitter *m_splitter;             // +0x48 (8 bytes) - vertical splitter
  WID_VBoxLayout *m_leftLayout;        // +0x50 (8 bytes)
  WID_VBoxLayout *m_rightLayout;       // +0x58 (8 bytes)
  QFrame *m_leftFrame;                 // +0x60 (8 bytes)
  QFrame *m_rightFrame;                // +0x68 (8 bytes)
};

/**
 * @brief Top-level dockable/floatable frame with tabbed view holders
 *
 * Contains a tab bar for switching between view holders, toolbar area,
 * and various control buttons.
 *
 * Inherits from QFrame for border painting and styling.
 */
class TULayoutFrame : public QFrame {
  Q_OBJECT

public:
  /**
   * @brief Construct a TULayoutFrame
   * @param manager Action/menu manager for creating menus
   * @param parent Parent widget
   * @param name Object name for this frame
   * @param layoutManager The TULayoutManager managing this frame
   * @param floating True if this is a floating window (false = docked)
   * @param unknown Unknown parameter
   * @param singleViewMode True for single view mode
   */
  TULayoutFrame(AC_Manager *manager, QWidget *parent, const QString &name,
                TULayoutManager *layoutManager, bool floating, bool unknown,
                bool singleViewMode);
  virtual ~TULayoutFrame();

  // Accessors
  TULayoutManager *getLayoutManager();
  QStackedWidget *getStack() const;
  GUT_TabBar *getTab();
  AC_Toolbar *getToolbar() const;
  bool isDocked() const;
  bool isInSingleViewMode() const;
  bool isVisibleFullScreen() const;

  // Tab management
  int currentTab() const;
  QString currentTabName() const;
  TULayoutViewHolder *currentTabViewHolder();
  const GUT_Tab *addTab(TULayoutViewHolder *holder, const QString &name);
  void delTab(TULayoutViewHolder *holder);
  void setCurrentTab(int index);
  void setCurrentTab(TULayoutViewHolder *holder);
  bool updateTabName(TULayoutViewHolder *holder);
  void setTabVisible(bool visible);

  // View management
  void delView(QWidget *widget);
  void delViewHolder(TULayoutViewHolder *holder);
  TULayoutView *newTab(int areaIndex);
  void newView(int areaIndex);
  QWidget *getViewWidget(int index);
  void setCurrentViewByIndex(unsigned int index);

  // State management
  void setCurrent(bool current);
  void setFocusToChild() const;
  void updateCaption();
  void updateFocus();
  void updateVisibilityStatus();
  void updateTabHolderSplitters();
  void emitViewAdded();
  void postCustomRaiseEvent();

  // Toolbar management
  const LAY_ToolbarInfo *getToolbarInfoForView(const QString &viewName) const;
  void setToolbarInfoForView(const QString &viewName,
                             const LAY_ToolbarInfo &info);
  void showViewToolBar();
  void removeViewToolBar();
  void showToolBarMenu();
  void loadToolbars(QDomElement &element);
  void saveToolbars(QDomElement &element);
  void saveLayoutToolbar(QDomElement &element, QToolBar *toolbar);
  void createEmptyToolBar(const QString &name);

  // Composite/display management
  void enableComposite(bool enable);
  void enableEditionStack(bool enable);
  void compositeAdded(const QString &name);
  void compositeDeleted(const QString &name);
  void compositeOrGroupRenamed();
  void resetDisplayList();
  void resetDisplayList(const QString &name, const std::vector<QString> &items);

  // Static

signals:
  void viewAdded();

public slots:
  void handleCloseViewButton();
  void onActionCloseAllTabs();
  void onActionCloseCurrentTab();
  void onActionCloseTab(QString name);
  void onActionRaiseArea(QString name);
  void onActionRenameTab();
  void renameTab(int index);
  void syncCurrentTab();

  virtual void contextMenuEvent(QContextMenuEvent *event) override;
  virtual void enterEvent(QEnterEvent *event) override;
  virtual bool eventFilter(QObject *watched, QEvent *event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;

protected:
  // Qt event overrides
  virtual void closeEvent(QCloseEvent *event) override;
  virtual void paintEvent(QPaintEvent *event) override;

  // Internal methods
  void borderPaintEvent(QWidget *widget);
  void filterComposite(const QString &name);
  int findCompositeName(const QString &name);
  int findName(const QString &name);
  bool getCurToolbarState(LAY_ToolbarInfo &info);
  QString getFrameCaption() const;
  bool isLastDockFrame() const;
  void updateDisplaySelectorStatus();
  void updateEditionStackStatus();
  void updateMenuStatus();

private slots:
  void onDisplayChanged(int index);
  void onShowLayoutFrameMenu(AC_Menu *menu);
  void compositeChanged(const QString &name) const;
  void toolbarWasCustomized();

private:
  // Member layout (x64):
  // QFrame members at +0x00 to +0x27
  TULayoutManager *m_layoutManager;     // +0x28
  WID_VBoxLayout *m_mainLayout;         // +0x30
  WID_HBoxLayout *m_topHBoxLayout;      // +0x38
  QToolButton *m_menuButton;            // +0x40 - "View Menu" button
  TULayoutMainWindow *m_mainWindow;     // +0x48 - contains toolbar area
  TULayoutDisplayTools *m_displayTools; // +0x50 - display selector
  QComboBox *m_compositeCombo;          // +0x58 - composite selector
  GUT_TabBar *m_tabBar;                 // +0x60 - tab bar
  QStackedWidget *m_stack;             // +0x68 - stacked widget for tab content
  QWidget *m_editionStackWidget;       // +0x70 - symbol/edition stack
  QToolButton *m_closeButton;          // +0x78 - close button
  QToolButton *m_createViewMenuButton; // +0x80 - "Create View Menu" button
  bool m_isCurrent;                    // +0x88 - is focused frame
  bool m_isDocked;                     // +0x89 - true if docked (not floating)
  bool m_isInSingleViewMode;           // +0x8A - single view mode
  char _padding1;                      // +0x8B
  int _reserved1;                      // +0x8C
  AC_Toolbar *m_toolbar;               // +0x90 - view-specific toolbar
  int m_unknown;                       // +0x98 - initialized to -1
  int _reserved2;                      // +0x9C
  // Map/list structure at +0xA0-0xAF for toolbar info per view
  QWidget *m_parentWidget;    // +0xB0
  AC_Menu *m_layoutFrameMenu; // +0xB8 - context menu
  AC_Toolbar *m_viewToolbar;  // +0xC0 - currently active toolbar
};

/**
 * @brief Lightweight QMainWindow for toolbar hosting inside TULayoutFrame
 *
 * Used inside TULayoutFrame to provide toolbar docking areas for
 * view-specific toolbars. The QStackedWidget is set as its central widget.
 *
 * sizeof(TULayoutMainWindow) = 0x30 (48 bytes) on x64
 */
class TULayoutMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit TULayoutMainWindow(AC_Manager *manager, QWidget *parent = nullptr);
  virtual ~TULayoutMainWindow();

  AC_Manager *getManager() const { return m_manager; }

private:
  // Member layout (x64):
  // QMainWindow members at +0x00 to +0x27
  AC_Manager *m_manager; // +0x28 - Action manager reference
};

/**
 * @brief Base class for layout persistence and configuration
 *
 * Provides layout loading, saving, and toolbar configuration management.
 * TULayoutManager inherits from this class.
 *
 * sizeof(TULayoutStorage) = 0x88 (136 bytes) on x64
 */
class TULayoutStorage {
public:
  /**
   * @brief Toolbar button configuration
   */
  struct ToolbarButtonConfig {
    QList<QString> buttons;
    bool isDefault;
  };

  TULayoutStorage(const QString &layoutPath);
  TULayoutStorage(const TULayoutStorage &other);
  TULayoutStorage &operator=(const TULayoutStorage &other);
  virtual ~TULayoutStorage();

  // Layout management
  TULayout *addLayout(const QString &name, int index = -1);
  virtual void delLayout(const QString &name, int index, bool notify);
  virtual void delLayout(TULayout *layout, bool notify);
  void delLayouts();
  TULayout *duplicateLayout(TULayout *layout);
  TULayout *findLayout(const QString &name, int index = -1);
  int findLayoutIndex(const QString &name, int index = -1);
  int findLayoutIndex(TULayout *layout);
  TULayout *loadLayout(const QString &name, int index);
  virtual void loadLayouts(bool reloadFromDisk);
  void loadLayouts(const QDir &dir, int index);
  void renameLayout(TULayout *layout, const QString &newName);
  bool resetLayouts(const QDir &sourceDir);
  void saveLayout(const QString &name, int index);
  void saveLayout(TULayout *layout, bool notify);
  void saveLayoutToolbar();

  // Layout toolbar (workspace bar)
  void addToLayoutToolbar(TULayout *layout);
  void clearLayoutToolbar();
  std::vector<TULayout *> &getToolbarLayouts();
  void setLayoutToolbarIndex(TULayout *layout, int index);

  // Accessors
  TULayout *getCurrentLayout();
  TULayout *getPreviousLayout();
  size_t getLayoutCount();
  void setCurrentLayout(TULayout *layout);
  QString getFileNameForLayout(TULayout *layout, TULayout *parent = nullptr);
  bool isLayoutsModified() const;
  bool isLocked();

  // Toolbar configuration
  const QList<QString> *getGlobalToolbarConfig(const QString &name,
                                               bool defaultConfig) const;
  const QList<QString> *getViewToolbarConfig(const QString &name,
                                             bool defaultConfig) const;
  void setGlobalToolbarConfig(const QString &name, const QList<QString> &config,
                              bool isDefault);
  void setViewToolbarConfig(const QString &name, const QList<QString> &config,
                            bool isDefault);

protected:
  void loadLayoutInfo(const QString &path, bool initial);
  void selectDefaultLayout();
  void setUniqueLayout(QString &name);
  static void setUniqueLayout(const QList<QString> &existing, QString &name);
  void addToolbarConfig(std::map<QString, ToolbarButtonConfig> &configMap,
                        const QString &name, const QList<QString> &config,
                        bool isDefault);
  void clearToolbarConfig(std::map<QString, ToolbarButtonConfig> &configMap,
                          bool defaultOnly);

private:
  // Member layout (x64):
  // Total size must be exactly 0x88 (136) bytes for correct multiple inheritance offsets!
  // vptr at +0x00 (8 bytes, implicit)
  TULayout *m_currentLayout;                // +0x08
  TULayout *m_previousLayout;               // +0x10
  std::vector<TULayout *> m_layouts;        // +0x18 (24 bytes, ends at +0x30)
  std::vector<TULayout *> m_toolbarLayouts; // +0x30 (24 bytes, ends at +0x48)
  void *_padding1;                          // +0x48 (8 bytes padding)
  QString m_layoutPath;                     // +0x50 (24 bytes, ends at +0x68)
  std::map<QString, ToolbarButtonConfig> *m_globalToolbarConfig; // +0x68
  void *_reserved;                                               // +0x70
  std::map<QString, ToolbarButtonConfig> *m_viewToolbarConfig;   // +0x78
  int m_flags;                                                   // +0x80
  int _padding2;                                                 // +0x84 (4 bytes padding for alignment)
  // Total size: 0x88 (136 bytes)
};

/**
 * @brief Service interface for toolbar management
 *
 * Empty interface class for toolbar-related plugin services.
 */
class PLUG_ToolbarService {
public:
  PLUG_ToolbarService() = default;
  virtual ~PLUG_ToolbarService() = default;
};

/**
 * @brief Service interface for menu management
 *
 * Empty interface class for menu-related plugin services.
 */
class PLUG_MenuService {
public:
  PLUG_MenuService() = default;
  virtual ~PLUG_MenuService() = default;
};

/**
 * @brief Private implementation data for TULayoutManager
 *
 * Holds the internal state including frame, area, and splitter vectors.
 *
 * sizeof(TULayoutManager_Private) = 0x110 (272 bytes) on x64
 */
class TULayoutManager_Private : public QObject {
  Q_OBJECT

public:
  explicit TULayoutManager_Private(TULayoutManager *owner);
  virtual ~TULayoutManager_Private();

  // Accessors
  QFrame *getMainFrame() const { return m_mainFrame; }
  WID_VBoxLayout *getMainLayout() const { return m_mainLayout; }
  const std::vector<TULayoutSplitter *> &getSplitters() const {
    return m_splitters;
  }
  const std::vector<TULayoutFrame *> &getFrames() const { return m_frames; }
  const std::vector<TULayoutArea *> &getAreas() const { return m_areas; }
  const std::vector<TULayoutArea *> &getPluginAreas() const {
    return m_pluginAreas;
  }
  TULayoutFrame *getCurrentLayoutFrame() const { return m_currentLayoutFrame; }
  AC_Manager *getActionManager() const { return m_actionManager; }
  QPoint getSavedPos() const { return m_savedPos; }
  QSize getSavedSize() const { return m_savedSize; }

  // Mutators
  void setMainFrame(QFrame *frame) { m_mainFrame = frame; }
  void setMainLayout(WID_VBoxLayout *layout) { m_mainLayout = layout; }
  void setCurrentLayoutFrame(TULayoutFrame *frame) {
    m_currentLayoutFrame = frame;
  }
  void setActionManager(AC_Manager *manager) { m_actionManager = manager; }
  void setSavedPos(const QPoint &pos) { m_savedPos = pos; }
  void setSavedSize(const QSize &size) { m_savedSize = size; }

private:
  // Member layout (x64):
  // QObject members at +0x00 to +0x0F
  QFrame *m_mainFrame;                         // +0x10
  WID_VBoxLayout *m_mainLayout;                // +0x18
  char _reserved[0x48];                        // +0x20-0x5F (padding)
  std::vector<TULayoutSplitter *> m_splitters; // +0x60
  std::vector<TULayoutFrame *> m_frames;       // +0x78
  std::vector<TULayoutArea *> m_areas;         // +0x90
  std::vector<TULayoutArea *> m_pluginAreas;   // +0xA8
  QPoint m_savedPos;                           // +0xC0
  QSize m_savedSize;                           // +0xC8
  int m_stateFlags;                            // +0xD0
  int _padding;                                // +0xD4
  TULayoutFrame *m_currentLayoutFrame;         // +0xD8
  char _reserved2[0x10];                       // +0xE0-0xEF (padding)
  TULayoutManager *m_owner;                    // +0xF0
  AC_Manager *m_actionManager;                 // +0xF8
};

/**
 * @brief Central manager for the entire layout system
 *
 * Manages all frames, areas, splitters, toolbars, and layout persistence.
 * Inherits from QMainWindow, TULayoutStorage, PLUG_ToolbarService, and
 * PLUG_MenuService to provide comprehensive layout management functionality.
 *
 * This is the main entry point for the Toon Boom layout system.
 */
class TULayoutManager : public QMainWindow,
                        public TULayoutStorage,
                        public PLUG_ToolbarService,
                        public PLUG_MenuService {
  Q_OBJECT

public:
  explicit TULayoutManager(const QString &layoutPath);
  virtual ~TULayoutManager();

  // Frame management
  TULayoutFrame *addFrame(const QString &name, bool floating, bool unknown,
                          bool singleViewMode);
  TULayoutFrame *addFrame(QWidget *parent, const double &ratio, bool floating,
                          bool unknown, bool singleViewMode);
  void delFrame(TULayoutFrame *frame);
  bool closeFrame(TULayoutFrame *frame);
  TULayoutFrame *findFrame(int index) const;
  TULayoutFrame *findFrame(QWidget *widget) const;
  TULayoutFrame *findFrame(TULayoutView *view) const;
  TULayoutFrame *findFirstChildFrame(QWidget *widget);
  int findNoFrame(TULayoutFrame *frame);
  int getNbFrame() const;
  int getNbFrameDocked();
  const std::vector<TULayoutFrame *> &getFrames() const;
  TULayoutFrame *getCurrentLayoutFrame() const;
  void setCurrentLayoutFrame(TULayoutFrame *frame);

  // Area management
  bool addArea(const char *type, const QString &name, TULayoutView *view,
               bool visible, bool createFrame, bool docked,
               const QSize &minSize, bool useMinSize, bool isPlugin,
               bool defaultVisible, bool unknown);
  bool addPluginArea(const char *type, const QString &name, TULayoutView *view,
                     bool visible, bool createFrame, bool docked,
                     const QSize &minSize, bool useMinSize,
                     bool defaultVisible);
  void delAreas(TULayoutFrame *frame = nullptr);
  TULayoutArea *findArea(const QString &name);
  TULayoutArea *findArea(QWidget *widget);
  TULayoutArea *findArea(TULayoutFrame *frame);
  TULayoutArea *findArea(TULayoutView *view) const;
  int findNoArea(QWidget *widget);
  int findNoArea(TULayoutFrame *frame);
  int findNoArea(const char *name);
  int getNbArea() const;
  bool isValidAreaID(int id);
  int isValidAreaName(const QString &name);
  const std::vector<TULayoutArea *> &getAreas() const;
  void getAvailableWindows(std::vector<TULayoutArea *> &windows);
  void commitPluginAreas();

  // View management
  bool addView(const char *type, const QString &name, TULayoutView *&view,
               TULayoutFrame *&frame, bool visible, double ratio);
  bool addView(const char *type, const QString &name, TULayoutFrame *frame,
               double ratio, Qt::Orientation orientation, TULayoutView *&view,
               TULayoutFrame *&newFrame, bool visible, double splitRatio);
  bool addViewHorizontal(const char *type, const QString &name,
                         TULayoutFrame *frame, double ratio,
                         TULayoutView *&view, TULayoutFrame *&newFrame,
                         bool visible, double splitRatio);
  bool addViewVertical(const char *type, const QString &name,
                       TULayoutFrame *frame, double ratio, TULayoutView *&view,
                       TULayoutFrame *&newFrame, bool visible,
                       double splitRatio);
  bool addDetached(const QString &name, const QString &type,
                   TULayoutView *&view, TULayoutFrame *&frame);
  void deleteAllViews();
  TULayoutView *findInstance(const QString &name);
  TULayoutView *findInstance(const QWidget *widget);
  TULayoutView *findInstance(const TULayoutFrame *frame, bool current);
  TULayoutView *findInstance(const TULayoutViewHolder *holder, bool current);
  TULayoutView *findInstanceOfType(const QString &typeName);
  TULayoutView *findFirstInstance();
  void findAllInstances(const QString &typeName,
                        std::vector<TULayoutView *> &results);
  QString findInstanceName(TULayoutFrame *frame);
  void getViews(const TULayoutFrame *frame,
                std::vector<TULayoutView *> *views) const;
  bool hasView(const char *name);
  TULayoutView *currentView();
  bool validateView(const QString &name, TULayoutView *view);

  // Splitter management
  TULayoutSplitter *getRootSplitter();
  void delSplitter(TULayoutSplitter *splitter);
  int getNbSplitter() const;
  const std::vector<TULayoutSplitter *> &getSplitters() const;
  void sortFramesRecursive(TULayoutSplitter *splitter, int &index);
  int isFrame(QWidget *widget);
  int isSplitter(QWidget *widget);

  // Layout operations
  virtual bool changeLayout(const QString &name, int index);
  void deleteLayout();
  void updateLayout();
  void expandMainWindow();
  void showFrames();

  // Toolbar management
  virtual AC_Toolbar *addToolbar(const char *name);
  virtual void addToolbarFromFile(const QString &path, const QString &name,
                                  bool global);
  virtual void addToolbarFromElement(const QDomElement &element,
                                     const QString &name, bool global);
  virtual AC_Toolbar *showToolbar(const char *name, bool show, bool global,
                                  AC_ToolbarItemGenerator *generator);
  virtual bool setToolbarVisibility(const QString &name, bool visible);
  virtual bool removeToolbar(const char *name);
  virtual AC_Toolbar *toolbar(const char *name);
  void addGlobalToolbar(QToolBar *toolbar);
  void registerGlobalToolbar(const QToolBar *toolbar);
  void clearGlobalToolBarList();
  void getAllToolbars(std::list<QToolBar *> &viewToolbars,
                      std::list<QToolBar *> &globalToolbars,
                      bool includeHidden);
  bool isGlobalToolbar(const QString &name) const;
  bool isGlobalToolbar(const QToolBar *toolbar) const;
  virtual bool getMainToolbarDefaultVisibility(const QToolBar &toolbar) const;
  void showViewToolBars();
  void removeViewToolBar();
  void showDisplayToolBar(bool show);
  void showPortNameToolBar(bool show);
  void hideLayoutToolBar();
  void createWorkspaceToolbar();
  void destroyWorkspaceToolbar();
  void customizeToolBar();

  // Menu management
  virtual bool addMenuFromFile(const QString &path, const QString &name);
  virtual bool addMenuFromElement(const QString &name,
                                  const QDomElement &element);

  // Fullscreen
  void fullScreenFrame(TULayoutFrame *frame);
  void fullScreenMainFrame(TULayoutFrame *frame);
  void restoreNormalScreen();

  // Accessors
  void setActionManager(AC_Manager *manager);
  QFrame *getMainFrame() const;
  QString getDefaultLayoutsPath() const;
  virtual const unsigned int &getCurrentViewBorderColor();
  QString unconnectedDisplayNodeName() const;
  QString unconnectedDisplayNodeNameTranslated() const;
  QString noPortFilterNameTranslated() const;

  // Event handlers
  virtual void closeEvent(QCloseEvent *event) override;
  virtual void moveEvent(QMoveEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override;
  virtual QMenu *createPopupMenu() override;

  // Layout operations
  virtual void loadLayouts(bool reloadFromDisk) override;
  virtual void updateLayoutBar();
  virtual void updateLayoutBarSelection();
  virtual void onPreferenceChanged();
  virtual void onActionFullscreen();
  virtual void onActionFullscreenValidate(AC_ActionInfo *info);

  // Composite management
  void compositeAdded(const QString &name);
  void compositeDeleted(const QString &name);
  void raiseWindow();
  TULayoutView *raiseArea(const QString &name, TULayoutFrame *frame,
                          bool createNew, const QPoint &pos);
  TULayoutView *raiseArea(int areaIndex, TULayoutFrame *frame, bool createNew,
                          const QPoint &pos);
  TULayoutView *raiseArea(TULayoutArea *area, TULayoutFrame *frame,
                          bool createNew, const QPoint &pos);
  TULayoutView *newDetached(TULayoutArea *area, QPoint pos);
  void saveCurrentLayout();

signals:
  void backgroundImageChanged();
  void fullScreenStateChanged();
  void layoutChanged(TULayout *layout);
  void preferencesChange();
  void sceneSaved();

public slots:
  void onActionRaiseArea(QString name, bool createNew);
  void onActionSaveLayouts();
  void onActionSaveWorkspaceAs();
  void onActionShowLayoutManager();
  void onLayoutMenu(int index);
  void onPortNamesChanged(int index);
  void onPortNamesChanged(QString name);
  void onDefaultDisplayChanged(int index);

protected slots:
  void closeLayoutCheck();
  void refresh();
  void resetDisplayLists();
  void resetPortNamesLists();
  void compositeOrGroupRenamed();
  void updateAutoSaveLayoutVisibilityStatus();

protected:
  void layoutFrameBeingDeleted(TULayoutFrame *frame);

private:
  void sortToolbarList(QList<QToolBar *> list,
                       std::list<QToolBar *> &viewToolbars,
                       std::list<QToolBar *> &globalToolbars,
                       bool includeHidden);
  QToolBar *getToolbarUnderMouse();
  void moveGlobalToolbar();
  void toolbarWasCustomized(QString name);
  void updateToolBarScriptIcons(const QString &oldName, const QString &newName,
                                const QString &path);

  // Member layout (x64):
  // QMainWindow members at +0x00 to +0x27
  // TULayoutStorage members at +0x28 to +0xAF
  // PLUG_ToolbarService at +0xB0
  // PLUG_MenuService at +0xB8
  bool m_unknown;                     // +0xC0
  char _padding[7];                   // +0xC1-0xC7
  TULayoutManager_Private *m_private; // +0xC8
};

#endif // TOON_BOOM_LAYOUT_HPP
