#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>
#include <BitmapDatabase.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/Color.hpp>
#include <math.h>

extern int g_currentTrack;
extern int g_climateTemp;
extern int g_lightsOn;
extern bool g_frontDoorLocked;
extern bool g_isPlaying;
extern bool g_lrOn;
extern bool g_kitchenOn;
extern bool g_hallwayOn;
extern bool g_conservatoryOn;

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void onSkipMainClicked();
    virtual void onBackMainClicked();
    virtual void onPlayPauseMainClicked();

protected:
    static constexpr float MAIN_ARC_START_DEG = 250.0f;
    static constexpr float MAIN_ARC_END_DEG   = 472.5f;
    static constexpr float MAIN_DEG_TO_RAD    = 3.14159265f / 180.0f;
    static constexpr float MAIN_DOT_RADIUS    = 59.7f;

    uint8_t mainWheelR;
    uint8_t mainWheelG;
    uint8_t mainWheelB;

    float star1CurrentScale;
    float star1TargetScale;
    float star2CurrentScale;
    float star2TargetScale;

    float sunBeamsTargetAngle;
    float sunBeamsCurrentAngle;
    float sunBodyCurrentScale;
    float sunBodyTargetScale;
    bool sunBodyShrinking;

    bool securityFadePending;
    touchgfx::colortype pendingSecurityColour;
    Unicode::UnicodeChar securityBuf[10];
    Unicode::UnicodeChar securityPendingBuf[10];

    void triggerMainLockAnimation(bool unlocking);
    void updateSecurityStatus();

    virtual void handleTickEvent() override;
    virtual void onBedtimeClicked();
    virtual void onWakeupClicked();

    void updateMusicCard();
    void updateClimateDot();
    void updateClimateWheel();
};

#endif // MAINVIEW_HPP
