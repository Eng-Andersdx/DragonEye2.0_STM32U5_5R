#include <gui/music_screen/MusicView.hpp>
#include <gui/common/DeviceSettings.hpp>

int g_currentTrack = 0;
bool g_isPlaying = false;

const MusicView::Track MusicView::tracks[NUM_TRACKS] =
{
    {
        "The Chain",
        "Fleetwood Mac",
        268,
        BITMAP_COVER_ART_FM_ID,
        BITMAP_ALBUM_1_ID,
        BITMAP_FM_BG_ID
    },
    {
        "Money",
        "Pink Floyd",
        170,
        BITMAP_COVER_ART_PF_ID,
        BITMAP_ALBUM_2_ID,
        BITMAP_PF_BG_ID
    },
    {
        "May Ninth",
        "Khruangbin",
        259,
        BITMAP_COVER_ART_O_ID,
        BITMAP_ALBUM_3_ID,
        BITMAP_O_BG_ID
    }
};

MusicView::MusicView()
    : scrubbing(false),
      timelineStartedCallback(this, &MusicView::timelineStarted),
      timelineMovedCallback(this, &MusicView::timelineMoved),
      timelineReleasedCallback(this, &MusicView::timelineReleased)
{
}

void MusicView::setupScreen()
{
    MusicViewBase::setupScreen();
    applyHeaderStatus(wifi, bt);   // Wi-Fi / Bluetooth icons follow the Settings screen

    // Let the user drag the timeline to seek
    scrubbing = false;
    timeline_music.setStartValueCallback(timelineStartedCallback);
    timeline_music.setNewValueCallback(timelineMovedCallback);
    timeline_music.setStopValueCallback(timelineReleasedCallback);

    currentTrack = g_currentTrack;  // restore last selected track
    elapsedTicks = 0;
    isPlaying    = g_isPlaying;

    // Sync button visual state
    play_pause_music.forceState(isPlaying);
    play_pause_music.invalidate();

    // Sync animation state
    if (isPlaying)
    {
        music_off.startFadeAnimation(0, 1);
        music_ani_music.startFadeAnimation(255, 1);
        music_ani_music.startAnimation(false, true, true);
    }
    else
    {
        music_ani_music.setAlpha(0);
        music_ani_music.stopAnimation();
        music_off.setAlpha(255);
        music_ani_music.invalidate();
        music_off.invalidate();
    }

    loadTrack(currentTrack);
}

void MusicView::tearDownScreen()
{
    MusicViewBase::tearDownScreen();
}

void MusicView::handleTickEvent()
{
    MusicViewBase::handleTickEvent();

    if (!isPlaying)
        return;

    elapsedTicks++;

    // Update every 60 ticks = 1 second at 60fps
    if (elapsedTicks % 60 == 0)
    {
        int elapsed = elapsedTicks / 60;
        int duration = tracks[currentTrack].durationSeconds;

        // Clamp at end of track
        if (elapsed >= duration)
        {
            elapsedTicks = duration * 60;
            stopAtEndOfTrack();
        }

        // Don't fight the user's finger while they drag the timeline
        if (!scrubbing)
        {
            updateTimelineFromPlayback();
        }

        updateTimers();
    }
}

void MusicView::onSkipClicked()
{
    currentTrack = (currentTrack + 1) % NUM_TRACKS;
    elapsedTicks = 0;
    isPlaying    = true;
    g_isPlaying  = true;
    loadTrack(currentTrack);

    // Sync play/pause button visual state
    play_pause_music.forceState(true);
    play_pause_music.invalidate();

    // Show animation, hide static image
    music_off.startFadeAnimation(0, 1);
    music_ani_music.startFadeAnimation(255, 1);
    music_ani_music.startAnimation(false, true, true);
}

void MusicView::onBackClicked()
{
    // If more than 3 seconds in, restart current track
    if (elapsedTicks > 180)
    {
        elapsedTicks = 0;
        isPlaying    = true;
    g_isPlaying  = true;
        loadTrack(currentTrack);

        // Sync play/pause button visual state
        play_pause_music.forceState(true);
        play_pause_music.invalidate();

        // Show animation, hide static image
        music_off.startFadeAnimation(0, 1);
        music_ani_music.startFadeAnimation(255, 1);
        music_ani_music.startAnimation(false, true, true);

        return;
    }

    currentTrack = (currentTrack - 1 + NUM_TRACKS) % NUM_TRACKS;
    elapsedTicks = 0;
    isPlaying    = true;
    g_isPlaying  = true;
    loadTrack(currentTrack);

    // Sync play/pause button visual state
    play_pause_music.forceState(true);
    play_pause_music.invalidate();

    // Show animation, hide static image
    music_off.startFadeAnimation(0, 1);
    music_ani_music.startFadeAnimation(255, 1);
    music_ani_music.startAnimation(false, true, true);
}

void MusicView::onPlayPauseClicked()
{
    g_isPlaying = !g_isPlaying;
    isPlaying = !isPlaying;

    if (isPlaying)
    {
        // Start playing — show animation, hide static image
        music_off.startFadeAnimation(0, 1);   // instant fade out
        music_ani_music.startFadeAnimation(255, 1); // instant fade in
        music_ani_music.startAnimation(false, true, true); // loop
    }
    else
    {
        // Paused — hide animation quickly, show static image
        music_ani_music.startFadeAnimation(0, 8);  // quick fade out
        music_off.startFadeAnimation(255, 8);       // quick fade in
        music_ani_music.stopAnimation();
    }
}

void MusicView::loadTrack(int idx)
{
    g_currentTrack = idx;

    // Background
    FM_blurred_bg.setBitmap(touchgfx::Bitmap(tracks[idx].background));
    FM_blurred_bg.invalidate();

    // Album cover
    album_cover.setBitmap(touchgfx::Bitmap(tracks[idx].albumCover));
    album_cover.invalidate();

    // Track name
    Unicode::strncpy(trackBuf, tracks[idx].title, 32);
    music_track.setWildcard(trackBuf);
    music_track.invalidate();

    // Artist name
    Unicode::strncpy(artistBuf, tracks[idx].artist, 32);
    music_artist.setWildcard(artistBuf);
    music_artist.invalidate();

    // Reset slider
    timeline_music.setValue(0);

    // Reset timers
    updateTimers();

    // Reset animation state on track change
    if (isPlaying)
    {
        music_ani_music.stopAnimation();
        music_ani_music.startAnimation(false, true, true);
    }
}

/* Slider value <-> playback position (the timeline slider's range is set in Designer) */
int MusicView::sliderToTicks(int value) const
{
    int range = timeline_music.getMaxValue() - timeline_music.getMinValue();
    if (range <= 0) return 0;
    int duration = tracks[currentTrack].durationSeconds;
    return ((value - timeline_music.getMinValue()) * duration / range) * 60;
}

void MusicView::updateTimelineFromPlayback()
{
    int duration = tracks[currentTrack].durationSeconds;
    int elapsed  = elapsedTicks / 60;
    int range    = timeline_music.getMaxValue() - timeline_music.getMinValue();
    timeline_music.setValue(timeline_music.getMinValue() + (elapsed * range) / duration);
}

void MusicView::timelineStarted(const touchgfx::Slider& s, int value)
{
    scrubbing = true;
}

void MusicView::timelineMoved(const touchgfx::Slider& s, int value)
{
    if (!scrubbing) return;           // ignore our own setValue() calls during playback
    elapsedTicks = sliderToTicks(value);
    updateTimers();                   // show the time under the finger while dragging
}

void MusicView::timelineReleased(const touchgfx::Slider& s, int value)
{
    scrubbing    = false;
    elapsedTicks = sliderToTicks(value);   // continue playing from the new position
    updateTimers();
}

void MusicView::stopAtEndOfTrack()
{
    isPlaying   = false;
    g_isPlaying = false;
    play_pause_music.forceState(false);
    play_pause_music.invalidate();
    music_ani_music.startFadeAnimation(0, 8);
    music_off.startFadeAnimation(255, 8);
    music_ani_music.stopAnimation();
}

void MusicView::updateTimers()
{
    int elapsed  = elapsedTicks / 60;
    int duration = tracks[currentTrack].durationSeconds;
    if (elapsed > duration) elapsed = duration;
    int remaining = duration - elapsed;

    // Elapsed: M:SS
    Unicode::snprintf(elapsedBuf, 6, "%d:%02d",
        elapsed / 60, elapsed % 60);
    time_elapsed.setWildcard(elapsedBuf);
    time_elapsed.invalidate();

    // Remaining: -M:SS
    Unicode::snprintf(remainingBuf, 7, "-%d:%02d",
        remaining / 60, remaining % 60);
    time_remaining.setWildcard(remainingBuf);
    time_remaining.invalidate();
}
