#include <gui/default_screen/DefaultView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <touchgfx/EasingEquations.hpp>

const uint16_t DefaultView::HINT_DELAY_TICKS = 90; // 1.5 seconds at 60fps
const uint16_t DefaultView::CHEVRON_BOUNCE_INTERVAL = 120; // 2 seconds at 60fps

DefaultView::DefaultView()
{
}

void DefaultView::setupScreen()
{
    DefaultViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen

    cloudMoving  = false;
    cloud2Moving = false;
    hintShown    = false;
    hintTicks    = 0;

    chevronTicks    = 0;
    chevronBouncing = false;
    chevron_up.setMoveAnimationDelay(0);

    chevron_up.setAlpha(0);
    chevron_up.invalidate();
    swipe_up.setAlpha(0);
    swipe_up.invalidate();
}

void DefaultView::tearDownScreen()
{
    DefaultViewBase::tearDownScreen();
}

void DefaultView::handleTickEvent()
{
    DefaultViewBase::handleTickEvent();

    if (!cloudMoving)
    {
        cloud.startMoveAnimation(450, 45, 2000,
            EasingEquations::linearEaseNone,
            EasingEquations::linearEaseNone);
        cloudMoving = true;
    }

    if (!cloud2Moving)
    {
        cloud_2.startMoveAnimation(540, 60, 1000,
            EasingEquations::linearEaseNone,
            EasingEquations::linearEaseNone);
        cloud2Moving = true;
    }

    if (!hintShown)
    {
        if (++hintTicks >= HINT_DELAY_TICKS)
        {
            hintShown = true;
            chevron_up.startFadeAnimation(255, 20);
            swipe_up.startFadeAnimation(255, 20);
        }
    }

    // Periodic bounce every 2 seconds once hint is shown
    if (hintShown)
    {
        if (++chevronTicks >= CHEVRON_BOUNCE_INTERVAL)
        {
            chevronTicks = 0;

            if (!chevronBouncing)
            {
                chevronBouncing = true;
                int startY = chevron_up.getY();

                // Bounce up 8px then return
                chevron_up.startMoveAnimation(
                    chevron_up.getX(), startY - 8, 20,
                    EasingEquations::cubicEaseOut,
                    EasingEquations::cubicEaseOut);
            }
        }

        // Return to original position after bounce up
        if (chevronBouncing && !chevron_up.isMoveAnimationRunning())
        {
            chevronBouncing = false;
            chevron_up.startMoveAnimation(
                chevron_up.getX(), chevron_up.getY() + 8, 20,
                EasingEquations::cubicEaseIn,
                EasingEquations::cubicEaseIn);
        }
    }
}
