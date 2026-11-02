#ifndef SECURITYPRESENTER_HPP
#define SECURITYPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SecurityView;

class SecurityPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SecurityPresenter(SecurityView& v);

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

    virtual ~SecurityPresenter() {}

private:
    SecurityPresenter();

    SecurityView& view;
};

#endif // SECURITYPRESENTER_HPP
