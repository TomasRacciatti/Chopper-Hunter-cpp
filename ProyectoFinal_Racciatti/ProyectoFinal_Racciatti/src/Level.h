#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class ResourceManager;

class Level
{
public:
    Level() = default;
    Level(const sf::Vector2u& windowSize,
        ResourceManager& resourceManager,
        const std::string& bgPath);

    sf::Vector2u WindowSize() const { return _windowSize; }

    void Draw(sf::RenderTarget& rt) const;

    sf::FloatRect Bounds() const
    {
        return sf::FloatRect(sf::Vector2f{ 0.f, 0.f },
                             sf::Vector2f{ static_cast<float>(_windowSize.x),
                                           static_cast<float>(_windowSize.y) });
    }

    // Colisiones
    const std::vector<sf::FloatRect>& Colliders() const { return _colliders;  }
    const sf::FloatRect& FloorRect() const { return _floor; }

protected:

private:
    sf::Vector2u _windowSize{ 0,0 };
    
    // Data de Background
    const sf::Texture* _bgTex = nullptr;
    sf::Sprite _bgSprite;

    // Piso y paredes momentaneas
    std::vector<sf::RectangleShape> _tiles;

    void BuildTiles(const sf::Vector2u& window, unsigned tileSize = 64);
    void FitBackground();

    // Colisiones
    std::vector<sf::FloatRect> _colliders;
    sf::FloatRect _floor{};
};

