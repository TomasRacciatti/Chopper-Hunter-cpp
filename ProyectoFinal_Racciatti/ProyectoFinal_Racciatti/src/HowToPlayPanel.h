#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <vector>
#include <string>

class HowToPlayPanel
{
public:
    HowToPlayPanel(ResourceManager& rm, sf::RenderWindow& window);
    ~HowToPlayPanel();

    void Open() { _open = true; }
    void Close() { _open = false; _backRequested = false; }
    bool IsOpen() const { return _open; }

    void HandleEvent(const sf::Event& ev);
    void Draw(sf::RenderTarget& rt) const;

    bool BackRequested() const { return _backRequested; }
    void ClearBackRequest() { _backRequested = false; }

private:
    struct IconLabel {
        sf::Sprite* icon = nullptr;
        sf::Text* text = nullptr;
        sf::Vector2f  pos;
    };

    sf::RenderWindow& _window;

    // State
    bool  _open = false;
    bool  _backRequested = false;
    int   _page = 0;
    const int _pageCount = 3;

    // Widgets
    sf::Sprite* _background = nullptr;
    sf::Text* _title = nullptr;
    sf::Sprite* _backBtn = nullptr;
    sf::Text* _backTxt = nullptr;
    sf::Sprite* _prevBtn = nullptr;
    sf::Text* _prevTxt = nullptr;
    sf::Sprite* _nextBtn = nullptr;
    sf::Text* _nextTxt = nullptr;

    // Style
    float _spacingY = 80.f;
    float _buttonScale = 0.5f;
    sf::Font* _titleFont = nullptr;
    sf::Font* _bodyFont = nullptr;

    // Pags
    std::vector<IconLabel> _controls;
    std::vector<IconLabel> _enemies; 
    std::vector<IconLabel> _weapons;

    // Paths
    std::string _heliPath = "../sprites/enemies/HeliSingle.png";
    std::string _dronePath = "../sprites/enemies/DroneSingle.png";
    std::string _artilleryPath = "../sprites/enemies/Missile.png";
    std::string _crateSgPath = "../sprites/player/ShotgunAmmo.png";
    std::string _crateHealthPath = "../sprites/player/HealthCrate.png";
    std::string _crateRpgPath = "../sprites/player/RPGAmmo.png";
    std::string _pistolPath = "../sprites/player/Pistol.png";
    std::string _shotgunPath = "../sprites/player/Shotgun.png";
    std::string _rpgPath = "../sprites/player/RPG.png";

    // Helpers
    void BuildCommonUI(ResourceManager& rm);
    void BuildControlsPage(ResourceManager& rm);
    void BuildEnemiesPage(ResourceManager& rm);
    void BuildWeaponsPage(ResourceManager& rm);

    sf::Sprite* MakeButton(ResourceManager& rm, float scale = 0.5f);
    sf::Text* MakeText(ResourceManager& rm, const std::string& s, unsigned size);
    sf::Sprite* MakeIcon(ResourceManager& rm, const std::string& path, float targetHeight);

    void DrawPage(sf::RenderTarget& rt, const std::vector<IconLabel>& elems) const;

    static void CenterSprite(sf::Sprite* sprite);
    static void CenterText(sf::Text* text);
};

