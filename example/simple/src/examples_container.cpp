#include <toon_boom/PLUG_Services.hpp>
#include "./include/examples_container.hpp"
#include "./include/toolbar_view.hpp"
#include "./include/basic_view.hpp"
#include "toon_boom/ext/util.hpp"

SimpleExamplesContainer::SimpleExamplesContainer() {}

SimpleExamplesContainer::~SimpleExamplesContainer() {}

void SimpleExamplesContainer::showBasicGreetingView() {
	auto lm = PLUG_Services::getLayoutManager();
	if (!lm) {
		return;
	}
	const char* id = "BasicGreetingView";
	const QString name = "Basic Greeting View";
	auto viewFactory = []() -> TULayoutView* { return new BasicGreetingView(); };
	if (!addViewIfNotExists(id, name, viewFactory, false, QSize(400, 400), true)) {
		return;
	}

	lm->raiseArea(name, nullptr, true, QPoint(100, 100));
}

void SimpleExamplesContainer::showCounterView() {
	auto lm = PLUG_Services::getLayoutManager();
	auto am = PLUG_Services::getActionManager();

	if (!lm) {
		std::cerr << "Failed to get layout manager!" << std::endl;
		return;
	}
	if (!am) {
		std::cerr << "Failed to get action manager!" << std::endl;
		return;
	}

	const QString name = "Counter View";

	auto viewFactory = []() -> TULayoutView* { return new CounterView(); };
	if (!addViewIfNotExists("Counter View", name, viewFactory, false, QSize(700, 400), true)) {
		return;
	}
	auto area = lm->raiseArea(name, nullptr, true, QPoint(200, 200));
	debug::out << "Area: " << debug::addrToHex(area) << std::endl;
	auto asCounterView = dynamic_cast<CounterView*>(m_views[name]);
	asCounterView->getWidget()->setFocus(Qt::OtherFocusReason);
	lm->showViewToolBars();
}

bool SimpleExamplesContainer::addViewIfNotExists(const char* id, const QString& displayName, std::function<TULayoutView*()> viewFactory, bool isDocked, QSize minSize, bool useMinSz) {
	auto lm = PLUG_Services::getLayoutManager();
	std::string asStr;
	for (auto &c : displayName) {
		asStr += c.toLatin1();
	}
	if (!m_views.contains(displayName)) {
		m_views[displayName] = viewFactory();
		bool res = lm->addArea(id, displayName, m_views[displayName], true, true, isDocked, minSize, useMinSz, false, true, true);
		if (!res) {
			debug::out << "Failed to add view " << asStr << " to layout!" << std::endl;
			return false;
		}
		debug::out << "Successfully added view " << asStr << " to layout!" << std::endl;
	}
	return true;
}