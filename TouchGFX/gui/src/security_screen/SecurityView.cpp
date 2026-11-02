#include <gui/security_screen/SecurityView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <stdio.h>

bool g_frontDoorLocked = true;
bool g_backDoorLocked  = true;
bool g_garageLocked    = true;
bool g_alarmOn         = true;

const touchgfx::colortype SecurityView::COLOUR_LOCKED   =
    touchgfx::Color::getColorFromRGB(0x70, 0xAC, 0xF4);
const touchgfx::colortype SecurityView::COLOUR_UNLOCKED =
    touchgfx::Color::getColorFromRGB(0xFF, 0xFF, 0xFF);

SecurityView::SecurityView()
{
}

void SecurityView::setupScreen()
{
    SecurityViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen

    front_door_toggle.forceState(g_frontDoorLocked);
    back_door_toggle.forceState(g_backDoorLocked);
    garage_toggle.forceState(g_garageLocked);
    alarm_toggle.forceState(g_alarmOn);

    frontDoorFadePending = false;
    backDoorFadePending  = false;
    garageFadePending    = false;
    alarmFadePending     = false;

    // Set backgrounds instantly based on saved state
    if (g_frontDoorLocked)
    {
        s_blue_bg.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_BG_ID));
        s_blue_bg_back.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_DARK_ID));
    }
    else
    {
        s_blue_bg.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_DARK_ID));
        s_blue_bg_back.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_BG_ID));
    }
    s_blue_bg.setAlpha(255);
    s_blue_bg_back.setAlpha(0);
    s_blue_bg.invalidate();
    s_blue_bg_back.invalidate();

    // Set text instantly — no fade on load
    front_door_status.setColor(g_frontDoorLocked ? COLOUR_LOCKED : COLOUR_UNLOCKED);
    Unicode::strncpy(frontDoorBuf, g_frontDoorLocked ? "Locked" : "Unlocked", 10);
    front_door_status.setWildcard(frontDoorBuf);
    front_door_status.setAlpha(255);
    front_door_status.invalidate();

    back_door_status.setColor(g_backDoorLocked ? COLOUR_LOCKED : COLOUR_UNLOCKED);
    Unicode::strncpy(backDoorBuf, g_backDoorLocked ? "Locked" : "Unlocked", 10);
    back_door_status.setWildcard(backDoorBuf);
    back_door_status.setAlpha(255);
    back_door_status.invalidate();

    garage_status.setColor(g_garageLocked ? COLOUR_LOCKED : COLOUR_UNLOCKED);
    Unicode::strncpy(garageBuf, g_garageLocked ? "Locked" : "Unlocked", 10);
    garage_status.setWildcard(garageBuf);
    garage_status.setAlpha(255);
    garage_status.invalidate();

    alarm_status.setColor(g_alarmOn ? COLOUR_LOCKED : COLOUR_UNLOCKED);
    Unicode::strncpy(alarmBuf, g_alarmOn ? "Online" : "Offline", 10);
    alarm_status.setWildcard(alarmBuf);
    alarm_status.setAlpha(255);
    alarm_status.invalidate();

    if (g_frontDoorLocked)
    {
        blue_lock.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_LOCK_ID));
        blue_body.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_BODY_ID));
        blue_lock.updateYAngle(0.0f);
        blue_lock.setXY(59, 72);
        blue_body.setXY(52, 102);
    }
    else
    {
        blue_lock.setBitmap(touchgfx::Bitmap(BITMAP_BIG_WHITE_LOCK_ID));
        blue_body.setBitmap(touchgfx::Bitmap(BITMAP_BIG_WHITE_BODY_ID));
        blue_lock.updateYAngle(3.0f);
        blue_lock.setXY(78, 66);
        blue_body.setXY(40, 107);
    }
    blue_lock.invalidate();
    blue_body.invalidate();

    printf("setupScreen: blue_body XY = %d %d\n",
           blue_body.getX(), blue_body.getY());

    // Force invalidate parent container to clear any ghost positions
    invalidate();

    // Reset animation state fully
    lockAnimState     = LOCK_IDLE;
    lockAnimTick      = 0;
    bitmapSwapped     = false;
    lockAnimUnlocking = false;

    lockAnimState        = LOCK_IDLE;
    lockAnimTick         = 0;
    bitmapSwapped        = false;
    lockAnimUnlocking    = false;
}

void SecurityView::tearDownScreen()
{
    // Stop any in-flight animation and snap to correct position
    lockAnimState = LOCK_IDLE;

    if (g_frontDoorLocked)
    {
        blue_lock.setXY(59, 72);
        blue_body.setXY(52, 102);
        blue_lock.updateYAngle(0.0f);
    }
    else
    {
        blue_lock.setXY(78, 66);
        blue_body.setXY(40, 107);
        blue_lock.updateYAngle(3.0f);
    }
    blue_lock.invalidate();
    blue_body.invalidate();

    SecurityViewBase::tearDownScreen();
}

void SecurityView::onFrontDoorToggleClicked()
{
    lockAnimUnlocking = g_frontDoorLocked; // true = about to unlock
    lockAnimState     = lockAnimUnlocking ? LOCK_ANIM_UP : LOCK_ANIM_FLIP;
    lockAnimTick      = 0;
    bitmapSwapped     = false;

    g_frontDoorLocked = !g_frontDoorLocked;
    updateFrontDoor();
}

void SecurityView::onBackDoorToggleClicked()
{
    g_backDoorLocked = !g_backDoorLocked;
    updateBackDoor();
}

void SecurityView::onGarageToggleClicked()
{
    g_garageLocked = !g_garageLocked;
    updateGarage();
}

void SecurityView::onAlarmToggleClicked()
{
    g_alarmOn = !g_alarmOn;
    updateAlarm();
}

void SecurityView::onLockAllToggleClicked()
{
    printf("onLockAllToggleClicked called, button state: %d, g_frontDoorLocked: %d\n",
           (int)lock_all_button.getState(),
           (int)g_frontDoorLocked);

    // Determine direction based on button press state
    bool unlocking = lock_all_button.getState();

    // Toggle all locks
    g_frontDoorLocked = !unlocking;
    g_backDoorLocked  = !unlocking;
    g_garageLocked    = !unlocking;
    g_alarmOn         = !unlocking;

    // Update all toggle buttons visually
    front_door_toggle.forceState(g_frontDoorLocked);
    front_door_toggle.invalidate();
    back_door_toggle.forceState(g_backDoorLocked);
    back_door_toggle.invalidate();
    garage_toggle.forceState(g_garageLocked);
    garage_toggle.invalidate();
    alarm_toggle.forceState(g_alarmOn);
    alarm_toggle.invalidate();

    // Force full screen redraw
    invalidate();

    // Trigger blue lock animation
    lockAnimUnlocking = unlocking;
    lockAnimState     = lockAnimUnlocking ? LOCK_ANIM_UP : LOCK_ANIM_FLIP;
    lockAnimTick      = 0;
    bitmapSwapped     = false;

    // Update all text areas
    updateFrontDoor();
    updateBackDoor();
    updateGarage();
    updateAlarm();
}

void SecurityView::updateFrontDoor()
{
    if (g_frontDoorLocked)
    {
        Unicode::strncpy(frontDoorPendingBuf, "Locked", 10);
        pendingFrontDoorColour = COLOUR_LOCKED;

        // Place new image on back layer first
        s_blue_bg_back.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_BG_ID));
        s_blue_bg_back.setAlpha(0);
        s_blue_bg_back.invalidate();

        // Fade back layer in and front layer out simultaneously
        s_blue_bg_back.startFadeAnimation(255, 25);
        s_blue_bg.startFadeAnimation(0, 25);
    }
    else
    {
        Unicode::strncpy(frontDoorPendingBuf, "Unlocked", 10);
        pendingFrontDoorColour = COLOUR_UNLOCKED;

        s_blue_bg_back.setBitmap(touchgfx::Bitmap(BITMAP_SECURITY_MAIN_DARK_ID));
        s_blue_bg_back.setAlpha(0);
        s_blue_bg_back.invalidate();

        s_blue_bg_back.startFadeAnimation(255, 25);
        s_blue_bg.startFadeAnimation(0, 25);
    }

    frontDoorFadePending = true;
    front_door_status.startFadeAnimation(0, 15);
}

void SecurityView::updateBackDoor()
{
    if (g_backDoorLocked)
    {
        Unicode::strncpy(backDoorPendingBuf, "Locked", 10);
        pendingBackDoorColour = COLOUR_LOCKED;
    }
    else
    {
        Unicode::strncpy(backDoorPendingBuf, "Unlocked", 10);
        pendingBackDoorColour = COLOUR_UNLOCKED;
    }
    backDoorFadePending = true;
    back_door_status.startFadeAnimation(0, 15);
}

void SecurityView::updateGarage()
{
    if (g_garageLocked)
    {
        Unicode::strncpy(garagePendingBuf, "Locked", 10);
        pendingGarageColour = COLOUR_LOCKED;
    }
    else
    {
        Unicode::strncpy(garagePendingBuf, "Unlocked", 10);
        pendingGarageColour = COLOUR_UNLOCKED;
    }
    garageFadePending = true;
    garage_status.startFadeAnimation(0, 15);
}

void SecurityView::updateAlarm()
{
    if (g_alarmOn)
    {
        Unicode::strncpy(alarmPendingBuf, "Online", 10);
        pendingAlarmColour = COLOUR_LOCKED;
    }
    else
    {
        Unicode::strncpy(alarmPendingBuf, "Offline", 10);
        pendingAlarmColour = COLOUR_UNLOCKED;
    }
    alarmFadePending = true;
    alarm_status.startFadeAnimation(0, 15);
}

void SecurityView::updateLockAnimation()
{
    float t;

    switch (lockAnimState)
    {
    case LOCK_ANIM_UP:
    {
        // Move blue_lock up from Y:78 to Y:66
        t = (float)lockAnimTick / (float)LOCK_UP_TICKS;
        if (t > 1.0f) t = 1.0f;
        float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        int newY = lockAnimUnlocking
            ? (int)(72.0f + eased * (63.0f - 72.0f))
            : (int)(63.0f + eased * (72.0f - 63.0f));

        blue_lock.invalidate();
        blue_lock.setXY(blue_lock.getX(), newY);
        blue_lock.invalidate();

        lockAnimTick++;
        if (lockAnimTick >= LOCK_UP_TICKS)
        {
            lockAnimTick  = 0;
            lockAnimState = LOCK_ANIM_FLIP;
        }
        break;
    }

    case LOCK_ANIM_FLIP:
    {
        // Flip blue_lock on Y axis and move both to X:78
        t = (float)lockAnimTick / (float)LOCK_FLIP_TICKS;
        if (t > 1.0f) t = 1.0f;
        float eased = t < 0.5f
            ? 4.0f * t * t * t
            : 1.0f - (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) / 2.0f;

        float yAngle = lockAnimUnlocking
            ? eased * 3.0f
            : 3.0f - eased * 3.0f;

        int lockX = lockAnimUnlocking
            ? (int)(59.0f + eased * (78.0f - 59.0f))
            : (int)(78.0f + eased * (59.0f - 78.0f));

        int bodyX = lockAnimUnlocking
            ? (int)(52.0f + eased * (40.0f - 52.0f))
            : (int)(40.0f + eased * (52.0f - 40.0f));

        // Swap bitmaps at midpoint of flip
        if (!bitmapSwapped && lockAnimTick >= LOCK_FLIP_TICKS / 2)
        {
            bitmapSwapped = true;
            if (lockAnimUnlocking)
            {
                blue_lock.setBitmap(touchgfx::Bitmap(BITMAP_BIG_WHITE_LOCK_ID));
                blue_body.setBitmap(touchgfx::Bitmap(BITMAP_BIG_WHITE_BODY_ID));
            }
            else
            {
                blue_lock.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_LOCK_ID));
                blue_body.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_BODY_ID));
            }
        }

        blue_lock.invalidate();
        blue_lock.updateYAngle(yAngle);
        blue_lock.setXY(lockX, blue_lock.getY());
        blue_lock.invalidate();

        blue_body.invalidate();
        blue_body.setXY(bodyX, blue_body.getY());
        blue_body.invalidate();

        lockAnimTick++;
        if (lockAnimTick >= LOCK_FLIP_TICKS)
        {
            lockAnimTick = 0;
            if (lockAnimUnlocking)
                lockAnimState = LOCK_IDLE;
            else
                lockAnimState = LOCK_ANIM_DOWN;
        }
        break;
    }

    case LOCK_ANIM_DOWN:
    {
        t = (float)lockAnimTick / (float)LOCK_UP_TICKS;
        if (t > 1.0f) t = 1.0f;
        float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        int newY = (int)(63.0f + eased * (72.0f - 63.0f));

        blue_lock.invalidate();
        blue_lock.setXY(blue_lock.getX(), newY);
        blue_lock.invalidate();

        lockAnimTick++;
        if (lockAnimTick >= LOCK_UP_TICKS)
        {
            lockAnimState = LOCK_IDLE;
        }
        break;
    }

    default:
        break;
    }
}

void SecurityView::handleTickEvent()
{
    SecurityViewBase::handleTickEvent();

    if (lockAnimState != LOCK_IDLE)
    {
        printf("lockAnimState: %d tick: %d\n",
               (int)lockAnimState, lockAnimTick);
    }

    if (lockAnimState != LOCK_IDLE)
        updateLockAnimation();

    // Front door text
    if (frontDoorFadePending && front_door_status.getAlpha() == 0)
    {
        frontDoorFadePending = false;
        Unicode::strncpy(frontDoorBuf, frontDoorPendingBuf, 10);
        front_door_status.setColor(pendingFrontDoorColour);
        front_door_status.setWildcard(frontDoorBuf);
        front_door_status.invalidate();
        front_door_status.startFadeAnimation(255, 15);
    }

    // Back door text
    if (backDoorFadePending && back_door_status.getAlpha() == 0)
    {
        backDoorFadePending = false;
        Unicode::strncpy(backDoorBuf, backDoorPendingBuf, 10);
        back_door_status.setColor(pendingBackDoorColour);
        back_door_status.setWildcard(backDoorBuf);
        back_door_status.invalidate();
        back_door_status.startFadeAnimation(255, 15);
    }

    // Garage text
    if (garageFadePending && garage_status.getAlpha() == 0)
    {
        garageFadePending = false;
        Unicode::strncpy(garageBuf, garagePendingBuf, 10);
        garage_status.setColor(pendingGarageColour);
        garage_status.setWildcard(garageBuf);
        garage_status.invalidate();
        garage_status.startFadeAnimation(255, 15);
    }

    // Alarm text
    if (alarmFadePending && alarm_status.getAlpha() == 0)
    {
        alarmFadePending = false;
        Unicode::strncpy(alarmBuf, alarmPendingBuf, 10);
        alarm_status.setColor(pendingAlarmColour);
        alarm_status.setWildcard(alarmBuf);
        alarm_status.invalidate();
        alarm_status.startFadeAnimation(255, 15);
    }

    // Layer swap after background crossfade
    if (!frontDoorFadePending && s_blue_bg.getAlpha() == 0
        && s_blue_bg_back.getAlpha() == 255)
    {
        touchgfx::BitmapId tmp = s_blue_bg_back.getBitmap();
        s_blue_bg.setBitmap(touchgfx::Bitmap(tmp));
        s_blue_bg.setAlpha(255);
        s_blue_bg.invalidate();
        s_blue_bg_back.setAlpha(0);
        s_blue_bg_back.invalidate();
    }
}
