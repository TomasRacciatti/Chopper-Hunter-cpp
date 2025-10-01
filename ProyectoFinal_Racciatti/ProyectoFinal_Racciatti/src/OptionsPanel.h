#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"

class OptionsPanel
{
public:
    OptionsPanel(ResourceManager& resourceManager, sf::RenderWindow& win);
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

    float GetVolume() const { return _volume; }
    void  SetVolume(float value);

private:

    sf::RenderWindow& _window;

    // widgets
    sf::Text text;

    sf::Text* _title;
    sf::Text* _label;
    sf::Text* _value;
    sf::Text* _backTxt;
    sf::Sprite* _backBtn;
    sf::Sprite* background;

    sf::RectangleShape* _track;
    sf::RectangleShape* _fill;
    sf::CircleShape*    _knob;

    sf::FloatRect _trackRect;
    bool  _open = false;
    bool  _backRequested = false;
    bool  _dragging = false;
    float _volume = 100.f;

    float _buttonScale = 0.5f;
    float _spacingY = 100.f;

    // Helpers
    void CenterSprite(sf::Sprite* sprite) const;
    void CenterText(sf::Text* text) const;
    void SetVolumeFromX(float worldX);
};

