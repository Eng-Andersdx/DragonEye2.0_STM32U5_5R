#include <gui/settings_screen/SettingsView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <touchgfx/Unicode.hpp>

#ifndef SIMULATOR
#include "dragoneye_leds.h"   // BSP_BL_SetIntensity, BSP_LED_SetColor (TouchGFX/target/DragonEye)
#endif

/* Shared device settings (declared in gui/common/DeviceSettings.hpp).
 * Defaults match main.c: backlight 100 %, status LED white. */
int  g_brightness  = 100;
bool g_statusLedOn = true;
bool g_wifiOn      = true;
bool g_btOn        = true;

static void hwSetBrightness(int percent)
{
#ifndef SIMULATOR
    BSP_BL_SetIntensity((uint8_t)percent);
#else
    (void)percent;
#endif
}

static void hwSetStatusLed(bool on)
{
#ifndef SIMULATOR
    BSP_LED_SetColor(on ? RGB_WHITE : 0);
#else
    (void)on;
#endif
}

SettingsView::SettingsView()
{

}

void SettingsView::setupScreen()
{
    SettingsViewBase::setupScreen();

    // Restore the controls from the saved settings
    bright_slider.setValue(g_brightness);
    led_toggle.forceState(g_statusLedOn);
    wifi_toggle.forceState(g_wifiOn);
    bt_toggle.forceState(g_btOn);

    updateBrightnessText();
    updateWifiText();
    updateBtText();
    applyHeaderStatus(wifi, bt);
}

void SettingsView::tearDownScreen()
{
    SettingsViewBase::tearDownScreen();
}

void SettingsView::onBrightnessChanged(int value)
{
    if (value < 10)  value = 10;     // never let the screen go fully dark
    if (value > 100) value = 100;
    g_brightness = value;
    hwSetBrightness(value);
    updateBrightnessText();
}

void SettingsView::onLedToggled()
{
    g_statusLedOn = led_toggle.getState();
    hwSetStatusLed(g_statusLedOn);
}

void SettingsView::onWifiToggled()
{
    g_wifiOn = wifi_toggle.getState();
    updateWifiText();
    applyHeaderStatus(wifi, bt);
}

void SettingsView::onBtToggled()
{
    g_btOn = bt_toggle.getState();
    updateBtText();
    applyHeaderStatus(wifi, bt);
}

void SettingsView::updateBrightnessText()
{
    Unicode::snprintf(bright_valBuffer, BRIGHT_VAL_SIZE, "%d", g_brightness);
    bright_val.invalidate();
}

void SettingsView::updateWifiText()
{
    Unicode::strncpy(wifi_statusBuffer, g_wifiOn ? "Connected - DragonEye" : "Off", WIFI_STATUS_SIZE);
    wifi_status.invalidate();
    wifi_icon.setAlpha(g_wifiOn ? 255 : 90);
    wifi_icon.invalidate();
}

void SettingsView::updateBtText()
{
    Unicode::strncpy(bt_statusBuffer, g_btOn ? "On - discoverable" : "Off", BT_STATUS_SIZE);
    bt_status.invalidate();
    bt_icon.setAlpha(g_btOn ? 255 : 90);
    bt_icon.invalidate();
}
