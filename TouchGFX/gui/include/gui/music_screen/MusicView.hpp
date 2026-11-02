#ifndef MUSICVIEW_HPP
#define MUSICVIEW_HPP

#include <gui_generated/music_screen/MusicViewBase.hpp>
#include <gui/music_screen/MusicPresenter.hpp>
#include <touchgfx/Unicode.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>

extern int g_currentTrack;
extern bool g_isPlaying;

class MusicView : public MusicViewBase
{
public:
    MusicView();
    virtual ~MusicView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    virtual void onSkipClicked();
    virtual void onBackClicked();
    virtual void onPlayPauseClicked();

protected:
    struct Track
    {
        const char* title;
        const char* artist;
        int         durationSeconds;
        BitmapId    albumCover;
        BitmapId    albumCoverMain;
        BitmapId    background;
    };

    static const int NUM_TRACKS = 3;
    static const Track tracks[NUM_TRACKS];

    int  currentTrack;
    int  elapsedTicks;
    bool isPlaying;

    void loadTrack(int idx);
    void updateTimers();
    void updateTimelineFromPlayback();
    void stopAtEndOfTrack();

    /* Timeline (seek) slider: dragging it moves the playback position */
    bool scrubbing;
    void timelineStarted(const touchgfx::Slider& s, int value);
    void timelineMoved(const touchgfx::Slider& s, int value);
    void timelineReleased(const touchgfx::Slider& s, int value);
    touchgfx::Callback<MusicView, const touchgfx::Slider&, int> timelineStartedCallback;
    touchgfx::Callback<MusicView, const touchgfx::Slider&, int> timelineMovedCallback;
    touchgfx::Callback<MusicView, const touchgfx::Slider&, int> timelineReleasedCallback;
    int sliderToTicks(int value) const;

    Unicode::UnicodeChar elapsedBuf[6];
    Unicode::UnicodeChar remainingBuf[7];
    Unicode::UnicodeChar trackBuf[32];
    Unicode::UnicodeChar artistBuf[32];
};

#endif // MUSICVIEW_HPP
