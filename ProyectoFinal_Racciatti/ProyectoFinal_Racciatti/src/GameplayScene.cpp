#include "GameplayScene.h"
#include "Utils.h"

static constexpr const char* bgPath = "../sprites/backgrounds/Game_bg.png";

GameplayScene::GameplayScene(ResourceManager& resourceManager, sf::RenderWindow& _window, AudioSettings& audio)
    : Scene(_window)
    , resourceManager(resourceManager)
    , _audio(audio)
    , _level(_window.getSize(), resourceManager, bgPath)
{
    CreatePlayer();
    SpawnHelicopter();

    _pause = new PauseMenu(resourceManager, _window, _audio);

    _droneSpawnTimer = Utils::RandomFloat(_droneSpawnMin, _droneSpawnMax);

    // ============== Music ===================
    std::string musicPath = "../audio/music/GameplayMusic.mp3";

    music.openFromFile(musicPath);

    music.setLooping(true);
    music.setVolume(_audio.GetMusicVolume());
    music.play();
}

GameplayScene::~GameplayScene()
{
    delete _pause;
}

void GameplayScene::HandleEvents(const sf::Event& ev)
{
    if (ev.is<sf::Event::KeyPressed>()) {
        auto key = ev.getIf<sf::Event::KeyPressed>();
        if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = true;
        if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = true;
        if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = true;

        if (key->scancode == sf::Keyboard::Scancode::Escape)
        {
            if (_pause && _pause->IsOpen()) 
                _pause->Close();
            else if (_pause)                 
                _pause->Open();
            return;
        }
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

    if (_pause && _pause->IsOpen())
    {
        _pause->HandleEvent(ev);
        return;
    }
}

void GameplayScene::Input()
{
    const auto mousePix = sf::Mouse::getPosition(_window);
    _playerInput.mouseWorld = _window.mapPixelToCoords(mousePix);
}

void GameplayScene::Update(float dt)
{
    // === Musica ===
    if (music.getStatus() == sf::SoundSource::Status::Stopped)
        music.play();

    music.setVolume(_audio.GetMusicVolume());


    // Pause
    if (_pause && _pause->IsOpen())
    {
        _pause->Update(dt);

        if (_pause->ResumeRequested())
        {
            _pause->ClearRequests();
            _pause->Close();
        }

        if (_pause->MainMenuRequested())
        {
            _pause->ClearRequests();
            wantsChange = true;
            nextSceneID = SceneID::MainMenu;
        }

        return;
    }

    // Player
    if (_player)
    {
        _player->SetInput(_playerInput);
        _player->Update(dt, _level);

        if (!_player->IsAlive())
        {
            wantsChange = true;
            nextSceneID = SceneID::MainMenu;
            return;
        }
    }

    // Heli
    if (_heli && (_heli->IsAlive() || _heli->IsDying())) 
    {
        if (_player && _player->IsAlive())
            _heli->SetTarget(_player->Center());

        _heli->Update(dt, _level);
    }
    else if (_heli && !_heli->IsAlive() && !_heli->IsDying())
    {
        SpawnHelicopter();
    }

    // Drones
    if (_drones.size() < static_cast<size_t>(_maxDronesOnScene)) 
    {
        _droneSpawnTimer -= dt;
        if (_droneSpawnTimer <= 0.f) 
        {
            SpawnDrone();
            _droneSpawnTimer = Utils::RandomFloat(_droneSpawnMin, _droneSpawnMax);
        }
    }

    for (auto& drone : _drones)
    {
        if (_player && _player->IsAlive())
        {
            drone->SetTarget(_player->Center());
            drone->SetAoETarget(_player.get());
        }

        drone->Update(dt, _level);
    }


    // ===== Combat =====
    // Bullet Player a Helicopteros
    if (_heli && _heli->IsAlive())
        Combat::ResolveHits(_playerBulletPool, _heli.get());

    // Drones
    for (auto& drone : _drones)
    {
        // Bullet Player a drones
        if (drone->IsAlive())
            Combat::ResolveHits(_playerBulletPool, drone.get());

        // Drones le pegan al player
        if (drone->IsAlive() && _player && _player->IsAlive() &&
            Utils::RectIntersects(_player->GetBounds(), drone->GetBounds()))
        {
            // Esto esta hardcodeado y corre peligro si quisieramos cambiarle la vida al drone
            // Exagero en el valor para asegurarme que siempre muera de una
            drone->TakeDamage(100); // Matar al drone lo hace explotar. 
        }
    }

    // Bullet de Helis le pegan al Player
    if (_player && _player->IsAlive())
        Combat::ResolveHits(_enemyBulletPool, _player.get());
}

void GameplayScene::Draw()
{
    _level.Draw(_window);
    if (_player) _player->Draw(_window);
    if (_heli)   _heli->Draw(_window);
    for (const auto& d : _drones)
        if (d) d->Draw(_window);

    if (_pause && _pause->IsOpen())
    {
        _pause->Draw(_window);
    }
}



// ========== Helpers ==========

void GameplayScene::CreatePlayer()
{
    const auto window = _window.getSize();
    const sf::Vector2f spawnPos(window.x * 0.5f, window.y - 64.f);

    std::string path = "../sprites/player/SoldierSpriteSheet.png";
    _player = std::make_unique<Player>(spawnPos, path, _audio, resourceManager);

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

    constexpr float offsetX = 80.f;
    const float minSpawnX = -offsetX;
    const float maxSpawnX = static_cast<float>(win.x) + offsetX;

    constexpr float minOffY = 80.f;
    constexpr float maxOffY = 220.f;
    const float spawnX = Utils::RandomFloat(minSpawnX, maxSpawnX);
    const float spawnY = -Utils::RandomFloat(minOffY, maxOffY);
    sf::Vector2f heliSpawn{ spawnX, spawnY };

    // Torreta
    auto turret = std::make_unique<Pistol>(
        0.5f,     // cooldown
        350.f,    // bullet speed
        5.f,      // bullet life
        1,        // damage
        &_enemyBulletPool);
    std::string turretPath = "../sprites/enemies/Turret.png";
    sf::Texture& turretTex = resourceManager.GetTexture(turretPath, false, {});
    turret->SetVisualSprite(turretTex, { 25.f, 26.5f }, 1.0f);
    turret->SetMuzzleDistance(25.f);

    std::string path = "../sprites/enemies/HelicopterSpriteSheet_Damaged.png";
    _heli = std::make_unique<Helicopter>(heliSpawn, std::move(turret), _audio, resourceManager, path);
}

void GameplayScene::SpawnDrone()
{
    if (!_player || !_player->IsAlive()) return;

    const auto win = _window.getSize();
    const auto floor = _level.FloorRect();
    const float floorY = floor.position.y;

    // Parametros visuales
    const sf::Vector2i spriteFrame{ 40, 29 };
    const float scale = 1.0f;
    const float halfH = spriteFrame.y * scale * 0.5f;
    const float halfW = spriteFrame.x * scale * 0.5f;

    // Margenes para el spawn
    constexpr float offX = 40.f;
    constexpr float offY = 40.f;
    constexpr float groundSafety = 32.f;

    // Spawns en los costados
    const float minY = halfH;
    const float maxY = floorY - groundSafety - halfH;

    // Spawn de arriba
    const float minX = halfW;
    const float maxX = win.x - halfW;

    float x = 0.f, y = 0.f;
    const int edge = Utils::RandomInt(0, 2); // 0 = izq, 1 = der, 2 = arriba

    if (edge == 0)
    {
        x = -offX;
        y = Utils::RandomFloat(minY, maxY);
    }
    else if (edge == 1)
    {
        x = win.x + offX;
        y = Utils::RandomFloat(minY, maxY);
    }
    else
    {
        y = -offY;
        x = Utils::RandomFloat(minX, maxX);
    }

    const std::string droneSpritePath = "../sprites/enemies/DroneSpriteSheet.png";

    auto drone = std::make_unique<Drone>(
        sf::Vector2f{ x, y },
        _audio,
        resourceManager,
        droneSpritePath,
        spriteFrame,
        scale
    );

    drone->SetTarget(_player->Center());
    drone->SetAoETarget(_player.get());
    _drones.emplace_back(std::move(drone));
}
