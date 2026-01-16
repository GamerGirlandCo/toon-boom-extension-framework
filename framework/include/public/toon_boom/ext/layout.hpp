#pragma once
#include "../PLUG_Services.hpp"
#include "../toon_boom_layout.hpp"
#include "./util.hpp"
#include "QtXml/qdom.h"
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/Qt>
#include <QtWidgets/QWidget>
#include <iostream>
#include <type_traits>

using namespace util;
template <typename T>
concept isQWidget = std::is_base_of<QWidget, T>::value;

/**
 * @brief Simple base class for implementing TULayoutView with a QWidget, with
 all the ugly parts abstracted away.

 * @tparam T The QWidget type to use
 */
template <isQWidget T> class TUWidgetLayoutViewBase : public TULayoutView {
public:
  TUWidgetLayoutViewBase() {
    m_widget = nullptr;
    m_parentConnected = nullptr;
  };
  TUWidgetLayoutViewBase(T *widget) {
    m_widget = widget;
    m_parentConnected = nullptr;
  };
  virtual ~TUWidgetLayoutViewBase() {};
  virtual void triggerMenuChanged() override {}
  QWidget *widget() override {
    return reinterpret_cast<QWidget *>(static_cast<TULayoutView *>(this));
  }
  const QWidget *getWidget() const override {
    const_cast<TUWidgetLayoutViewBase *>(this)->ensureWidget();
    const_cast<TUWidgetLayoutViewBase *>(this)->connectToParentIfNeeded();
    return m_widget;
  }
  QWidget *getWidget() override {
    ensureWidget();
    connectToParentIfNeeded();
    return m_widget;
  }
  // Note: initiate() is NOT called by Toon Boom! The actual parenting
  // is done externally via getWidget() + setParent(). We keep this for
  // API compatibility but the real work is done in connectToParentIfNeeded().
  TULayoutView *initiate(QWidget *parent) override {
    ensureWidget();
    if (parent && m_widget) {
      m_widget->setParent(parent);
      connectToParent(parent);
    }
    return this;
  }

  void isTULayoutView() override {}
  void disconnectView() override {}
  TULayoutFrame *getOwnerFrame() {
    auto lm = PLUG_Services::getLayoutManager();
    if (!lm) {
      return nullptr;
    }
    auto frame = lm->findFrame(this);
    if (!frame) {
      return nullptr;
    }
    return frame;
  }

protected:
  QPointer<T> m_widget;
  QWidget *m_parentConnected; // Track which parent we've connected to
  virtual void onParentDisconnect() {}
  virtual void afterWidgetCreated() {}

  void ensureWidget() {
    if (!m_widget) {
      m_widget = createWidget();
      afterWidgetCreated();
    }
  }

  // Connect to the widget's current parent to unparent before deletion
  // This prevents cross-DLL heap corruption when Qt tries to delete our widget
  void connectToParentIfNeeded() {
    if (!m_widget)
      return;
    QWidget *parent = m_widget->parentWidget();
    if (parent && parent != m_parentConnected) {
      connectToParent(parent);
    }
  }

  void connectToParent(QWidget *parent) {
    if (!parent || !m_widget)
      return;
    m_parentConnected = parent;
    QObject::connect(
        parent, &QObject::destroyed, m_widget.data(),
        [this]() {
          debug::out << "[parent destroyed] Unparenting widget to prevent "
                       "cross-DLL heap deletion"
                    << std::endl;
          if (m_widget) {
            m_widget->setParent(nullptr);
          }
          m_parentConnected = nullptr;
          onParentDisconnect();
        },
        Qt::DirectConnection);
  }
  /**
   * @brief convenience method to register a toolbar from an xml element
   */
  bool registerToolbar(const QDomElement &element, const QString &name) {
    auto am = PLUG_Services::getActionManager();
    QDomElement docEl = element;
    if (docEl.tagName() == "toolbars") {
      docEl = docEl.firstChildElement();
    }
    if (am) {
      QList<QString> ids;
      am->loadToolbars(element, ids);
      debug::out << "Registered toolbar with AC_Manager. IDs loaded: "
                << ids.size() << std::endl;
      for (const auto &id : ids) {
        debug::out << "  - " << id.toStdString() << std::endl;
      }
    } else {
      std::cerr << "Could not get AC_Manager!" << std::endl;
      return false;
    }
    auto layToolbarInfo = getToolbarInfo();
    QList<QString> btns;
    for (int i = 0; i < docEl.childNodes().size(); i++) {
      auto node = docEl.childNodes().at(i).toElement();
      btns.append(node.attribute("id"));
    }
    layToolbarInfo.setName(name);
    layToolbarInfo.setButtonConfig(&btns);
    layToolbarInfo.setButtonDefaultConfig(&btns);
    setToolbarInfo(layToolbarInfo);
    return true;
  }

  virtual T *createWidget() = 0;
};