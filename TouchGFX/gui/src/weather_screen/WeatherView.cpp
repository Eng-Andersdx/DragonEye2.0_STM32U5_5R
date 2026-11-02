#include <gui/weather_screen/WeatherView.hpp>
#include <gui/common/DeviceSettings.hpp>

WeatherView::WeatherView()
{

}

void WeatherView::setupScreen()
{
    WeatherViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen
}

void WeatherView::tearDownScreen()
{
    WeatherViewBase::tearDownScreen();
}
