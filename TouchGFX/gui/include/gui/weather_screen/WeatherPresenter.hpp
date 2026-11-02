#ifndef WEATHERPRESENTER_HPP
#define WEATHERPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class WeatherView;

class WeatherPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    WeatherPresenter(WeatherView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~WeatherPresenter() {}

private:
    WeatherPresenter();

    WeatherView& view;
};

#endif // WEATHERPRESENTER_HPP
