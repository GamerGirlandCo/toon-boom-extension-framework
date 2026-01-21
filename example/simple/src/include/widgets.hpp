#pragma once

#include <QtCore/qtmetamacros.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/QtWidgets>
#include <toon_boom/ac_manager.hpp>

class WidgetWrapper : public QFrame {
public:
  WidgetWrapper(QWidget *parent = nullptr) : QFrame(parent) {
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Expanding));
    m_wrapperLayout = new QVBoxLayout(this);
    m_wrapperFrame = new QGroupBox();
    m_wrapperLayout->addWidget(m_wrapperFrame, 1);
    m_wrapperLayout->setContentsMargins(10, 10, 10, 10);
  };
  ~WidgetWrapper() override {};

protected:
  QGroupBox *m_wrapperFrame;
  QBoxLayout *m_wrapperLayout;
};

class GreetingsWidget : public WidgetWrapper {
public:
  GreetingsWidget(QWidget *parent = nullptr);
  ~GreetingsWidget() override;

private:
  QVBoxLayout *m_mainLayout;
};

class CounterWidget : public WidgetWrapper, public AC_ResponderBase {
  Q_OBJECT
public:
  CounterWidget(QWidget *parent = nullptr);
  ~CounterWidget() override;
  static const QString &IDENTITY;
public slots:
  void onActionIncrementCounter();
  void onActionDecrementCounter();
  void onActionResetCounter();
  void onActionIncrementCounterValidate(AC_ActionInfo *info);
  void onActionDecrementCounterValidate(AC_ActionInfo *info);
  void onActionResetCounterValidate(AC_ActionInfo *info);
  int counter() const;

private:
  QVBoxLayout *m_mainLayout;
  QHBoxLayout *m_counterLayout;
  void updateCounterLabel();
  QGroupBox *m_counterFrame;
  int m_counter = 0;
  QLabel *m_counterLabel;
};