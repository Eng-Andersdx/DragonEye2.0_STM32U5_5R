#include <gui/energy_screen/EnergyView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <touchgfx/Unicode.hpp>

/* Remember the last chosen period while the device is running */
static bool g_energyShowMonth = false;

EnergyView::EnergyView()
{

}

void EnergyView::setupScreen()
{
    EnergyViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen
    showPeriod(g_energyShowMonth);
}

void EnergyView::tearDownScreen()
{
    EnergyViewBase::tearDownScreen();
}

void EnergyView::onWeekClicked()
{
    showPeriod(false);
}

void EnergyView::onMonthClicked()
{
    showPeriod(true);
}

void EnergyView::showPeriod(bool month)
{
    g_energyShowMonth = month;

    // Week / Month behave like a segmented control: exactly one is selected
    week_btn.forceState(!month);
    month_btn.forceState(month);
    week_btn.invalidate();
    month_btn.invalidate();

    chart_week.setVisible(!month);
    chart_month.setVisible(month);
    chart_week.invalidate();
    chart_month.invalidate();

    // Demo figures, matching the bars drawn in chart_week.png / chart_month.png
    Unicode::strncpy(total_valueBuffer, month ? "378.0 kWh" : "87.4 kWh", TOTAL_VALUE_SIZE);
    Unicode::strncpy(total_labelBuffer, month ? "Last 30 days  -  12.6 kWh per day"
                                              : "This week  -  12.5 kWh per day", TOTAL_LABEL_SIZE);
    total_value.invalidate();
    total_label.invalidate();
}
