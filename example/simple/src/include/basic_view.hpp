#pragma once
#include "./widgets.hpp"
#include <toon_boom/ext/layout.hpp>
#include <QtCore/QPointer>
typedef AC_Toolbar* (__fastcall *toolbar_fn)(void*, const char*);

class BasicGreetingView : public TUWidgetLayoutViewBase<GreetingsWidget> {
public:
  BasicGreetingView();
  ~BasicGreetingView() override;

  GreetingsWidget *createWidget() override;

  QString displayName() const override;
};
