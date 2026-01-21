#pragma once
#include <functional>
#include <map>
#include <QtCore/QtCore>
#include <toon_boom/toon_boom_layout.hpp>

class SimpleExamplesContainer {
public:
  SimpleExamplesContainer();
  ~SimpleExamplesContainer();
	void showBasicGreetingView();
	void showCounterView();
	private:
	bool addViewIfNotExists(const char* id, const QString& displayName, std::function<TULayoutView*()> viewFactory, bool isDocked = true, QSize minSize = QSize(500, 400), bool useMinSz = true);
	std::map<QString, TULayoutView*> m_views;
};