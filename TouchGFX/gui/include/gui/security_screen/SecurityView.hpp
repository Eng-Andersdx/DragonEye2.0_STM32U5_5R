#ifndef SECURITYVIEW_HPP
#define SECURITYVIEW_HPP

#include <gui_generated/security_screen/SecurityViewBase.hpp>
#include <gui/security_screen/SecurityPresenter.hpp>
#include <BitmapDatabase.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/Color.hpp>

extern bool g_frontDoorLocked;
extern bool g_backDoorLocked;
extern bool g_garageLocked;
extern bool g_alarmOn;

class SecurityView : public SecurityViewBase
{
public:
    SecurityView();
    virtual ~SecurityView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    virtual void onFrontDoorToggleClicked();
    virtual void onBackDoorToggleClicked();
    virtual void onGarageToggleClicked();
    virtual void onAlarmToggleClicked();
    virtual void onLockAllToggleClicked();

protected:
    void updateFrontDoor();
    void updateBackDoor();
    void updateGarage();
    void updateAlarm();

    static const touchgfx::colortype COLOUR_LOCKED;
    static const touchgfx::colortype COLOUR_UNLOCKED;

    Unicode::UnicodeChar frontDoorBuf[10];
    Unicode::UnicodeChar frontDoorPendingBuf[10];
    Unicode::UnicodeChar backDoorBuf[10];
    Unicode::UnicodeChar backDoorPendingBuf[10];
    Unicode::UnicodeChar garageBuf[10];
    Unicode::UnicodeChar garagePendingBuf[10];
    Unicode::UnicodeChar alarmBuf[10];
    Unicode::UnicodeChar alarmPendingBuf[10];

    bool frontDoorFadePending;
    bool backDoorFadePending;
    bool garageFadePending;
    bool alarmFadePending;

    touchgfx::colortype pendingFrontDoorColour;
    touchgfx::colortype pendingBackDoorColour;
    touchgfx::colortype pendingGarageColour;
    touchgfx::colortype pendingAlarmColour;

    enum LockAnimState
    {
        LOCK_IDLE,
        LOCK_ANIM_UP,
        LOCK_ANIM_FLIP,
        LOCK_ANIM_DOWN,
        LOCK_ANIM_UNFLIP
    };

    LockAnimState lockAnimState;
    bool lockAnimUnlocking;
    int  lockAnimTick;
    bool bitmapSwapped;

    static const int LOCK_UP_TICKS   = 20;
    static const int LOCK_FLIP_TICKS = 25;

    void updateLockAnimation();
};

#endif // SECURITYVIEW_HPP
