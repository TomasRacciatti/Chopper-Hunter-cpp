#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"

class CreditsPanel
{
public:
    CreditsPanel(ResourceManager& rm, sf::RenderWindow& window);
    ~CreditsPanel();

    void Open() { _open = true; }
    void Close() { _open = false; _backRequested = false; }
    bool IsOpen() const { return _open; }

    void HandleEvent(const sf::Event& ev);
    void Update(float dt);
    void Draw(sf::RenderTarget& rt) const;

    bool BackRequested() const { return _backRequested; }
    void ClearBackRequest() { _backRequested = false; }

private:
    sf::RenderWindow& _window;

    sf::Text text;

    sf::Text* _titleTxt = nullptr;
    sf::Text* _bodyTxt = nullptr; // Uno solo con multiples lineas
    sf::Text* _backTxt = nullptr;
    sf::Sprite* _backBtn = nullptr;
    sf::Sprite* background;

    bool  _open = false;
    bool  _backRequested = false;

    float _spacingY = 80.f;
    float _buttonScale = 0.5f;

    //Helpers
    void CenterText(sf::Text* t) const;
    void CenterSprite(sf::Sprite* s) const;
};

