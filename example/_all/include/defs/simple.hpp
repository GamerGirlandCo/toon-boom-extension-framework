#pragma once
#include <basic_view.hpp>
#include <examples_container.hpp>
#include "./base.hpp"

class SimpleBaseExample : public BaseExample {
public:
	SimpleBaseExample() : BaseExample() {
		m_container = new SimpleExamplesContainer();
	}
	~SimpleBaseExample() {}
protected:
	SimpleExamplesContainer *m_container = nullptr;
};

class SimpleExample : public SimpleBaseExample {
public:
	SimpleExample() : SimpleBaseExample() {}
	~SimpleExample() {}
	
	QString jsName() override {
		return "showSimpleExample";
	}

	std::function<QScriptValue(QScriptContext *, QScriptEngine *)> run() override {
		return [this](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
			m_container->showBasicGreetingView();
			return engine->undefinedValue();
		};
	}
};

class ToolbarExample : public SimpleBaseExample {
public:
	ToolbarExample() : SimpleBaseExample() {}
	~ToolbarExample() {}

	std::function<QScriptValue(QScriptContext *, QScriptEngine *)> run() override {
		return [this](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
			m_container->showCounterView();
			return engine->undefinedValue();
		};
	}
	QString jsName() override {
		return "showToolbarExample";
	}
};