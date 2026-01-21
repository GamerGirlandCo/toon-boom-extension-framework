#pragma once
#include "./widgets.hpp"
#include <toon_boom/toon_boom_layout.hpp>  // Includes ac_manager.hpp via toolbar.hpp
#include <toon_boom/ext/layout.hpp>
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>

class CounterView : public TUWidgetLayoutViewBase<CounterWidget> {
public:
	CounterView();
	~CounterView() override;	

	CounterWidget *createWidget() override;
	QDomElement toolbar() override;

	QString displayName() const override;
	void onParentDisconnect() override;
	void afterWidgetCreated() override;
private:
	QDomDocument m_toolbarDoc;  // Must store document, not just element (QDomElement refs doc's data)
	void initToolbar();
	bool has_initialized_toolbar = false;
};