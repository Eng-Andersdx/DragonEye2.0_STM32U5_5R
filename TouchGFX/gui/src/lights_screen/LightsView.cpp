#include <gui/lights_screen/LightsView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <math.h>

int  g_lightsOn       = 3;
bool g_lrOn           = true;
bool g_kitchenOn      = true;
bool g_hallwayOn      = true;
bool g_conservatoryOn = false;

LightsView::LightsView()
{
}

void LightsView::setupScreen()
{
    LightsViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen

    // Restore states from saved globals
    lrOn           = g_lrOn;
    kitchenOn      = g_kitchenOn;
    hallwayOn      = g_hallwayOn;
    conservatoryOn = g_conservatoryOn;

    // Restore toggle button visual states
    lr_toggle.forceState(lrOn);
    kitchen_toggle.forceState(kitchenOn);
    hallway_toggle.forceState(hallwayOn);
    conservatory_toggle.forceState(conservatoryOn);

    // Apply initial widget states
    updateLightWidget(lrOn,           lr_light,           lr_slider,           lr_light_on);
    updateLightWidget(kitchenOn,      kitchen_light,      kitchen_slider,      kitchen_light_on);
    updateLightWidget(hallwayOn,      hallway_light,      hallway_slider,      hallway_light_on);
    updateLightWidget(conservatoryOn, conservatory_light, conservatory_slider, conservatory_light_on);

    // Set initial alphas based on slider values and toggle state
    lr_light_on.setAlpha(g_lrOn ?
        (uint8_t)((lr_slider.getValue() * 255) / 400) : 0);
    lr_light_on.invalidate();

    kitchen_light_on.setAlpha(g_kitchenOn ?
        (uint8_t)((kitchen_slider.getValue() * 255) / 400) : 0);
    kitchen_light_on.invalidate();

    hallway_light_on.setAlpha(g_hallwayOn ?
        (uint8_t)((hallway_slider.getValue() * 255) / 400) : 0);
    hallway_light_on.invalidate();

    conservatory_light_on.setAlpha(g_conservatoryOn ?
        (uint8_t)((conservatory_slider.getValue() * 255) / 400) : 0);
    conservatory_light_on.invalidate();

    star1CurrentScale = 1.0f;
    star1TargetScale  = 1.0f;
    star2CurrentScale = 0.6f;
    star2TargetScale  = 0.6f;

    star_1.setScale(1.0f);
    star_1.invalidate();
    star_2.setScale(0.6f);
    star_2.invalidate();

    sunBeamsCurrentAngle = 0.0f;
    sunBeamsTargetAngle  = 0.0f;
    sunBodyCurrentScale  = 1.2f;
    sunBodyTargetScale   = 1.2f;
    sunBodyShrinking     = false;

    sun_beams.updateZAngle(0.0f);
    sun_beams.setScale(1.2f);
    sun_beams.invalidate();
    sun_body.setScale(1.2f);
    sun_body.invalidate();
}

void LightsView::tearDownScreen()
{
    LightsViewBase::tearDownScreen();
}

void LightsView::onLRToggleClicked()
{
    lrOn = !lrOn;
    g_lrOn = lrOn;
    updateLightWidget(lrOn, lr_light, lr_slider, lr_light_on);
    updateLightsCount();
}

void LightsView::onKitchenToggleClicked()
{
    kitchenOn = !kitchenOn;
    g_kitchenOn = kitchenOn;
    updateLightWidget(kitchenOn, kitchen_light, kitchen_slider, kitchen_light_on);
    updateLightsCount();
}

void LightsView::onHallwayToggleClicked()
{
    hallwayOn = !hallwayOn;
    g_hallwayOn = hallwayOn;
    updateLightWidget(hallwayOn, hallway_light, hallway_slider, hallway_light_on);
    updateLightsCount();
}

void LightsView::onConservatoryToggleClicked()
{
    conservatoryOn = !conservatoryOn;
    g_conservatoryOn = conservatoryOn;
    updateLightWidget(conservatoryOn, conservatory_light, conservatory_slider, conservatory_light_on);
    updateLightsCount();
}

void LightsView::onBedtimeClicked()
{
    // Star animation
    if (star1TargetScale == 1.0f)
    {
        star1TargetScale = 0.6f;
        star2TargetScale = 1.0f;
    }
    else
    {
        star1TargetScale = 1.0f;
        star2TargetScale = 0.6f;
    }

    // Turn all lights off
    lrOn           = false;
    kitchenOn      = false;
    hallwayOn      = false;
    conservatoryOn = false;
    g_lrOn           = false;
    g_kitchenOn      = false;
    g_hallwayOn      = false;
    g_conservatoryOn = false;

    // Force all_on_button toggle off
    all_on_button.forceState(false);
    all_on_button.invalidate();

    // Update toggle buttons visually
    lr_toggle.forceState(false);
    lr_toggle.invalidate();
    kitchen_toggle.forceState(false);
    kitchen_toggle.invalidate();
    hallway_toggle.forceState(false);
    hallway_toggle.invalidate();
    conservatory_toggle.forceState(false);
    conservatory_toggle.invalidate();

    // Update all light widgets
    updateLightWidget(false, lr_light,           lr_slider,           lr_light_on);
    updateLightWidget(false, kitchen_light,      kitchen_slider,      kitchen_light_on);
    updateLightWidget(false, hallway_light,      hallway_slider,      hallway_light_on);
    updateLightWidget(false, conservatory_light, conservatory_slider, conservatory_light_on);

    updateLightsCount();
}

void LightsView::onAllbuttonClicked()
{
    // Sun animation
    sunBeamsTargetAngle += 6.25f;
    sunBodyTargetScale   = 1.0f;
    sunBodyShrinking     = true;

    // Turn all lights on
    lrOn           = true;
    kitchenOn      = true;
    hallwayOn      = true;
    conservatoryOn = true;
    g_lrOn           = true;
    g_kitchenOn      = true;
    g_hallwayOn      = true;
    g_conservatoryOn = true;

    // Force bedtime toggle off
    bedtime.forceState(false);
    bedtime.invalidate();

    // Update toggle buttons visually
    lr_toggle.forceState(true);
    lr_toggle.invalidate();
    kitchen_toggle.forceState(true);
    kitchen_toggle.invalidate();
    hallway_toggle.forceState(true);
    hallway_toggle.invalidate();
    conservatory_toggle.forceState(true);
    conservatory_toggle.invalidate();

    // Update all light widgets
    updateLightWidget(true, lr_light,           lr_slider,           lr_light_on);
    updateLightWidget(true, kitchen_light,      kitchen_slider,      kitchen_light_on);
    updateLightWidget(true, hallway_light,      hallway_slider,      hallway_light_on);
    updateLightWidget(true, conservatory_light, conservatory_slider, conservatory_light_on);

    updateLightsCount();
}

void LightsView::handleTickEvent()
{
    LightsViewBase::handleTickEvent();

    if (fabsf(star1CurrentScale - star1TargetScale) > 0.005f)
    {
        star1CurrentScale += (star1TargetScale - star1CurrentScale) * 0.12f;
        star_1.setScale(star1CurrentScale);
        star_1.invalidate();
    }

    if (fabsf(star2CurrentScale - star2TargetScale) > 0.005f)
    {
        star2CurrentScale += (star2TargetScale - star2CurrentScale) * 0.12f;
        star_2.setScale(star2CurrentScale);
        star_2.invalidate();
    }

    // Ease sun_beams rotation
    if (fabsf(sunBeamsCurrentAngle - sunBeamsTargetAngle) > 0.01f)
    {
        sunBeamsCurrentAngle += (sunBeamsTargetAngle - sunBeamsCurrentAngle) * 0.15f;
        sun_beams.updateZAngle(sunBeamsCurrentAngle);
        sun_beams.invalidate();
    }

    // Sun body pulse — shrink to 1.0 then return to 1.2
    if (fabsf(sunBodyCurrentScale - sunBodyTargetScale) > 0.005f)
    {
        sunBodyCurrentScale += (sunBodyTargetScale - sunBodyCurrentScale) * 0.15f;
        sun_body.setScale(sunBodyCurrentScale);
        sun_body.invalidate();

        if (sunBodyShrinking &&
            fabsf(sunBodyCurrentScale - 1.0f) < 0.02f)
        {
            sunBodyShrinking   = false;
            sunBodyTargetScale = 1.2f;
        }
    }
}

void LightsView::updateLightWidget(bool state,
    touchgfx::Image& lightImg,
    touchgfx::Slider& slider,
    touchgfx::Image& lightOn)
{
    if (state)
    {
        lightImg.setBitmap(touchgfx::Bitmap(BITMAP_LIGHT_OFF_SMALL_ID));
        slider.setBitmaps(
            touchgfx::Bitmap(BITMAP_LIGHT_BAR_BG_2_ID),    // background (right of indicator)
            touchgfx::Bitmap(BITMAP_LIGHT_BAR_MAIN_2_ID),  // filled (left of indicator)
            touchgfx::Bitmap(BITMAP_LIGHT_BAR_SLIDER_ID)); // keep existing indicator
        // Restore alpha from slider value
        uint8_t alpha = (uint8_t)((slider.getValue() * 255) / 400);
        lightOn.setAlpha(alpha);
    }
    else
    {
        lightImg.setBitmap(touchgfx::Bitmap(BITMAP_LIGHT_OFF_SMALL_ID));
        slider.setBitmaps(
            touchgfx::Bitmap(BITMAP_LIGHT_BAR_BG_2_ID),    // background (right of indicator)
            touchgfx::Bitmap(BITMAP_LIGHT_LINE_OFF_2_ID),  // filled (left of indicator)
            touchgfx::Bitmap(BITMAP_LIGHT_BAR_SLIDER_ID)); // keep existing indicator
        lightOn.setAlpha(0);
    }
    lightImg.invalidate();
    lightOn.invalidate();
    slider.invalidate();
}

void LightsView::updateLightAlpha(int sliderValue,
                                   touchgfx::Image& lightOn)
{
    uint8_t alpha = (uint8_t)((sliderValue * 255) / 400);
    lightOn.setAlpha(alpha);
    lightOn.invalidate();
}

void LightsView::onLRSliderChanged(int value)
{
    if (g_lrOn)
        updateLightAlpha(value, lr_light_on);
}

void LightsView::onKitchenSliderChanged(int value)
{
    if (g_kitchenOn)
        updateLightAlpha(value, kitchen_light_on);
}

void LightsView::onHallwaySliderChanged(int value)
{
    if (g_hallwayOn)
        updateLightAlpha(value, hallway_light_on);
}

void LightsView::onConservatorySliderChanged(int value)
{
    if (g_conservatoryOn)
        updateLightAlpha(value, conservatory_light_on);
}

void LightsView::updateLightsCount()
{
    g_lightsOn = (lrOn ? 1 : 0) +
                 (kitchenOn ? 1 : 0) +
                 (hallwayOn ? 1 : 0) +
                 (conservatoryOn ? 1 : 0);
}
