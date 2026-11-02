#ifndef LIGHTSVIEW_HPP
#define LIGHTSVIEW_HPP

#include <gui_generated/lights_screen/LightsViewBase.hpp>
#include <gui/lights_screen/LightsPresenter.hpp>
#include <BitmapDatabase.hpp>

extern int g_lightsOn;
extern bool g_lrOn;
extern bool g_kitchenOn;
extern bool g_hallwayOn;
extern bool g_conservatoryOn;

class LightsView : public LightsViewBase
{
public:
    LightsView();
    virtual ~LightsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void onLRToggleClicked();
    virtual void onKitchenToggleClicked();
    virtual void onHallwayToggleClicked();
    virtual void onConservatoryToggleClicked();

    virtual void onLRSliderChanged(int value);
    virtual void onKitchenSliderChanged(int value);
    virtual void onHallwaySliderChanged(int value);
    virtual void onConservatorySliderChanged(int value);

protected:
    bool lrOn;
    bool kitchenOn;
    bool hallwayOn;
    bool conservatoryOn;

    float star1CurrentScale;
    float star1TargetScale;
    float star2CurrentScale;
    float star2TargetScale;

    float sunBeamsCurrentAngle;
    float sunBeamsTargetAngle;
    float sunBodyCurrentScale;
    float sunBodyTargetScale;
    bool  sunBodyShrinking;

    virtual void handleTickEvent() override;
    virtual void onBedtimeClicked();
    virtual void onAllbuttonClicked();

    void updateLightWidget(bool state,
        touchgfx::Image& lightImg,
        touchgfx::Slider& slider,
        touchgfx::Image& lightOn);
    void updateLightAlpha(int sliderValue,
                          touchgfx::Image& lightOn);
    void updateLightsCount();
};

#endif // LIGHTSVIEW_HPP
