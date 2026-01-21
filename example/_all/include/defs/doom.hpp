#pragma once

#include "./base.hpp"
#include <doom_view.hpp>

class DoomExample : public BaseExample {
public:
  DoomExample() : BaseExample() {
  }
  ~DoomExample() {}

  std::function<QScriptValue(QScriptContext *, QScriptEngine *)>
  run() override {
    return
        [this](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
          auto lm = PLUG_Services::getLayoutManager();
          if(doomView == nullptr) {
            doomView = new DoomView();
            lm->addArea("DoomView", doomView->displayName(), doomView, true, true, false, QSize(320, 200), true, false, true, true);
          }
          lm->raiseArea("DoomView", nullptr, true, QPoint(2020, 100));
          auto widget =
                dynamic_cast<ToonDoomWidget *>(doomView->getWidget());
                widget->start();
          return engine->undefinedValue();
        };
  }
  QString jsName() override { return "runDoom"; }

private:
  DoomView *doomView = nullptr;
};