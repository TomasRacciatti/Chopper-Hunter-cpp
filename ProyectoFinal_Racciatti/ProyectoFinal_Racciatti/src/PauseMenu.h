#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "OptionsPanel.h"

class PauseMenu
{
public:
    PauseMenu(ResourceManager& resourceManager, sf::RenderWindow& window);
    ~PauseMenu();

    void Open() { _open = true; }
    void Close() { _open = false; }
    bool IsOpen() const { return _open; }

    void HandleEvent(const sf::Event& ev);
    void Update(float dt);
    void Draw(sf::RenderTarget& rt) const;

    bool ResumeRequested() const { return _resumeRequested; }
    bool MainMenuRequested() const { return _menuRequested; }
    void ClearRequests() { _resumeRequested = false; _menuRequested = false; }

private:
    sf::RenderWindow& _window;

    sf::Text text;

    sf::RectangleShape* _panel = nullptr;

    sf::Text* _titleTxt = nullptr;
    sf::Text* _resumeTxt = nullptr;
    sf::Text* _optionsTxt = nullptr;
    sf::Text* _toMenuTxt = nullptr;

    OptionsPanel* _optionsPanel = nullptr;

    // State
    bool _open = false;
    bool _resumeRequested = false;
    bool _menuRequested = false;

    // Layout
    float _spacingY = 100.f;

    // Helper
    void CenterText(sf::Text* t) const;
};

