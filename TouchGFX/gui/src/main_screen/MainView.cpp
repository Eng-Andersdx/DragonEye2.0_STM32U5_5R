#include <gui/main_screen/MainView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <touchgfx/Color.hpp>

static const char* trackNames[]  = { "The Chain", "Money", "May Ninth" };
static const char* artistNames[] = { "Fleetwood Mac", "Pink Floyd", "Khruangbin" };
static const touchgfx::BitmapId albumCovers[] =
{
    BITMAP_ALBUM_1_ID,
    BITMAP_ALBUM_2_ID,
    BITMAP_ALBUM_3_ID
};

MainView::MainView()
{
}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen
    updateMusicCard();

    // Sync music animation on main screen
    if (g_isPlaying)
    {
        music_off_main.startFadeAnimation(0, 1);
        music_ani_main.startFadeAnimation(255, 1);
        music_ani_main.startAnimation(false, true, true);
    }
    else
    {
        music_ani_main.startFadeAnimation(0, 1);
        music_ani_main.stopAnimation();
        music_off_main.startFadeAnimation(255, 1);
    }

    // Sync play_pause_main button visual state
    play_pause_main.forceState(g_isPlaying);
    play_pause_main.invalidate();

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
    sunBodyCurrentScale  = 1.0f;
    sunBodyTargetScale   = 1.0f;
    sunBodyShrinking     = false;

    sun_beams.updateZAngle(0.0f);
    sun_beams.invalidate();
    sun_body.setScale(1.0f);
    sun_body.invalidate();

    // Update climate card
    static touchgfx::Unicode::UnicodeChar climateTempBuf[6];
    touchgfx::Unicode::snprintf(climateTempBuf, 6, "%d", g_climateTemp);
    climate_temp.setWildcard(climateTempBuf);
    climate_temp.invalidate();

    static touchgfx::Unicode::UnicodeChar climateStatusBuf[20];
    if (g_climateTemp <= 21)
    {
        touchgfx::Unicode::snprintf(climateStatusBuf, 20, "Cooling to %d%c", g_climateTemp, 0xB0);
    }
    else
    {
        touchgfx::Unicode::snprintf(climateStatusBuf, 20, "Heating to %d%c", g_climateTemp, 0xB0);
    }
    climate_h3.setWildcard(climateStatusBuf);

    if (g_climateTemp <= 21)
    {
        climate_h3.setColor(touchgfx::Color::getColorFromRGB(0x4A, 0x9E, 0xFF));
    }
    else
    {
        climate_h3.setColor(touchgfx::Color::getColorFromRGB(0xEB, 0x3A, 0x13));
    }
    climate_h3.invalidate();

    // Map temperature to circle range
    int mapped = ((g_climateTemp - 15) * 100) / (28 - 15);
    if (mapped > 100) mapped = 100;
    climate_wheel_2.setValue(mapped);
    climate_wheel_2.invalidate();

    updateClimateDot();
    updateClimateWheel();

    climate_h3.setColor(touchgfx::Color::getColorFromRGB(mainWheelR, mainWheelG, mainWheelB));
    climate_h3.invalidate();

    // Update lights count
    static touchgfx::Unicode::UnicodeChar lightsBuf[2];
    touchgfx::Unicode::snprintf(lightsBuf, 2, "%d", g_lightsOn);
    lights_count.setWildcard(lightsBuf);
    lights_count.invalidate();

    if (g_lightsOn == 0)
    {
        light_bulb_main.setBitmap(
            touchgfx::Bitmap(BITMAP_LIGHT_OFF_BIG_ID));
    }
    else
    {
        light_bulb_main.setBitmap(
            touchgfx::Bitmap(BITMAP_LIGHT_ON_BIG_ID));
    }
    light_bulb_main.invalidate();

    securityFadePending  = false;

    // Restore security status text
    if (g_frontDoorLocked)
    {
        Unicode::strncpy(securityBuf, "Locked", 10);
        security_status.setColor(
            touchgfx::Color::getColorFromRGB(0x70, 0xAC, 0xF4));
    }
    else
    {
        Unicode::strncpy(securityBuf, "Unlocked", 10);
        security_status.setColor(
            touchgfx::Color::getColorFromRGB(0xFF, 0xFF, 0xFF));
    }
    security_status.setWildcard(securityBuf);
    security_status.setAlpha(255);
    security_status.invalidate();

    if (g_frontDoorLocked)
    {
        padlock_main.setBitmap(
            touchgfx::Bitmap(BITMAP_LOCKED_SMALL_ID));
    }
    else
    {
        padlock_main.setBitmap(
            touchgfx::Bitmap(BITMAP_WHITE_UNLOCKED_MEDIUM_ID));
    }
    padlock_main.invalidate();
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::onSkipMainClicked()
{
    g_currentTrack = (g_currentTrack + 1) % 3;
    updateMusicCard();
}

void MainView::onBackMainClicked()
{
    g_currentTrack = (g_currentTrack - 1 + 3) % 3;
    updateMusicCard();
}

void MainView::onPlayPauseMainClicked()
{
    g_isPlaying = !g_isPlaying;

    play_pause_main.forceState(g_isPlaying);
    play_pause_main.invalidate();

    if (g_isPlaying)
    {
        music_off_main.startFadeAnimation(0, 1);
        music_ani_main.startFadeAnimation(255, 1);
        music_ani_main.startAnimation(false, true, true);
    }
    else
    {
        music_ani_main.startFadeAnimation(0, 8);
        music_ani_main.stopAnimation();
        music_off_main.startFadeAnimation(255, 8);
    }
}

void MainView::onBedtimeClicked()
{
    // Force wakeup off
    wakeup.forceState(false);
    wakeup.invalidate();

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
    g_lrOn           = false;
    g_kitchenOn      = false;
    g_hallwayOn      = false;
    g_conservatoryOn = false;
    g_lightsOn       = 0;

    // Update lights count
    static Unicode::UnicodeChar lightsBuf[2];
    Unicode::snprintf(lightsBuf, 2, "%d", g_lightsOn);
    lights_count.setWildcard(lightsBuf);
    lights_count.invalidate();

    // Update light bulb image
    light_bulb_main.setBitmap(
        touchgfx::Bitmap(BITMAP_LIGHT_OFF_BIG_ID));
    light_bulb_main.invalidate();

    // Lock front door
    g_frontDoorLocked = true;
    updateSecurityStatus();
    triggerMainLockAnimation(false);

    padlock_main.invalidate();
    padlock_main.setBitmap(
        touchgfx::Bitmap(BITMAP_LOCKED_SMALL_ID));
    padlock_main.invalidate();
}

void MainView::onWakeupClicked()
{
    // Force bedtime off
    bedtime.forceState(false);
    bedtime.invalidate();

    // Reset star scales
    star1TargetScale = 1.0f;
    star2TargetScale = 0.6f;

    // Turn all lights on
    g_lrOn           = true;
    g_kitchenOn      = true;
    g_hallwayOn      = true;
    g_conservatoryOn = true;
    g_lightsOn       = 4;

    // Update lights count
    static Unicode::UnicodeChar lightsBuf[2];
    Unicode::snprintf(lightsBuf, 2, "%d", g_lightsOn);
    lights_count.setWildcard(lightsBuf);
    lights_count.invalidate();

    // Update light bulb image
    light_bulb_main.setBitmap(
        touchgfx::Bitmap(BITMAP_LIGHT_ON_BIG_ID));
    light_bulb_main.invalidate();

    // Unlock front door
    g_frontDoorLocked = false;
    updateSecurityStatus();
    triggerMainLockAnimation(true);

    padlock_main.setBitmap(
        touchgfx::Bitmap(BITMAP_WHITE_UNLOCKED_MEDIUM_ID));
    padlock_main.invalidate();

    // Sun animation
    sunBeamsTargetAngle += 6.25f;
    sunBodyTargetScale   = 0.8f;
    sunBodyShrinking     = true;
}

void MainView::triggerMainLockAnimation(bool unlocking)
{
    // lock_lock_white / lock_body_white no longer exist — animation removed.
}

void MainView::updateSecurityStatus()
{
    if (g_frontDoorLocked)
    {
        Unicode::strncpy(securityPendingBuf, "Locked", 10);
        pendingSecurityColour =
            touchgfx::Color::getColorFromRGB(0x70, 0xAC, 0xF4);
    }
    else
    {
        Unicode::strncpy(securityPendingBuf, "Unlocked", 10);
        pendingSecurityColour =
            touchgfx::Color::getColorFromRGB(0xFF, 0xFF, 0xFF);
    }
    securityFadePending = true;
    security_status.startFadeAnimation(0, 10);
}

void MainView::handleTickEvent()
{
    MainViewBase::handleTickEvent();

    // Ease star_1 scale
    if (fabsf(star1CurrentScale - star1TargetScale) > 0.005f)
    {
        star1CurrentScale += (star1TargetScale - star1CurrentScale) * 0.12f;
        star_1.setScale(star1CurrentScale);
        star_1.invalidate();
    }

    // Ease star_2 scale
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

    // Sun body pulse — shrink then return to 1.0
    if (fabsf(sunBodyCurrentScale - sunBodyTargetScale) > 0.005f)
    {
        sunBodyCurrentScale += (sunBodyTargetScale - sunBodyCurrentScale) * 0.15f;
        sun_body.setScale(sunBodyCurrentScale);
        sun_body.invalidate();

        // Once shrunk, start returning to 1.0
        if (sunBodyShrinking &&
            fabsf(sunBodyCurrentScale - 0.8f) < 0.02f)
        {
            sunBodyShrinking   = false;
            sunBodyTargetScale = 1.0f;
        }
    }

    // Security status text fade
    if (securityFadePending && security_status.getAlpha() == 0)
    {
        securityFadePending = false;
        Unicode::strncpy(securityBuf, securityPendingBuf, 10);
        security_status.setColor(pendingSecurityColour);
        security_status.setWildcard(securityBuf);
        security_status.invalidate();
        security_status.startFadeAnimation(255, 10);
    }
}

void MainView::updateClimateDot()
{
    climate_dot_main.invalidate();

    float pct   = (float)(g_climateTemp - 15) / (float)(28 - 15);
    float angle = (MAIN_ARC_START_DEG - 90.0f + pct *
                  (MAIN_ARC_END_DEG - MAIN_ARC_START_DEG)) * MAIN_DEG_TO_RAD;

    int dotX = (int)(126.0f + MAIN_DOT_RADIUS * cosf(angle))
               - climate_dot_main.getWidth()  / 2;
    int dotY = (int)(145.0f  + MAIN_DOT_RADIUS * sinf(angle))
               - climate_dot_main.getHeight() / 2;

    climate_dot_main.setXY(dotX, dotY);
    climate_dot_main.invalidate();
}

void MainView::updateClimateWheel()
{
    if (g_climateTemp <= 18)
    {
        mainWheelR = 0x4A; mainWheelG = 0x96; mainWheelB = 0xEC;
    }
    else if (g_climateTemp <= 20)
    {
        mainWheelR = 0x6C; mainWheelG = 0xAE; mainWheelB = 0xF1;
    }
    else if (g_climateTemp <= 22)
    {
        mainWheelR = 0x9B; mainWheelG = 0xCE; mainWheelB = 0xF9;
    }
    else if (g_climateTemp <= 24)
    {
        mainWheelR = 0xD1; mainWheelG = 0xEE; mainWheelB = 0xF9;
    }
    else if (g_climateTemp <= 26)
    {
        mainWheelR = 0xFF; mainWheelG = 0xC8; mainWheelB = 0x73;
    }
    else if (g_climateTemp == 27)
    {
        mainWheelR = 0xF0; mainWheelG = 0x64; mainWheelB = 0x41;
    }
    else
    {
        mainWheelR = 0xE6; mainWheelG = 0x1E; mainWheelB = 0x1E;
    }

    climate_wheel_2Painter.setColor(
        touchgfx::Color::getColorFromRGB(mainWheelR, mainWheelG, mainWheelB));
    climate_wheel_2.invalidate();
}

void MainView::updateMusicCard()
{
    int idx = g_currentTrack;

    // Update album cover
    album_cover_small.setBitmap(touchgfx::Bitmap(albumCovers[idx]));
    album_cover_small.invalidate();

    // Update track name
    static touchgfx::Unicode::UnicodeChar trackBuf[32];
    touchgfx::Unicode::strncpy(trackBuf, trackNames[idx], 32);
    music_track_main.setWildcard(trackBuf);
    music_track_main.invalidate();

    // Update artist name
    static touchgfx::Unicode::UnicodeChar artistBuf[32];
    touchgfx::Unicode::strncpy(artistBuf, artistNames[idx], 32);
    music_artist_main.setWildcard(artistBuf);
    music_artist_main.invalidate();
}
