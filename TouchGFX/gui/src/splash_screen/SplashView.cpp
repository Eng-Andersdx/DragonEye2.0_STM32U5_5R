#include <gui/splash_screen/SplashView.hpp>

const uint16_t SplashView::HOLD_TICKS = 180; // 3 seconds at 60fps

SplashView::SplashView()
{
}

void SplashView::setupScreen()
{
    SplashViewBase::setupScreen();
    holdActive = false;
    holdTicks  = 0;
}

void SplashView::tearDownScreen()
{
    SplashViewBase::tearDownScreen();
}

void SplashView::handleTickEvent()
{
    SplashViewBase::handleTickEvent();

    holdActive = true;

    if (++holdTicks >= HOLD_TICKS)
    {
        holdActive = false;
        application().gotoDefaultScreenNoTransition();
    }
}
