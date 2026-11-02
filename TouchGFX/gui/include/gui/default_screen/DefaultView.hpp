#ifndef DEFAULTVIEW_HPP
#define DEFAULTVIEW_HPP

#include <gui_generated/default_screen/DefaultViewBase.hpp>
#include <gui/default_screen/DefaultPresenter.hpp>

class DefaultView : public DefaultViewBase
{
public:
    DefaultView();
    virtual ~DefaultView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

protected:
    bool cloudMoving;
    bool cloud2Moving;
    bool hintShown;
    uint16_t hintTicks;
    static const uint16_t HINT_DELAY_TICKS;

    uint16_t chevronTicks;
    bool chevronBouncing;
    static const uint16_t CHEVRON_BOUNCE_INTERVAL;
};

#endif // DEFAULTVIEW_HPP
