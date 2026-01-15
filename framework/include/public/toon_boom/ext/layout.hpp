#pragma once
#include "../PLUG_Services.hpp"
#include "../toon_boom_layout.hpp"
#include "QtXml/qdom.h"
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/Qt>
#include <QtWidgets/QWidget>
#include <iostream>
#include <type_traits>

template <typename T>
concept isQWidget = std::is_base_of<QWidget, T>::value;

/**
 * @brief Simple base class for implementing TULayoutView with a QWidget, with
 all the ugly parts abstracted away.

 * @tparam T The QWidget type to use
 */
template <isQWidget T> class TUWidgetLayoutViewBase : public TULayoutView {
public:
  TUWidgetLayoutViewBase() { m_widget = nullptr; };
  TUWidgetLayoutViewBase(T *widget) { m_widget = widget; };
  virtual ~TUWidgetLayoutViewBase() {
  };
  virtual void triggerMenuChanged() override {}
  QWidget *widget() override {
    return reinterpret_cast<QWidget *>(static_cast<TULayoutView *>(this));
  }
  const QWidget *getWidget() const override {
    return m_widget;
  }
  QWidget *getWidget() override {
    ensureWidget();
    return m_widget;
  }
  TULayoutView *initiate(QWidget *parent) override {
    ensureWidget();
    if (parent && m_widget) {
      m_widget->setParent(parent);
    }
    QObject::connect(
        parent, &QObject::destroyed, m_widget,
        [this]() {
          if (m_widget) {
            m_widget->setParent(nullptr); // Prevent Qt from deleting us
          }
        },
        Qt::DirectConnection); // DirectConnection ensures this runs BEFORE
                               // deletion

    return this;
  }

  void isTULayoutView() override {}
  void disconnectView() override {}

protected:
  QPointer<T> m_widget;
  void ensureWidget() {
    if (!m_widget) {
      m_widget = createWidget();
      m_widget->setAttribute(Qt::WA_DeleteOnClose, false);
      m_widget->setAttribute(Qt::WA_QuitOnClose, false);
    }
  }
  void isTULayoutView() override {}
  void disconnectView() override {
    
  }
  virtual T *createWidget() = 0;
};