#ifndef CLIMATEPRESENTER_HPP
#define CLIMATEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ClimateView;

class ClimatePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ClimatePresenter(ClimateView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ClimatePresenter() {}

private:
    ClimatePresenter();

    ClimateView& view;
};

#endif // CLIMATEPRESENTER_HPP
