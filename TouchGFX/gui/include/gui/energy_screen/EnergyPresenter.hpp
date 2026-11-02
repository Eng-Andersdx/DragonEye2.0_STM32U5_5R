#ifndef ENERGYPRESENTER_HPP
#define ENERGYPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class EnergyView;

class EnergyPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    EnergyPresenter(EnergyView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~EnergyPresenter() {}

private:
    EnergyPresenter();

    EnergyView& view;
};

#endif // ENERGYPRESENTER_HPP
