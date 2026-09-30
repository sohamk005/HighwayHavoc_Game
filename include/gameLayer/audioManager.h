#pragma once

#include <raudio.h>
#include <string>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "resources/"
#endif

class AudioManager
{
public:
    enum class MusicTrack
    {
        None,
        Menu,
        Gameplay
    };

    AudioManager() = default;
    ~AudioManager() = default;

    // Subsystem Lifecycle
    void init();
    void loadAssets();
    void freeAssets();
    void cleanup();

    // Frame update for streaming music buffer refills
    void update();

    // Music State Controls
    void playMenuMusic(bool restart = false);
    void playGameplayMusic(bool restart = false);
    void pauseGameplayMusic();
    void resumeGameplayMusic();
    void stopMusic();

    // Sound Effect Triggers
    void playCollisionSound();
    void playDeathSound();
    void playBeepSound();

    // Volume & Mixing Controls (0.0f - 1.0f)
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);
    void setBeepVolume(float volume);
    float getMusicVolume() const { return m_musicVolume; }
    float getSfxVolume() const { return m_sfxVolume; }
    float getBeepVolume() const { return m_beepVolume; }

    // State Inspection
    MusicTrack getCurrentTrack() const { return m_currentTrack; }
    bool isMusicPaused() const { return m_isPaused; }
    bool isAudioReady() const;

    // Asset status query (safe check if underlying buffer exists)
    bool isMenuMusicLoaded() const { return m_menuMusic.ctxData != nullptr; }
    bool isGameplayMusicLoaded() const { return m_gameplayMusic.ctxData != nullptr; }
    bool isCollisionSfxLoaded() const { return m_collisionSfx.stream.buffer != nullptr; }
    bool isDeathSfxLoaded() const { return m_deathSfx.stream.buffer != nullptr; }
    bool isBeepSfxLoaded() const { return m_beepSfx.stream.buffer != nullptr; }

private:
    Music m_menuMusic = {};
    Music m_gameplayMusic = {};

    Sound m_collisionSfx = {};
    Sound m_deathSfx = {};
    Sound m_beepSfx = {};

    MusicTrack m_currentTrack = MusicTrack::None;
    bool m_isPaused = false;
    bool m_isInitialized = false;

    // Default arcade volume mix
    float m_musicVolume = 0.50f;
    float m_sfxVolume = 0.85f;
    float m_beepVolume = 0.60f;
};
