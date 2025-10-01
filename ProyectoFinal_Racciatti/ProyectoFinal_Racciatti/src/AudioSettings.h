#pragma once

class AudioSettings 
{
public:
    float GetMasterVolume() const { return _master; }
    void  SetMasterVolume(float value) { _master = Clamp(value); }

    float GetMusicVolume() const { return _master * _music / 100.f; }
    void  SetMusicVolume(float value) { _music = Clamp(value); }

    float GetSfxVolume() const { return _master * _sfx / 100.f; }
    void  SetSfxVolume(float value) { _sfx = Clamp(value); }


private:
    float _master = 50.f;
    float _music = 100.f;
    float _sfx = 100.f;

    static float Clamp(float value) 
    {
        if (value < 0.f)   return 0.f;
        if (value > 100.f) return 100.f;
        return value;
    }
};
