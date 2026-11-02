#ifndef DEFAULTPRESENTER_HPP
#define DEFAULTPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DefaultView;

class DefaultPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DefaultPresenter(DefaultView& v);

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

    virtual ~DefaultPresenter() {}

private:
    DefaultPresenter();

    DefaultView& view;
};

#endif // DEFAULTPRESENTER_HPP
