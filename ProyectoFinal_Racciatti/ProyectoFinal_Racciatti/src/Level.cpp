#include "Level.h"

Level::Level(const sf::Vector2u& windowSize, const std::string& bgPath)
    : _bgTex{}              // En SFML 3 tenemos que construir la textura
    , _bgSprite{_bgTex}     // y construir el sprite con esa textura
{
	_bgLoaded = _bgTex.loadFromFile(bgPath);
    if (_bgLoaded)
        FitBackground(windowSize);

    BuildTiles(windowSize, 64);
}

void Level::FitBackground(const sf::Vector2u& window)
{
    auto size = _bgTex.getSize();
    if (size.x == 0 || size.y == 0) return;

    const float sx = static_cast<float>(window.x) / static_cast<float>(size.x);
    const float sy = static_cast<float>(window.y) / static_cast<float>(size.y);
    _bgSprite.setScale(sf::Vector2f{ sx, sy });
    _bgSprite.setPosition(sf::Vector2f{ 0.f, 0.f });
}

void Level::BuildTiles(const sf::Vector2u& win, unsigned tileSize)
{
    _tiles.clear();
    _colliders.clear();

    const float width = static_cast<float>(win.x);
    const float height = static_cast<float>(win.y);
    const float ts = static_cast<float>(tileSize);

    const sf::Color tileColor(85, 60, 30); // Esto es solo ahora porque no tengo sprite del piso y paredes
    const sf::Color outline(150, 150, 150); // Esto es solo para debuggear y ver como se arma el mapita

    // ===== Piso =====
    const int floorColumns = width / ts;
    const float floorTop = height - ts;

    for (int i = 0; i < floorColumns; i++)
    {
        sf::RectangleShape tile({ ts, ts });
        tile.setPosition(sf::Vector2f{ i * ts, floorTop });
        tile.setFillColor(tileColor);
        tile.setOutlineThickness(1.f);
        tile.setOutlineColor(outline);
        _tiles.push_back(tile);
    }

    _floor = sf::FloatRect{ sf::Vector2f{ 0.f, floorTop }, sf::Vector2f{ width, ts } };
    _colliders.push_back(_floor);


    // ===== Half Walls =====
    const float wallHeight = height / 4;
    const int wallRows = wallHeight / ts;

    // Pared izquierda

    // Colider
    sf::FloatRect leftWall{ sf::Vector2f{ 0.f, floorTop - wallRows * ts },
                            sf::Vector2f{ ts, wallRows * ts } };
    _colliders.push_back(leftWall);


    // Visual
    for (int i = 0; i < wallRows; i++)
    {
        sf::RectangleShape tile({ ts, ts });
        tile.setPosition(sf::Vector2f{0.f, floorTop - ts * (i + 1) });
        tile.setFillColor(tileColor);
        tile.setOutlineThickness(1.f);
        tile.setOutlineColor(outline);
        _tiles.push_back(tile);
    }

    // Pared derecha
    const float rightSide = width - ts;

    // Colider
    sf::FloatRect rightWall{ sf::Vector2f{ rightSide, floorTop - wallRows * ts },
                             sf::Vector2f{ ts, wallRows * ts } };
    _colliders.push_back(rightWall);

    // Visual
    for (int i = 0; i < wallRows; i++)
    {
        sf::RectangleShape tile({ ts, ts });
        tile.setPosition(sf::Vector2f{ rightSide, floorTop - ts * (i + 1) });
        tile.setFillColor(tileColor);
        tile.setOutlineThickness(1.f);
        tile.setOutlineColor(outline);
        _tiles.push_back(tile);
    }
}

void Level::Draw(sf::RenderTarget& rt) const
{
    if (_bgLoaded) rt.draw(_bgSprite);
    for (size_t i = 0; i < _tiles.size(); ++i)
        rt.draw(_tiles[i]);
}