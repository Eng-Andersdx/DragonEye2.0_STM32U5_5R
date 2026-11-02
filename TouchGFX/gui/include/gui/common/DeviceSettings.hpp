#ifndef DEVICESETTINGS_HPP
#define DEVICESETTINGS_HPP

#include <touchgfx/Drawable.hpp>

/*
 * Device settings shared by all screens (5" DragonEye).
 * Defined in SettingsView.cpp, changed on the Settings screen.
 */
extern int  g_brightness;   // display backlight, 10..100 %
extern bool g_statusLedOn;  // RGB status LED on the board
extern bool g_wifiOn;       // Wi-Fi icon / status (demo)
extern bool g_btOn;         // Bluetooth icon / status (demo)

/* Show or hide the Wi-Fi and Bluetooth icons in a screen's header bar */
inline void applyHeaderStatus(touchgfx::Drawable& wifiIcon, touchgfx::Drawable& btIcon)
{
    wifiIcon.setVisible(g_wifiOn);
    btIcon.setVisible(g_btOn);
    wifiIcon.invalidate();
    btIcon.invalidate();
}

#endif // DEVICESETTINGS_HPP
