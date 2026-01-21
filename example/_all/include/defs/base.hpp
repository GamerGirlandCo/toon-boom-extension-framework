#pragma once
#define LIB_ALL
#include "QtScript/qscriptengine.h"
#include <QtCore/QtCore>
#include <QtScript/QtScript>

typedef QScriptValue(FunctionSignature)(QScriptContext *, QScriptEngine *);

/* // Usage
void setupScriptingModern(QScriptEngine *engine, BaseExample *example) {
    auto lambda = example->run();

    // Automatically deduces type and creates wrapper
    QScriptValue scriptFunc = CallbackWrapper<decltype(lambda)>::create(
        engine, std::move(lambda)
    );

    engine->globalObject().setProperty(example->jsName(), scriptFunc);
} */

class BaseExample {
public:
  BaseExample() {}
  ~BaseExample() {}
  virtual QString jsName() = 0;
  virtual std::function<QScriptValue(QScriptContext *, QScriptEngine *)>
  run() = 0;
};

template <typename Lambda> struct CallbackWrapper {
  // The actual function pointer that can be passed to C APIs
  static QScriptValue invoke(QScriptContext *ctx, QScriptEngine *eng) {
    void *ptr = ctx->callee().data().toVariant().value<void *>();
    auto *lambda = static_cast<Lambda *>(ptr);
    return (*lambda)(ctx, eng);
  }

  // Helper to create a script function from any lambda
  static QScriptValue create(QScriptEngine *engine, Lambda &&lambda) {
    // Store lambda in heap-allocated wrapper
    auto *stored = new Lambda(std::forward<Lambda>(lambda));

    QScriptValue func = engine->newFunction(CallbackWrapper<Lambda>::invoke);
    func.setData(engine->newVariant(QVariant::fromValue<void *>(stored)));

    // Attach finalizer for cleanup
    engine->newQObject(func, nullptr, QScriptEngine::ScriptOwnership);

    return func;
  }
};