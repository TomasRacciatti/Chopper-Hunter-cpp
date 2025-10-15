#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "AudioSettings.h"
#include <functional>
#include <vector>

class OptionsPanel
{
public:
    OptionsPanel(ResourceManager& resourceManager, sf::RenderWindow& win, AudioSettings& audio);
    ~OptionsPanel();

    void Open() { _open = true; }
    void Close() { _open = false; _dragging = false; }
    bool IsOpen() const { return _open; }

    void HandleEvent(const sf::Event& ev);
    void Update(float dt);
    void Draw(sf::RenderTarget& renderTarget) const;

    // State
    bool BackRequested() const { return _backRequested; }
    void ClearBackRequest() { _backRequested = false; }

    float GetVolume() const { return _audio.GetMasterVolume(); }
    void  SetVolume(float value);

private:

    sf::RenderWindow& _window;

    // widgets
    sf::Text text;

    sf::Text* _title;
    sf::Text* _backTxt;
    sf::Sprite* _backBtn;
    sf::Sprite* background;

    struct Slider
    {
        sf::Text* _label = nullptr;
        sf::Text* _value = nullptr;
        sf::RectangleShape* _track = nullptr;
        sf::RectangleShape* _fill = nullptr;
        sf::CircleShape* _knob = nullptr;
        sf::FloatRect _trackRect;
        bool dragging = false;

        std::function<float()> getter; 
        std::function<void(float)> setter;
    };

    std::vector<Slider> _sliders;

    bool  _open = false;
    bool  _backRequested = false;
    //bool  _dragging = false;
    
    AudioSettings& _audio;

    float _buttonScale = 0.5f;
    float _spacingY = 100.f;

    // Helpers
    void CenterSprite(sf::Sprite* sprite) const;
    void CenterText(sf::Text* text) const;

    void CreateSliders(ResourceManager& rm);
    void LayoutSliders();
    void UpdateSliderVisual(Slider& slider);
    void SetSliderFromX(Slider& slider, float worldX);
};

