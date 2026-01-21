#include "./include/widgets.hpp"
#include "QtCore/qobject.h"
#include <toon_boom/ac_manager.hpp>
#include <toon_boom/PLUG_Services.hpp>

GreetingsWidget::GreetingsWidget(QWidget *parent) : WidgetWrapper(parent) {
  m_mainLayout = new QVBoxLayout(m_wrapperFrame);
  m_mainLayout->setContentsMargins(5, 5, 5, 5);

  QLabel *label = new QLabel("greetings, universe ~ 🌸");
  label->setAlignment(Qt::AlignCenter);
  label->setFont(QFont("Courier New", 48, QFont::Bold));
  label->setStyleSheet("color: #00d9db;");
  label->setWordWrap(true);

  QLabel *subtitle = new QLabel("this isn't your usual harmony view....");
  subtitle->setAlignment(Qt::AlignCenter);
  subtitle->setFont(QFont("Courier New", 18, -1, true));
  subtitle->setWordWrap(true);

  m_mainLayout->addWidget(label);
  m_mainLayout->addStretch();
  m_mainLayout->addWidget(subtitle);
}

GreetingsWidget::~GreetingsWidget() {}

const QString &CounterWidget::IDENTITY = QString::fromStdString("counter");
CounterWidget::CounterWidget(QWidget *parent)
    : WidgetWrapper(parent),
      AC_ResponderBase(IDENTITY, this,
                       PLUG_Services::getActionManager()) {
  auto asResponderBase = dynamic_cast<AC_ResponderBase *>(this);
  auto asResponder = dynamic_cast<AC_Responder *>(asResponderBase);
  actionManager()->registerResponder(asResponder, this); 
  m_mainLayout = new QVBoxLayout(m_wrapperFrame);
  QLabel *title = new QLabel("Test widget 2: Counter");
  title->setAlignment(Qt::AlignCenter);
  title->setFont(QFont("Courier New", 48, QFont::Bold));
  title->setStyleSheet("color: #00d9db;");
  title->setWordWrap(true);

  m_counterFrame = new QGroupBox(this);
  m_counterLayout = new QHBoxLayout(m_counterFrame);
  m_counterLayout->addStretch(1);
  auto counterValueLabel = new QLabel("counter value:");
  auto counterFont = QFont("Courier New", 24);
  counterValueLabel->setFont(counterFont);
  m_counterLayout->addWidget(counterValueLabel, 1);
  m_counterLabel = new QLabel("0");
  m_counterLabel->setFont(counterFont);
  m_counterLayout->addStretch(1);
  m_counterLayout->addWidget(m_counterLabel, 1);

  m_mainLayout->addWidget(title, 0, Qt::AlignCenter);
  m_mainLayout->addStretch(1);
  m_mainLayout->addWidget(m_counterFrame, 0, Qt::AlignCenter);
}

void CounterWidget::onActionIncrementCounter() {
  m_counter++;
  updateCounterLabel();
}
void CounterWidget::onActionDecrementCounter() {
  m_counter--;
  updateCounterLabel();
}
void CounterWidget::onActionResetCounter() {
  m_counter = 0;
  updateCounterLabel();
}
void CounterWidget::updateCounterLabel() {
  m_counterLabel->setText(QString::number(m_counter));
}

void CounterWidget::onActionIncrementCounterValidate(AC_ActionInfo *info) {
  info->setEnabled(true);
}

void CounterWidget::onActionDecrementCounterValidate(AC_ActionInfo *info) {
  info->setEnabled(true);
}

void CounterWidget::onActionResetCounterValidate(AC_ActionInfo *info) {
  info->setEnabled(m_counter != 0);
}

int CounterWidget::counter() const { return m_counter; }

CounterWidget::~CounterWidget() { actionManager()->unregisterResponder(this); }