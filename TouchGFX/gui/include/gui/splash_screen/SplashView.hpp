#ifndef SPLASHVIEW_HPP
#define SPLASHVIEW_HPP

#include <gui_generated/splash_screen/SplashViewBase.hpp>
#include <gui/splash_screen/SplashPresenter.hpp>

class SplashView : public SplashViewBase
{
public:
    SplashView();
    virtual ~SplashView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

protected:
    bool     holdActive;
    uint16_t holdTicks;
    static const uint16_t HOLD_TICKS;
};

#endif // SPLASHVIEW_HPP
