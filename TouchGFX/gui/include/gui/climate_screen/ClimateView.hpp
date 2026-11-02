#ifndef CLIMATEVIEW_HPP
#define CLIMATEVIEW_HPP

#include <gui_generated/climate_screen/ClimateViewBase.hpp>
#include <gui/climate_screen/ClimatePresenter.hpp>
#include <touchgfx/Unicode.hpp>
#include <math.h>
#include <BitmapDatabase.hpp>

extern int g_climateTemp;
extern bool g_autoModeOn;
extern bool g_fanOn;
extern float g_autoAngle;
extern float g_fanAngle;

class ClimateView : public ClimateViewBase
{
public:
    ClimateView();
    virtual ~ClimateView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;
    virtual void handleDragEvent(const touchgfx::DragEvent& evt) override;

    virtual void onPlusClimateClicked();
    virtual void onMinusClimateClicked();

    virtual void onAutoModeClicked();
    virtual void onFanToggleClicked();

protected:
    static const int TEMP_MIN = 15;
    static const int TEMP_MAX = 28;
    static const int COOL_THRESHOLD = 21;

    static constexpr float ARC_START_DEG = 250.0f;
    static constexpr float ARC_END_DEG   = 470.0f;
    static constexpr float DEG_TO_RAD    = 3.14159265f / 180.0f;
    static constexpr float DOT_RADIUS    = 121.5f;
    static constexpr float CENTER_X      = 127.3f;
    static constexpr float CENTER_Y      = 127.0f;

    void updateDisplay();

    touchgfx::Unicode::UnicodeChar tempBuf[6];
    touchgfx::Unicode::UnicodeChar statusBuf[20];

    uint8_t currentR;
    uint8_t currentG;
    uint8_t currentB;
    uint8_t targetR;
    uint8_t targetG;
    uint8_t targetB;

    bool textFadePending;
    uint8_t pendingR;
    uint8_t pendingG;
    uint8_t pendingB;

    float autoTargetAngle;
    float autoCurrentAngle;
    bool autoAnimating;
    int autoAnimTick;
    static const int AUTO_ANIM_STEPS = 40;

    float currentMappedValue;
    float targetMappedValue;
};

#endif // CLIMATEVIEW_HPP
