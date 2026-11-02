#ifndef WEATHERVIEW_HPP
#define WEATHERVIEW_HPP

#include <gui_generated/weather_screen/WeatherViewBase.hpp>
#include <gui/weather_screen/WeatherPresenter.hpp>

class WeatherView : public WeatherViewBase
{
public:
    WeatherView();
    virtual ~WeatherView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
};

#endif // WEATHERVIEW_HPP
