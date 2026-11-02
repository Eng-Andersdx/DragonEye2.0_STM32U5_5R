#ifndef ENERGYVIEW_HPP
#define ENERGYVIEW_HPP

#include <gui_generated/energy_screen/EnergyViewBase.hpp>
#include <gui/energy_screen/EnergyPresenter.hpp>

class EnergyView : public EnergyViewBase
{
public:
    EnergyView();
    virtual ~EnergyView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* Interactions defined in TouchGFX Designer (Week / Month buttons) */
    virtual void onWeekClicked();
    virtual void onMonthClicked();

protected:
    void showPeriod(bool month);
};

#endif // ENERGYVIEW_HPP
