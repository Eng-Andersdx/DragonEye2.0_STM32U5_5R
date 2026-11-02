#ifndef SETTINGSPRESENTER_HPP
#define SETTINGSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SettingsView;

class SettingsPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SettingsPresenter(SettingsView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~SettingsPresenter() {}

private:
    SettingsPresenter();

    SettingsView& view;
};

#endif // SETTINGSPRESENTER_HPP
