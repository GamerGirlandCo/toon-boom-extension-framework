#include "./include/basic_view.hpp"
#include <QtWidgets/QLabel>

BasicGreetingView::BasicGreetingView()
    : TUWidgetLayoutViewBase<GreetingsWidget>() {
}

GreetingsWidget *BasicGreetingView::createWidget() {
  auto w = new GreetingsWidget(nullptr);
  return w;
}

QString BasicGreetingView::displayName() const {
  return QString("Basic Greeting View 👋");
}

BasicGreetingView::~BasicGreetingView() {
}