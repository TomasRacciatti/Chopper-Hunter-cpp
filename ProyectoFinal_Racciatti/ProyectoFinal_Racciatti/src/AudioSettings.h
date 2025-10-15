#pragma once

class AudioSettings 
{
public:
    float GetMasterVolume() const { return _master; }
    void  SetMasterVolume(float value) { _master = Clamp100(value); }

    float GetMusicVolume() const { return (_master * _music / 100.f) * _musicController; }
    void  SetMusicVolume(float value) { _music = Clamp100(value); }

    float GetSfxVolume() const { return (_master * _sfx / 100.f) * _sfxController; }
    void  SetSfxVolume(float value) { _sfx = Clamp100(value); }

    void  SetMusicController(float value) { _musicController = Clamp01(value); }
    void  SetSfxController(float value) { _sfxController = Clamp01(value); }


private:
    float _master = 20.f;
    float _music = 100.f;
    float _sfx = 100.f;

    float _musicController = 1.f;
    float _sfxController = 1.f;

    static float Clamp100(float value) 
    {
        if (value < 0.f)   return 0.f;
        if (value > 100.f) return 100.f;
        return value;
    }

    static float Clamp01(float value)
    {
        if (value < 0.f)   return 0.f;
        if (value > 1.f) return 1.f;
        return value;
    }
};
