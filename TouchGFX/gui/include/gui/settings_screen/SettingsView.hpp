#ifndef SETTINGSVIEW_HPP
#define SETTINGSVIEW_HPP

#include <gui_generated/settings_screen/SettingsViewBase.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>

class SettingsView : public SettingsViewBase
{
public:
    SettingsView();
    virtual ~SettingsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* Interactions defined in TouchGFX Designer */
    virtual void onBrightnessChanged(int value);
    virtual void onLedToggled();
    virtual void onWifiToggled();
    virtual void onBtToggled();

protected:
    void updateBrightnessText();
    void updateWifiText();
    void updateBtText();
};

#endif // SETTINGSVIEW_HPP
