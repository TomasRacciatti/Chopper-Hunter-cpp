#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Level
{
public:
    Level() = default;
    Level(const sf::Vector2u& windowSize,
        const std::string& bgPath = "Sprites/background.png");

    sf::Vector2u WindowSize() const { return _windowSize; }

    void Draw(sf::RenderTarget& rt) const;

    // Colisiones
    const std::vector<sf::FloatRect>& Colliders() const { return _colliders;  }
    const sf::FloatRect& FloorRect() const { return _floor; }

protected:

private:
    sf::Vector2u _windowSize{ 0,0 };
    
    // Data de Background
    sf::Texture _bgTex;
    sf::Sprite _bgSprite;
    bool _bgLoaded = false;

    // Piso y paredes momentaneas
    std::vector<sf::RectangleShape> _tiles;

    void BuildTiles(const sf::Vector2u& window, unsigned tileSize = 64);
    void FitBackground(const sf::Vector2u& window);

    // Colisiones
    std::vector<sf::FloatRect> _colliders;
    sf::FloatRect _floor{};
};

