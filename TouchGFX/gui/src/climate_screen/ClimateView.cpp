#include <gui/climate_screen/ClimateView.hpp>
#include <gui/common/DeviceSettings.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <stdio.h>

int g_climateTemp = 21;

bool  g_autoModeOn = false;
bool  g_fanOn      = false;
float g_autoAngle  = 0.0f;
float g_fanAngle   = 0.0f;

ClimateView::ClimateView()
{
}

void ClimateView::setupScreen()
{
    ClimateViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen

    // Set colour immediately based on restored temperature
    if (g_climateTemp <= 18)
    {
        // Really cold - blue
        currentR = 0x4A; currentG = 0x96; currentB = 0xEC;
    }
    else if (g_climateTemp <= 20)
    {
        // Cold - mid blue
        currentR = 0x6C; currentG = 0xAE; currentB = 0xF1;
    }
    else if (g_climateTemp <= 22)
    {
        // Cool - light blue
        currentR = 0x9B; currentG = 0xCE; currentB = 0xF9;
    }
    else if (g_climateTemp <= 24)
    {
        // Neutral - near white
        currentR = 0xD1; currentG = 0xEE; currentB = 0xF9;
    }
    else if (g_climateTemp <= 26)
    {
        // Warm - amber
        currentR = 0xFF; currentG = 0xC8; currentB = 0x73;
    }
    else if (g_climateTemp == 27)
    {
        currentR = 0xF0; currentG = 0x64; currentB = 0x41;
    }
    else
    {
        // Hot - red
        currentR = 0xE6; currentG = 0x1E; currentB = 0x1E;
    }

    targetR = currentR;
    targetG = currentG;
    targetB = currentB;

    climate_wheel_1Painter.setColor(touchgfx::Color::getColorFromRGB(currentR, currentG, currentB));
    climate_wheel_1.invalidate();

    climate_h1_text.setColor(
        touchgfx::Color::getColorFromRGB(currentR, currentG, currentB));
    climate_h1_text.invalidate();

    textFadePending = false;

    // Restore auto_circle rotation
    auto_circle.updateZAngle(g_autoAngle);
    auto_circle.invalidate();

    // Restore bitmaps based on saved state
    if (g_autoModeOn)
        auto_circle.setBitmap(touchgfx::Bitmap(BITMAP_AUTO_BLACK_ID));
    else
        auto_circle.setBitmap(touchgfx::Bitmap(BITMAP_AUTO_WHITE_ID));
    auto_circle.invalidate();

    if (g_fanOn)
        fan_1.setBitmap(touchgfx::Bitmap(BITMAP_FAN_BLACK_ID));
    else
        fan_1.setBitmap(touchgfx::Bitmap(BITMAP_FAN_WHITE_ID));
    fan_1.invalidate();

    autoTargetAngle  = g_autoAngle;
    autoCurrentAngle = g_autoAngle;
    autoAnimating    = false;
    autoAnimTick     = 0;

    // Restore toggle button visual states
    auto_mode.forceState(g_autoModeOn);
    auto_mode.invalidate();
    fan_toggle.forceState(g_fanOn);
    fan_toggle.invalidate();

    int mapped = ((g_climateTemp - TEMP_MIN) * 100) / (TEMP_MAX - TEMP_MIN);
    if (mapped > 100) mapped = 100;
    currentMappedValue = (float)mapped;
    targetMappedValue  = (float)mapped;

    // Apply immediately so wheel shows correct position on entry
    climate_wheel_1.setValue(mapped);
    climate_wheel_1.invalidate();

    // Apply dot position immediately too
    float pct   = (float)mapped / 100.0f;
    float angle = (ARC_START_DEG - 90.0f + pct *
                  (ARC_END_DEG - ARC_START_DEG)) * DEG_TO_RAD;

    int dotX = (int)(240.0f + 119.5f * cosf(angle))
               - climate_dot.getWidth()  / 2;
    int dotY = (int)(194.0f + 119.5f * sinf(angle))
               - climate_dot.getHeight() / 2;

    climate_dot.setXY(dotX, dotY);
    climate_dot.invalidate();

    updateDisplay();
}

void ClimateView::tearDownScreen()
{
    ClimateViewBase::tearDownScreen();
}

void ClimateView::onPlusClimateClicked()
{
    if (g_climateTemp < TEMP_MAX)
    {
        g_climateTemp++;
        updateDisplay();
    }
}

void ClimateView::onMinusClimateClicked()
{
    if (g_climateTemp > TEMP_MIN)
    {
        g_climateTemp--;
        updateDisplay();
    }
}

void ClimateView::onAutoModeClicked()
{
    g_autoModeOn = !g_autoModeOn;

    if (g_autoModeOn)
        auto_circle.setBitmap(touchgfx::Bitmap(BITMAP_AUTO_BLACK_ID));
    else
        auto_circle.setBitmap(touchgfx::Bitmap(BITMAP_AUTO_WHITE_ID));
    auto_circle.invalidate();

    autoCurrentAngle = g_autoAngle;
    g_autoAngle     += 3.2f;
    // Do NOT wrap g_autoAngle — let it accumulate indefinitely
    // so interpolation always goes forward (clockwise)
    autoTargetAngle  = g_autoAngle;

    autoAnimating = true;
    autoAnimTick  = 0;
}

void ClimateView::onFanToggleClicked()
{
    g_fanOn = !g_fanOn;

    if (g_fanOn)
        fan_1.setBitmap(touchgfx::Bitmap(BITMAP_FAN_BLACK_ID));
    else
        fan_1.setBitmap(touchgfx::Bitmap(BITMAP_FAN_WHITE_ID));

    fan_1.invalidate();
}

// Rotating the wheel: drag your finger around the ring, clockwise warms
// up, counter-clockwise cools down — same interaction as turning a
// physical dial. Only drags that *start* on top of climate_wheel_1 are
// honoured, so buttons elsewhere on the screen are unaffected.
void ClimateView::handleDragEvent(const touchgfx::DragEvent& evt)
{
    ClimateViewBase::handleDragEvent(evt);

    // Restrict to drags that started on (or near) the wheel widget.
    // TOUCH_MARGIN grows the touchable area beyond the wheel's own visible
    // bounds on every side — bump this up if swipes near the edge of the
    // wheel aren't registering, or reduce it if drags meant for something
    // else nearby are getting picked up.
    const int16_t TOUCH_MARGIN = 40;

    touchgfx::Rect wheelRect = climate_wheel_1.getAbsoluteRect();
    int16_t leftBound   = wheelRect.x - TOUCH_MARGIN;
    int16_t topBound    = wheelRect.y - TOUCH_MARGIN;
    int16_t rightBound  = wheelRect.x + wheelRect.width  + TOUCH_MARGIN;
    int16_t bottomBound = wheelRect.y + wheelRect.height + TOUCH_MARGIN;

    bool startedOnWheel =
        evt.getOldX() >= leftBound && evt.getOldX() < rightBound &&
        evt.getOldY() >= topBound  && evt.getOldY() < bottomBound;

    if (!startedOnWheel)
    {
        return;
    }

    // Use the wheel's own on-screen centre for the angle maths, so this
    // keeps working even if the widget is repositioned or resized later.
    float wheelCenterX = wheelRect.x + wheelRect.width  / 2.0f;
    float wheelCenterY = wheelRect.y + wheelRect.height / 2.0f;

    float oldDx = (float)evt.getOldX() - wheelCenterX;
    float oldDy = (float)evt.getOldY() - wheelCenterY;
    float newDx = (float)evt.getNewX() - wheelCenterX;
    float newDy = (float)evt.getNewY() - wheelCenterY;

    // Too close to the centre and the angle becomes unstable/undefined
    const float MIN_RADIUS = 20.0f;
    if ((oldDx * oldDx + oldDy * oldDy) < (MIN_RADIUS * MIN_RADIUS) ||
        (newDx * newDx + newDy * newDy) < (MIN_RADIUS * MIN_RADIUS))
    {
        return;
    }

    float oldAngle = atan2f(oldDy, oldDx);
    float newAngle = atan2f(newDy, newDx);

    float deltaAngle = newAngle - oldAngle;

    // Normalise so a drag crossing the -180/180 seam doesn't jump the wrong way
    const float PI = 3.14159265f;
    if (deltaAngle > PI)  deltaAngle -= 2.0f * PI;
    if (deltaAngle < -PI) deltaAngle += 2.0f * PI;

    float deltaDeg = deltaAngle * (180.0f / PI);

    // Accumulate fractional rotation between events; only step the
    // temperature once enough of the ring has been swept. Persists across
    // calls (there's only ever one ClimateView instance) without needing
    // a new member variable in the header.
    static float s_wheelDragAccumDeg = 0.0f;
    s_wheelDragAccumDeg += deltaDeg;

    // Degrees of drag rotation needed per 1 degree of temperature change —
    // tune this to make the wheel feel more or less sensitive.
    const float DEG_PER_STEP = 12.0f;

    while (s_wheelDragAccumDeg >= DEG_PER_STEP)
    {
        s_wheelDragAccumDeg -= DEG_PER_STEP;
        if (g_climateTemp < TEMP_MAX)
        {
            g_climateTemp++;
            updateDisplay();
        }
    }
    while (s_wheelDragAccumDeg <= -DEG_PER_STEP)
    {
        s_wheelDragAccumDeg += DEG_PER_STEP;
        if (g_climateTemp > TEMP_MIN)
        {
            g_climateTemp--;
            updateDisplay();
        }
    }
}

void ClimateView::updateDisplay()
{
    // Single shared colour block
    uint8_t newR, newG, newB;
    if (g_climateTemp <= 18)
    {
        newR = 0x4A; newG = 0x96; newB = 0xEC;
    }
    else if (g_climateTemp <= 20)
    {
        newR = 0x6C; newG = 0xAE; newB = 0xF1;
    }
    else if (g_climateTemp <= 22)
    {
        newR = 0x9B; newG = 0xCE; newB = 0xF9;
    }
    else if (g_climateTemp <= 24)
    {
        newR = 0xD1; newG = 0xEE; newB = 0xF9;
    }
    else if (g_climateTemp <= 26)
    {
        newR = 0xFF; newG = 0xC8; newB = 0x73;
    }
    else if (g_climateTemp == 27)
    {
        newR = 0xF0; newG = 0x64; newB = 0x41;
    }
    else
    {
        newR = 0xE6; newG = 0x1E; newB = 0x1E;
    }

    // Set wheel target for gradual tick interpolation
    targetR = newR; targetG = newG; targetB = newB;

    // Store pending colour and start fade out
    pendingR = newR; pendingG = newG; pendingB = newB;
    textFadePending = true;
    climate_h1_text.startFadeAnimation(0, 15);

    // Update temperature text
    touchgfx::Unicode::snprintf(tempBuf, 6, "%d", g_climateTemp);
    climate_temp.setWildcard(tempBuf);
    climate_temp.invalidate();

    // Update status text
    if (g_climateTemp <= COOL_THRESHOLD)
    {
        touchgfx::Unicode::snprintf(statusBuf, 20, "Cooling to %d%c", g_climateTemp, 0xB0);
    }
    else
    {
        touchgfx::Unicode::snprintf(statusBuf, 20, "Heating to %d%c", g_climateTemp, 0xB0);
    }
    climate_h1_text.setWildcard(statusBuf);
    climate_h1_text.invalidate();

    // Map temperature to circle range
    int mapped = ((g_climateTemp - TEMP_MIN) * 100) / (TEMP_MAX - TEMP_MIN);
    if (mapped > 100) mapped = 100;
    targetMappedValue = (float)mapped;
}

void ClimateView::handleTickEvent()
{
    ClimateViewBase::handleTickEvent();

    // Slowly interpolate current colour toward target
    bool changed = false;

    if (currentR != targetR) { currentR += (currentR < targetR) ? 1 : -1; changed = true; }
    if (currentG != targetG) { currentG += (currentG < targetG) ? 1 : -1; changed = true; }
    if (currentB != targetB) { currentB += (currentB < targetB) ? 1 : -1; changed = true; }

    if (changed)
    {
        climate_wheel_1Painter.setColor(touchgfx::Color::getColorFromRGB(currentR, currentG, currentB));
        climate_wheel_1.invalidate();
    }

    // Ease wheel and dot toward target value
    if (currentMappedValue != targetMappedValue)
    {
        float diff = targetMappedValue - currentMappedValue;
        currentMappedValue += diff * 0.12f;

        // Snap when close enough
        if (fabsf(diff) < 0.5f)
            currentMappedValue = targetMappedValue;

        climate_wheel_1.setValue((int)currentMappedValue);
        climate_wheel_1.invalidate();

        // Update dot position
        float pct   = currentMappedValue / 100.0f;
        float angle = (ARC_START_DEG - 90.0f + pct *
                      (ARC_END_DEG - ARC_START_DEG)) * DEG_TO_RAD;

        int dotX = (int)(240.0f + 119.5f * cosf(angle))
                   - climate_dot.getWidth()  / 2;
        int dotY = (int)(194.0f + 119.5f * sinf(angle))
                   - climate_dot.getHeight() / 2;

        climate_dot.invalidate();
        climate_dot.setXY(dotX, dotY);
        climate_dot.invalidate();
    }

    // When text has faded out, swap colour and fade back in
    if (textFadePending && climate_h1_text.getAlpha() == 0)
    {
        textFadePending = false;
        climate_h1_text.setColor(
            touchgfx::Color::getColorFromRGB(pendingR, pendingG, pendingB));
        climate_h1_text.startFadeAnimation(255, 15);
    }

    // Rotate fan when active
    if (g_fanOn)
    {
        g_fanAngle -= 0.08f;  // was 0.05f
        if (g_fanAngle < 0.0f) g_fanAngle += 6.28318f;
        fan_1.updateZAngle(g_fanAngle);
        fan_1.invalidate();
    }

    // Animate auto_circle rotation with cubic ease in/out
    if (autoAnimating)
    {
        autoAnimTick++;
        float t = (float)autoAnimTick / (float)AUTO_ANIM_STEPS;
        if (t > 1.0f) t = 1.0f;

        // Cubic ease in/out: t<0.5 ? 4t^3 : 1-(-2t+2)^3/2
        float eased;
        if (t < 0.5f)
            eased = 4.0f * t * t * t;
        else
        {
            float f = -2.0f * t + 2.0f;
            eased = 1.0f - (f * f * f) / 2.0f;
        }

        float angle = autoCurrentAngle + eased * (autoTargetAngle - autoCurrentAngle);
        auto_circle.updateZAngle(angle);
        auto_circle.invalidate();

        if (autoAnimTick >= AUTO_ANIM_STEPS)
        {
            autoAnimating = false;
            auto_circle.updateZAngle(autoTargetAngle);
            auto_circle.invalidate();
        }
    }
}
