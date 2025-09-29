#include "GameplayScene.h"
#include "Utils.h"

static constexpr const char* bgPath = "../sprites/backgrounds/Game_bg.png";

GameplayScene::GameplayScene(ResourceManager& resourceManager, sf::RenderWindow& _window)
    : Scene(_window)
    , resourceManager(resourceManager)
    , _level(_window.getSize(), resourceManager, bgPath)
{
    CreatePlayer();
    SpawnHelicopter();
}

GameplayScene::~GameplayScene() = default;

void GameplayScene::HandleEvents(const sf::Event& ev)
{
    if (ev.is<sf::Event::KeyPressed>()) {
        auto key = ev.getIf<sf::Event::KeyPressed>();
        if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = true;
        if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = true;
        if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = true;
    }
    else if (ev.is<sf::Event::KeyReleased>()) {
        auto key = ev.getIf<sf::Event::KeyReleased>();
        if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = false;
        if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = false;
        if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = false;
    }
    else if (ev.is<sf::Event::MouseButtonPressed>()) {
        auto mouse = ev.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left) _playerInput.fireHeld = true;
    }
    else if (ev.is<sf::Event::MouseButtonReleased>()) {
        auto mouse = ev.getIf<sf::Event::MouseButtonReleased>();
        if (mouse->button == sf::Mouse::Button::Left) _playerInput.fireHeld = false;
    }
}

void GameplayScene::Input()
{
    const auto mousePix = sf::Mouse::getPosition(_window);
    _playerInput.mouseWorld = _window.mapPixelToCoords(mousePix);
}

void GameplayScene::Update(float dt)
{
    if (_player) 
    {
        _player->SetInput(_playerInput);
        _player->Update(dt, _level);
    }

    if (_heli && _heli->IsAlive()) 
    {
        if (_player && _player->IsAlive())
            _heli->SetTarget(_player->Center());

        _heli->Update(dt, _level);
    }
    else 
    {
        SpawnHelicopter();
    }

    // ===== Combat =====
    // Bullet Player a Helicopteros
    if (_heli && _heli->IsAlive())
        Combat::ResolveHits(_playerBulletPool, _heli.get());

    // Bullet de Helis le pegan al Player
    if (_player && _player->IsAlive())
        Combat::ResolveHits(_enemyBulletPool, _player.get());
}

void GameplayScene::Draw()
{
    _level.Draw(_window);
    if (_player) _player->Draw(_window);
    if (_heli)   _heli->Draw(_window);
}



// ========== Helpers ==========

void GameplayScene::CreatePlayer()
{
    const auto window = _window.getSize();
    const sf::Vector2f spawnPos(window.x * 0.5f, window.y - 64.f);

    std::string path = "../sprites/player/SoldierSpriteSheet.png";
    _player = std::make_unique<Player>(spawnPos, path, resourceManager);

    // Spawn de arma
    auto pistol = std::make_unique<Pistol>(
        0.35f,						// Cooldown
        350.f,						// Bullet speed
        5.f,						// Bullet lifetime
        1,							// Bullet damage
        &_playerBulletPool
    );

    std::string pistolPath = "../sprites/player/Pistol.png";
    sf::Texture& pistolTex = resourceManager.GetTexture(pistolPath, false, {});
    pistol->SetVisualSprite(pistolTex, { 25.f, 26.5f }, 1.5f);
    pistol->SetMuzzleDistance(18.f);
    _player->EquipWeapon(std::move(pistol));
}


void GameplayScene::SpawnHelicopter()
{
    const auto win = _window.getSize();

    constexpr float kOffX = 80.f;
    const float minSpawnX = -kOffX;
    const float maxSpawnX = static_cast<float>(win.x) + kOffX;

    constexpr float kMinOffY = 80.f;
    constexpr float kMaxOffY = 220.f;
    const float spawnX = Utils::RandomFloat(minSpawnX, maxSpawnX);
    const float spawnY = -Utils::RandomFloat(kMinOffY, kMaxOffY);
    sf::Vector2f heliSpawn{ spawnX, spawnY };

    // Torreta
    auto turret = std::make_unique<Pistol>(
        0.65f,     // cooldown
        350.f,    // bullet speed
        5.f,      // bullet life
        1,        // damage
        &_enemyBulletPool);
    std::string turretPath = "../sprites/enemies/Turret.png";
    sf::Texture& turretTex = resourceManager.GetTexture(turretPath, false, {});
    turret->SetVisualSprite(turretTex, { 25.f, 26.5f }, 1.0f);
    turret->SetMuzzleDistance(25.f);

    std::string path = "../sprites/enemies/HelicopterSpriteSheet.png";
    _heli = std::make_unique<Helicopter>(heliSpawn, std::move(turret), resourceManager, path);
}
