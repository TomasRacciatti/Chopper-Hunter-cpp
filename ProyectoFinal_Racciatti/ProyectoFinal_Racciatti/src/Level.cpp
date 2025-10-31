#include "Level.h"
#include "ResourceManager.h"

Level::Level(const sf::Vector2u& windowSize, ResourceManager& resourceManager, const std::string& bgPath)
    : _windowSize(windowSize)
    , _bgTex(&resourceManager.GetTexture(bgPath, false, {}))
    , _bgSprite(*_bgTex)
{
    _bgTex = &resourceManager.GetTexture(bgPath, false, {});
    _bgSprite.setTexture(*_bgTex);
    FitBackground();

    _floorTex = &resourceManager.GetTexture("res/sprites/tiles/Ground.png", false, {});
    _wallTex = &resourceManager.GetTexture("res/sprites/tiles/Wall.png", false, {});

    const auto floorSize = _floorTex->getSize();
    const auto wallSize = _wallTex->getSize();
    _floorScale = 64.f / static_cast<float>(floorSize.x);
    _wallScale = 64.f / static_cast<float>(wallSize.x);

    BuildTiles(_windowSize, 64);
}

void Level::FitBackground()
{
    const auto texSize = _bgTex->getSize();
    if (!texSize.x || !texSize.y) return;
    const float sx = static_cast<float>(_windowSize.x) / static_cast<float>(texSize.x);
    const float sy = static_cast<float>(_windowSize.y) / static_cast<float>(texSize.y);
    _bgSprite.setScale(sf::Vector2f{ sx, sy });

    _bgSprite.setPosition({ 0.f, 0.f });
}

void Level::BuildTiles(const sf::Vector2u& win, unsigned tileSize)
{
    _tiles.clear();
    _colliders.clear();

    const float width = static_cast<float>(win.x);
    const float height = static_cast<float>(win.y);
    const float ts = static_cast<float>(tileSize);

    // ===== Piso =====
    const int floorColumns = width / ts;
    const float floorTop = height - ts;

    for (int i = 0; i < floorColumns; i++)
    {
        _tiles.emplace_back(*_floorTex);
        auto& spr = _tiles.back();
        spr.setScale(sf::Vector2f{ _floorScale, _floorScale });
        spr.setPosition(sf::Vector2f{ i * ts, floorTop });
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
        _tiles.emplace_back(*_wallTex);
        auto& spr = _tiles.back();
        spr.setScale(sf::Vector2f{ _wallScale, _wallScale });
        spr.setPosition(sf::Vector2f{ 0.f, floorTop - ts * (i + 1) });
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
        _tiles.emplace_back(*_wallTex);
        auto& spr = _tiles.back();
        spr.setScale(sf::Vector2f{ _wallScale, _wallScale });
        spr.setPosition(sf::Vector2f{ rightSide, floorTop - ts * (i + 1) });
    }
}

void Level::Draw(sf::RenderTarget& rt) const
{
    rt.draw(_bgSprite);
    for (size_t i = 0; i < _tiles.size(); ++i)
        rt.draw(_tiles[i]);
}