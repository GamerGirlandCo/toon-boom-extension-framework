#pragma once
#include "./toon_doom.hpp"
class DoomView : public TUWidgetLayoutViewBase<ToonDoomWidget> {
public:
	DoomView();
	~DoomView() override;
	QString displayName() const override;
protected:
	ToonDoomWidget *createWidget() override;
};