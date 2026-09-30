#include "audioManager.h"
#include <algorithm>

bool AudioManager::isAudioReady() const
{
    return IsAudioDeviceReady();
}

void AudioManager::init()
{
    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
    }
    m_isInitialized = IsAudioDeviceReady();
}

void AudioManager::loadAssets()
{
    if (!isAudioReady())
    {
        return;
    }

    // 1. Music Streams
    m_menuMusic = LoadMusicStream(RESOURCES_PATH "audio/music/menu_loop.mp3");
    if (isMenuMusicLoaded())
    {
        SetMusicVolume(m_menuMusic, m_musicVolume);
    }

    m_gameplayMusic = LoadMusicStream(RESOURCES_PATH "audio/music/gameplay_loop.mp3");
    if (isGameplayMusicLoaded())
    {
        SetMusicVolume(m_gameplayMusic, m_musicVolume);
    }

    // 2. Sound Effects
    m_collisionSfx = LoadSound(RESOURCES_PATH "audio/sfx/collision.mp3");
    if (isCollisionSfxLoaded())
    {
        SetSoundVolume(m_collisionSfx, m_sfxVolume);
    }

    m_deathSfx = LoadSound(RESOURCES_PATH "audio/sfx/death.mp3");
    if (isDeathSfxLoaded())
    {
        SetSoundVolume(m_deathSfx, m_sfxVolume);
    }

    // Beep SFX: attempt loading mp3 then wav; if missing, remains safely null
    m_beepSfx = LoadSound(RESOURCES_PATH "audio/sfx/beep.mp3");
    if (!isBeepSfxLoaded())
    {
        m_beepSfx = LoadSound(RESOURCES_PATH "audio/sfx/beep.wav");
    }
    if (isBeepSfxLoaded())
    {
        SetSoundVolume(m_beepSfx, m_beepVolume);
    }
}

void AudioManager::freeAssets()
{
    stopMusic();

    if (isMenuMusicLoaded())
    {
        UnloadMusicStream(m_menuMusic);
        m_menuMusic = {};
    }

    if (isGameplayMusicLoaded())
    {
        UnloadMusicStream(m_gameplayMusic);
        m_gameplayMusic = {};
    }

    if (isCollisionSfxLoaded())
    {
        UnloadSound(m_collisionSfx);
        m_collisionSfx = {};
    }

    if (isDeathSfxLoaded())
    {
        UnloadSound(m_deathSfx);
        m_deathSfx = {};
    }

    if (isBeepSfxLoaded())
    {
        UnloadSound(m_beepSfx);
        m_beepSfx = {};
    }
}

void AudioManager::cleanup()
{
    freeAssets();

    if (IsAudioDeviceReady())
    {
        CloseAudioDevice();
    }
    m_isInitialized = false;
}

void AudioManager::update()
{
    if (!isAudioReady() || m_isPaused)
    {
        return;
    }

    if (m_currentTrack == MusicTrack::Menu && isMenuMusicLoaded())
    {
        UpdateMusicStream(m_menuMusic);
    }
    else if (m_currentTrack == MusicTrack::Gameplay && isGameplayMusicLoaded())
    {
        UpdateMusicStream(m_gameplayMusic);
    }
}

void AudioManager::playMenuMusic(bool restart)
{
    if (m_currentTrack == MusicTrack::Menu && !restart && !m_isPaused)
    {
        return;
    }

    // Stop gameplay music if running
    if (m_currentTrack == MusicTrack::Gameplay && isGameplayMusicLoaded())
    {
        StopMusicStream(m_gameplayMusic);
    }

    if (isMenuMusicLoaded())
    {
        if (restart)
        {
            StopMusicStream(m_menuMusic);
        }
        PlayMusicStream(m_menuMusic);
        SetMusicVolume(m_menuMusic, m_musicVolume);
    }

    m_currentTrack = MusicTrack::Menu;
    m_isPaused = false;
}

void AudioManager::playGameplayMusic(bool restart)
{
    if (m_currentTrack == MusicTrack::Gameplay && !restart && !m_isPaused)
    {
        return;
    }

    // Stop menu music if running
    if (m_currentTrack == MusicTrack::Menu && isMenuMusicLoaded())
    {
        StopMusicStream(m_menuMusic);
    }

    if (isGameplayMusicLoaded())
    {
        if (restart)
        {
            StopMusicStream(m_gameplayMusic);
        }
        PlayMusicStream(m_gameplayMusic);
        SetMusicVolume(m_gameplayMusic, m_musicVolume);
    }

    m_currentTrack = MusicTrack::Gameplay;
    m_isPaused = false;
}

void AudioManager::pauseGameplayMusic()
{
    if (m_currentTrack == MusicTrack::Gameplay && !m_isPaused)
    {
        if (isGameplayMusicLoaded())
        {
            PauseMusicStream(m_gameplayMusic);
        }
        m_isPaused = true;
    }
}

void AudioManager::resumeGameplayMusic()
{
    if (m_currentTrack == MusicTrack::Gameplay && m_isPaused)
    {
        if (isGameplayMusicLoaded())
        {
            ResumeMusicStream(m_gameplayMusic);
        }
        m_isPaused = false;
    }
}

void AudioManager::stopMusic()
{
    if (m_currentTrack == MusicTrack::Gameplay && isGameplayMusicLoaded())
    {
        StopMusicStream(m_gameplayMusic);
    }
    else if (m_currentTrack == MusicTrack::Menu && isMenuMusicLoaded())
    {
        StopMusicStream(m_menuMusic);
    }

    m_currentTrack = MusicTrack::None;
    m_isPaused = false;
}

void AudioManager::playCollisionSound()
{
    if (isCollisionSfxLoaded())
    {
        SetSoundVolume(m_collisionSfx, m_sfxVolume);
        PlaySound(m_collisionSfx);
    }
}

void AudioManager::playDeathSound()
{
    if (isDeathSfxLoaded())
    {
        SetSoundVolume(m_deathSfx, m_sfxVolume);
        PlaySound(m_deathSfx);
    }
}

void AudioManager::playBeepSound()
{
    if (isBeepSfxLoaded())
    {
        SetSoundVolume(m_beepSfx, m_beepVolume);
        PlaySound(m_beepSfx);
    }
}

void AudioManager::setMusicVolume(float volume)
{
    m_musicVolume = std::clamp(volume, 0.0f, 1.0f);
    if (isMenuMusicLoaded())
    {
        SetMusicVolume(m_menuMusic, m_musicVolume);
    }
    if (isGameplayMusicLoaded())
    {
        SetMusicVolume(m_gameplayMusic, m_musicVolume);
    }
}

void AudioManager::setSfxVolume(float volume)
{
    m_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    if (isCollisionSfxLoaded())
    {
        SetSoundVolume(m_collisionSfx, m_sfxVolume);
    }
    if (isDeathSfxLoaded())
    {
        SetSoundVolume(m_deathSfx, m_sfxVolume);
    }
}

void AudioManager::setBeepVolume(float volume)
{
    m_beepVolume = std::clamp(volume, 0.0f, 1.0f);
    if (isBeepSfxLoaded())
    {
        SetSoundVolume(m_beepSfx, m_beepVolume);
    }
}
