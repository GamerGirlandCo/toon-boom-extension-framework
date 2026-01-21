#include "./include/toolbar_view.hpp"
#include "./include/widgets.hpp"
#include <QtXml/QtXml>
#include <iostream>
#include <toon_boom/PLUG_Services.hpp>
#include <toon_boom/ext/util.hpp>
#include <toon_boom/toon_boom_layout.hpp>
#include <toon_boom/ext/util.hpp>


using namespace util;


CounterView::CounterView()
    : TUWidgetLayoutViewBase<CounterWidget>() {}

CounterWidget *CounterView::createWidget() {
  auto w = new CounterWidget(nullptr);
  return w;
}

void CounterView::afterWidgetCreated() { initToolbar(); }

void CounterView::onParentDisconnect() {
  debug::out << "Parent disconnected" << std::endl;
  has_initialized_toolbar = false;
  m_toolbarDoc = QDomDocument(); // Clear the document
}
CounterView::~CounterView() {}

QString CounterView::displayName() const {
  return "Example Toolbar View";
}

void CounterView::initToolbar() {
  if (has_initialized_toolbar) {
    return;
  }
  QString errorMsg;
  int errorLine = 0;
  int errorCol = 0;
 
  std::string rawToolbarXml(R"XML(<?xml version="1.0" encoding="UTF-8"?>
<toolbars>
<toolbar id="TestToolbar" customizable="true" text="Test Toolbar" visible="true">
<item icon="timeline/add.svg" id="INCREMENT_COUNTER" slot="onActionIncrementCounter()" responder="counter" text="Increment Counter" />
<item icon="timeline/remove.svg" id="DECREMENT_COUNTER" slot="onActionDecrementCounter()" responder="counter" text="Decrement Counter" />
<item icon="view/resetview.svg" id="RESET_COUNTER" slot="onActionResetCounter()" responder="counter" text="Reset Counter" />
</toolbar>
</toolbars>
)XML");
  QString rawToolbarXmlQStr(rawToolbarXml.c_str());
  debug::out << "Toolbar xml: " << rawToolbarXml << std::endl;
  bool success = m_toolbarDoc.setContent(rawToolbarXmlQStr,
                                         &errorMsg, &errorLine, &errorCol);
  if (errorLine != 0 || errorCol != 0 || !success) {
    debug::out << "Error loading toolbar XML: "
              << " at line " << errorLine << ", column " << errorCol
              << std::endl;
    std::string errStr;
    for (auto &c : errorMsg) {
      errStr += c.toLatin1();
    }
    debug::out << "Error message: " << errStr << std::endl;
    return;
  }
  debug::out << "Toolbar document node count: "
            << m_toolbarDoc.documentElement().childNodes().size() << std::endl;

  has_initialized_toolbar = registerToolbar(m_toolbarDoc.documentElement(), "TestToolbar");
}

QDomElement CounterView::toolbar() {
  auto am = PLUG_Services::getActionManager();
  auto mgrEl = am->toolbarElement("TestToolbar");
  debug::out << "mgr el" << std::endl;
  /* if (mgrEl.isElement() && !mgrEl.isNull()) {
    auto tagName = mgrEl.tagName();
    debug::out << "\t<" << tagName.toStdString() << ">" << std::endl;
  } */
  debug::out << "\tis null: " << mgrEl.isNull() << std::endl
            << "\tis element:" << mgrEl.isElement() << std::endl;

  auto tbe = m_toolbarDoc.documentElement().firstChildElement();
  {
    auto id = tbe.attribute("id");
    auto asStr = id.toStdString();
    debug::out << "Getting toolbar: " << asStr
    << std::endl;
  }
  debug::out << "item count: " << tbe.childNodes().size() << std::endl;
  return tbe;
}
