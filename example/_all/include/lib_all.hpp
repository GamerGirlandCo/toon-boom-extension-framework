#pragma once

#include <QtScript/qscriptcontext.h>
#include <QtCore/QMap>
#include <QtCore/QtCore>
#include <QtScript/qscriptengine.h>
#include <vector>
#include "./defs/base.hpp"
#include "./defs/simple.hpp"
#include "./defs/doom.hpp"

class ToonBoomExamples {
public:
  ToonBoomExamples() {
    addExample(new SimpleExample());
    addExample(new ToolbarExample());
    addExample(new DoomExample());
  }
  void addExample(BaseExample *example) { examples.push_back(example); }
  QScriptValue getExamples(QScriptEngine *engine) {
    QScriptValue obj = engine->newObject();
    for (auto &example : examples) {
      auto lambda = example->run();

      QScriptValue scriptFunc =
          CallbackWrapper<decltype(lambda)>::create(engine, std::move(lambda));

      obj.setProperty(example->jsName(), scriptFunc);
    }
    return obj;
  }
  ~ToonBoomExamples() {}

private:
  std::vector<BaseExample *> examples;
};