#include "GameplayScene.h"
#include "Utils.h"

// Templates para la creacion de los unique_ptr que uso
namespace {
	template <typename T, typename... Args>
	std::unique_ptr<T> Make(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename SetupFn, typename... Args>
	std::unique_ptr<T> MakeWith(SetupFn&& setup, Args&&... args) {
		auto obj = std::make_unique<T>(std::forward<Args>(args)...);
		std::forward<SetupFn>(setup)(*obj);
		return obj;
	}
}

static constexpr const char* bgPath = "res/sprites/backgrounds/Game_bg.png";

GameplayScene::GameplayScene(ResourceManager& resourceManager, sf::RenderWindow& _window, AudioSettings& audio)
	: Scene(_window)
	, resourceManager(resourceManager)
	, _audio(audio)
	, _level(_window.getSize(), resourceManager, bgPath)
	, _cratePickupSfx(resourceManager.GetSound("res/audio/sfx/CratePickup.mp3"))
{
	_progress.Reset();

	CreatePlayer();
	SpawnHelicopter();

	_pause = new PauseMenu(resourceManager, _window, _audio);

	_droneSpawnTimer = Utils::RandomFloat(_droneSpawnMin, _droneSpawnMax);
	_artillerySpawnTimer = Utils::RandomFloat(_artillerySpawnMin, _artillerySpawnMax);

	// ============== Music ===================
	std::string musicPath = "res/audio/music/GameplayMusic.mp3";

	music.openFromFile(musicPath);

	music.setLooping(true);
	music.setVolume(_audio.GetMusicVolume());
	music.play();

	// Drop sfx
	_cratePickupSfx.setLooping(false);

	// UI
	_gameplayUI = new GameplayUI(resourceManager, _window.getSize());
	_gameplayUI->SetProgress(&_progress);
	_gameplayUI->SetPlayer(_player.get());

	// Highscore
	_scores.Load();
	_death = new DeathPanel(resourceManager, _window);
}

GameplayScene::~GameplayScene()
{
	delete _pause;
	delete _gameplayUI;
	delete _death;
}

void GameplayScene::HandleEvents(const sf::Event& ev)
{
	if (_death && _death->IsOpen())
	{
		_death->HandleEvent(ev);
		return;
	}

	if (ev.is<sf::Event::KeyPressed>()) {
		auto key = ev.getIf<sf::Event::KeyPressed>();
		if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = true;
		if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = true;
		if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = true;

		if (key->scancode == sf::Keyboard::Scancode::Num1) { if (_player) _player->SelectWeapon(0); }
		if (key->scancode == sf::Keyboard::Scancode::Num2) { if (_player) _player->SelectWeapon(1); }
		if (key->scancode == sf::Keyboard::Scancode::Num3) { if (_player) _player->SelectWeapon(2); }

		if (key->scancode == sf::Keyboard::Scancode::Escape)
		{
			if (_pause && _pause->IsOpen())
			{
				_audio.SetMusicController(1.f);
				_audio.SetSfxController(1.f);
				_pause->Close();
			}
			else if (_pause)
			{
				_pause->Open();
				_audio.SetMusicController(0.5f);
				_audio.SetSfxController(0.f);
				_freezeOneFrame = true;
			}
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

	// Muerte
	if (_death && _death->IsOpen())
	{
		if (_freezeOneFrame)
		{
			const float dt0 = 0.f;
			
			if (_player)_player->Update(dt0, _level);
			if (_heli) _heli->Update(dt0, _level);
			for (auto& drone : _drones) drone->Update(dt0, _level);
			for (auto& arti : _artilleryRounds) arti->Update(dt0, _level);

			_freezeOneFrame = false;
		}
		else
		{
			_death->Update(dt);

			if (_death->WantsReturnToMenu())
			{
				_audio.SetMusicController(1.f);
				_audio.SetSfxController(1.f);

				wantsChange = true;
				nextSceneID = SceneID::MainMenu;
				return;
			}
		}
		return;
	}


	// Pause
	if (_pause && _pause->IsOpen())
	{
		if (_freezeOneFrame)
		{
			const float dt0 = 0.f;

			if (_player)_player->Update(dt0, _level);
			if (_heli) _heli->Update(dt0, _level);
			for (auto& drone : _drones) drone->Update(dt0, _level);
			for (auto& arti : _artilleryRounds) arti->Update(dt0, _level);

			_freezeOneFrame = false;
		}
		else
		{
			_pause->Update(dt);

			if (_pause->ResumeRequested())
			{
				_audio.SetMusicController(1.f);
				_audio.SetSfxController(1.f);

				_pause->ClearRequests();
				_pause->Close();
			}

			if (_pause->MainMenuRequested())
			{
				_audio.SetMusicController(1.f);
				_audio.SetSfxController(1.f);

				_pause->ClearRequests();
				wantsChange = true;
				nextSceneID = SceneID::MainMenu;
			}
		}
		return;
	}

	// Progreso
	_progress.Update(dt);
	if (_gameplayUI) _gameplayUI->Update(dt);
	CheckAdvanceDifficulty();

	// Player
	if (_player)
	{
		_player->SetInput(_playerInput);
		_player->Update(dt, _level);

		if (!_player->IsAlive())
		{
			if (_death && !_death->IsOpen())
			{
				// Cambio de musica
				music.stop();
				std::string deathMusicPath = "res/audio/music/DeathMusic.mp3";
				music.openFromFile(deathMusicPath);

				music.setLooping(true);
				music.setVolume(_audio.GetMusicVolume());
				music.play();

				_audio.SetSfxController(0.f);
				_freezeOneFrame = true;

				_death->Open(_progress.GetScore(), _progress.GetTimeElapsed(), &_scores);
			}
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
		const sf::Vector2f dropPos = _heli->Center();
		TryDropCrate(dropPos);
		
		SpawnHelicopter();
	}

	// Drones
	const size_t aliveDrones = std::count_if(
		_drones.begin(), _drones.end(),
		[](const std::unique_ptr<Drone>& drone) { return drone && drone->IsAlive(); }
	);

	if (aliveDrones < static_cast<size_t>(_maxDronesOnScene))
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

	// Arti
	const size_t aliveArtillery = std::count_if(
		_artilleryRounds.begin(), _artilleryRounds.end(),
		[](const std::unique_ptr<Artillery>& a) { return a && a->IsAlive(); }
	);

	if (aliveArtillery < static_cast<size_t>(_maxArtilleryOnScene))
	{
		_artillerySpawnTimer -= dt;
		if (_artillerySpawnTimer <= 0.f)
		{
			SpawnArtillery();
			_artillerySpawnTimer = Utils::RandomFloat(_artillerySpawnMin, _artillerySpawnMax);
		}
	}

	for (auto& artiRound : _artilleryRounds)
	{
		if (!artiRound) continue;
		artiRound->Update(dt, _level);
	}

	_artilleryRounds.erase(std::remove_if(_artilleryRounds.begin(), _artilleryRounds.end(),
		[](const std::unique_ptr<Artillery>& a) { return !a || !a->IsAlive(); }),
		_artilleryRounds.end());

	// Drops
	for (auto& crate : _crates)
	{
		if (!crate) continue;
		crate->Update(dt, _level);

		if (_player && _player->IsAlive() && crate->Alive())
		{
			if (Utils::RectIntersects(_player->GetBounds(), crate->Bounds()))
			{
				switch (crate->Type())
				{
				case CrateType::Health:
					_player->Heal(crate->HealAmount());
					break;

				case CrateType::ShotgunAmmo:
					if (auto shotgun = _player->FindWeaponOfType<Shotgun>())
						shotgun->AddAmmo(crate->ShotgunAmount());
					break;

				case CrateType::RpgAmmo:
					if (auto rpg = _player->FindWeaponOfType<Rpg>())
						rpg->AddAmmo(crate->RpgAmount());
					break;
				}
				crate->Kill();

				_cratePickupSfx.setVolume(_audio.GetSfxVolume());
				_cratePickupSfx.play();
			}
		}
	}

	// ===== Combat =====
	// Bullet Player a Helicopteros
	if (_heli && _heli->IsAlive())
		Combat::ResolveHits(_playerBulletPool, _heli.get());

	// Drones
	for (const auto& drone : _drones)
	{
		// Bullet Player a drones
		if (!drone) continue;
		if (!drone->IsAlive()) continue;

		Combat::ResolveHits(_playerBulletPool, drone.get());

		// Drones le pegan al player
		if (drone->IsAlive() && !drone->IsExploding() && Utils::RectIntersects(_player->GetBounds(), drone->GetBounds()))
		{
			// Esto esta hardcodeado y corre peligro si quisieramos cambiarle la vida al drone
			// Exagero en el valor para asegurarme que siempre muera de una
			drone->TakeDamage(100); // Matar al drone lo hace explotar. 
		}
	}

	// Elimino de la lista los drones muertos
	_drones.erase(std::remove_if(_drones.begin(), _drones.end(),
		[](const std::unique_ptr<Drone>& drone) { return !drone || !drone->IsAlive(); }),
		_drones.end());

	// Bullet de Helis le pegan al Player
	if (_player && _player->IsAlive())
		Combat::ResolveHits(_enemyBulletPool, _player.get());
}

void GameplayScene::Draw()
{
	_level.Draw(_window);

	// Entities
	if (_player) _player->Draw(_window);
	if (_heli)   _heli->Draw(_window);
	for (const auto& drone : _drones)
		if (drone) drone->Draw(_window);
	for (const auto& artiRound : _artilleryRounds)
		if (artiRound) artiRound->Draw(_window);

	// Drops
	for (const auto& crates : _crates)
		if (crates) 
			crates->Draw(_window);

	// UI
	if (_gameplayUI) _gameplayUI->Draw(_window);

	// Pause
	if (_pause && _pause->IsOpen())
		_pause->Draw(_window);

	// Death
	if (_death && _death->IsOpen())
		_death->Draw(_window);
}



// ========== Helpers ==========

void GameplayScene::TryDropCrate(sf::Vector2f pos)
{
	if (Utils::RandomInt(0, 99) >= _dropChance) return;

	const int rand = Utils::RandomInt(0, 99);
	CrateType type;

	if (rand < _hpChance)
		type = CrateType::Health;
	else if (rand < _hpChance + _rpgChance)
		type = CrateType::RpgAmmo;
	else
		type = CrateType::ShotgunAmmo;

	std::string path;
	if (type == CrateType::RpgAmmo)   
		path = "res/sprites/player/RPGAmmo.png";
	else if (type == CrateType::Health) 
		path = "res/sprites/player/HealthCrate.png";
	else if (type == CrateType::ShotgunAmmo) 
		path = "res/sprites/player/ShotgunAmmo.png";

	sf::Texture& tex = resourceManager.GetTexture(path, false, {});
	const float scale = 1.25f;
	_crates.emplace_back(Make<Crate>(pos, type, tex, scale));
}

void GameplayScene::CreatePlayer()
{
	const auto window = _window.getSize();
	const sf::Vector2f spawnPos(window.x * 0.5f, window.y - 64.f);

	std::string path = "res/sprites/player/SoldierSpriteSheet.png";
	_player = Make<Player>(spawnPos, path, _audio, resourceManager);

	// Spawn de arma

	// Pistol
	float pistolCooldown = 0.35f;
	float pBulletSpeed = 350.f;
	float pBullLifetime = 5.f;
	int pBullDamage = 1;
	auto pistolSfx = std::make_unique<sf::Sound>(resourceManager.GetSound("res/audio/sfx/gunshot.mp3"));

	auto pistol = MakeWith<Pistol>(
		[&](Pistol& w) {
			std::string pistolPath = "res/sprites/player/Pistol.png";
			sf::Texture& pistolTex = resourceManager.GetTexture(pistolPath, false, {});
			w.SetVisualSprite(pistolTex, { 25.f, 26.5f }, 1.5f);
			w.SetMuzzleDistance(18.f);
		},
		pistolCooldown, pBulletSpeed, pBullLifetime, pBullDamage, 
		&_playerBulletPool, _audio, std::move(pistolSfx)
	);

	_player->EquipWeapon(std::move(pistol));

	// Shotgun
	float sgCooldown = 1.9f;
	float sgBulletSpeed = 550.f;
	float sgBullLifetime = 5.f;
	int sgBullDamage = 1;
	int sgStartingAmmo = 4;
	auto shotgunSfx = std::make_unique<sf::Sound>(resourceManager.GetSound("res/audio/sfx/shotgun.mp3"));

	auto shotgun = MakeWith<Shotgun>(
		[&](Shotgun& w) {
			std::string shotgunPath = "res/sprites/player/Shotgun.png";
			sf::Texture& shotgunTex = resourceManager.GetTexture(shotgunPath, false, {});
			w.SetVisualSprite(shotgunTex, { 25.f, 26.5f }, 1.5f);
			w.SetMuzzleDistance(25.f);

			w.SetAmmo(sgStartingAmmo);
		},
		sgCooldown, sgBulletSpeed, sgBullLifetime, sgBullDamage,
		&_playerBulletPool, _audio, std::move(shotgunSfx)
	);

	_player->EquipWeapon(std::move(shotgun));

	// RPG
	const float rpgCooldown = 5.f;  
	const float rpgSpeed = 300.f;
	const float rpgLifetime = 5.f;  
	const int rpgDamage = 8;
	int rpgStartingAmmo = 2;
	auto rpgSfx = std::make_unique<sf::Sound>(resourceManager.GetSound("res/audio/sfx/rpg.mp3"));

	auto rpg = MakeWith<Rpg>(
		[&](Rpg& w) {
			std::string rpgPath = "res/sprites/player/RPG.png";
			sf::Texture& rpgTex = resourceManager.GetTexture(rpgPath, false, {});
			w.SetVisualSprite(rpgTex, { 25.f, 26.5f }, 1.5f);
			w.SetMuzzleDistance(25.f);

			// Textura distinta para la bala
			const std::string missilePath = "res/sprites/player/RpgRound.png";
			sf::Texture& missileTex = resourceManager.GetTexture(missilePath, false, {});
			w.SetMissileSprite(missileTex, { 9.f, 5.5f }, 1.5f);

			w.SetupExplosion(resourceManager);

			w.SetAmmo(rpgStartingAmmo);
		},
		rpgCooldown, rpgSpeed, rpgLifetime, rpgDamage,
		&_playerBulletPool, _audio, std::move(rpgSfx)
	);

	_player->EquipWeapon(std::move(rpg));
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
	float turretCooldown = 0.5f;
	float tBulletSpeed = 350.f;
	float tBullLifetime = 5.f;
	int tBullDamage = 1;
	auto turretSfx = std::make_unique<sf::Sound>(resourceManager.GetSound("res/audio/sfx/turret.mp3"));

	auto turret = MakeWith<Pistol>(
		[&](Pistol& w) {
			std::string turretPath = "res/sprites/enemies/Turret.png";
			sf::Texture& turretTex = resourceManager.GetTexture(turretPath, false, {});
			w.SetVisualSprite(turretTex, { 25.f, 26.5f }, 1.0f);
			w.SetMuzzleDistance(25.f);
		},
		turretCooldown, tBulletSpeed, tBullLifetime, tBullDamage, 
		&_enemyBulletPool, _audio, std::move(turretSfx)
	);

	std::string path = "res/sprites/enemies/HelicopterSpriteSheet_Damaged.png";
	_heli = Make<Helicopter>(heliSpawn, std::move(turret), _audio, resourceManager, path);

	_heli->SetProgress(&_progress);
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

	const std::string droneSpritePath = "res/sprites/enemies/DroneSpriteSheet.png";

	auto drone = Make<Drone>(
		sf::Vector2f{ x, y },
		_audio,
		resourceManager,
		droneSpritePath,
		spriteFrame,
		scale
	);

	drone->SetProgress(&_progress);

	drone->SetTarget(_player->Center());
	drone->SetAoETarget(_player.get());
	_drones.emplace_back(std::move(drone));
}

void GameplayScene::SpawnArtillery()
{
	if (!_player || !_player->IsAlive()) return;

	const auto win = _level.WindowSize();
	constexpr float offY = 64.f; // Va a aparecer 64 pixeles arriba del techo
	constexpr float sideMargin = 96.f; // Este valor es para evitar que caigan en las paredes de los costados

	const float minX = sideMargin;
	const float maxX = win.x - sideMargin;

	const float spawnX = Utils::RandomFloat(minX, maxX);
	const float spawnY = -offY;

	const std::string spritePath = "res/sprites/enemies/Missile.png";

	const sf::Vector2i frame{ 64, 64 };
	const float scale = 1.0f;

	auto artiRound = Make<Artillery>(
		sf::Vector2f{ spawnX, spawnY },
		_audio,
		resourceManager,
		spritePath,
		frame,
		scale
	);

	artiRound->SetProgress(&_progress);

	artiRound->SetAoETarget(_player.get());
	_artilleryRounds.emplace_back(std::move(artiRound));
}


// ======= Cambios de dificultad ========
void GameplayScene::CheckAdvanceDifficulty()
{
	while (_difficultyStage < 5 && _progress.GetScore() >= _nextDifficultyScore)
	{
		ApplyDifficultyStep();
		_difficultyStage++;

		_difficultyIncrement += _incrementAddition;
		_nextDifficultyScore += _difficultyIncrement;
	}
}

void GameplayScene::ApplyDifficultyStep()
{
	switch (_difficultyStage)
	{
		case 0: // Cae el cooldown de arti y drones por 5 secs
		{
			_droneSpawnMin -= 5.f;
			_droneSpawnMax -= 5.f;
			_artillerySpawnMin -= 5.f;
			_artillerySpawnMax -= 5.f;
			ClampSpawnWindows();
		} break;

		case 1: // Drones y arti spawnean hasta 2
		{
			_maxDronesOnScene += 1;
			_maxArtilleryOnScene += 1;
		} break;

		case 2: // Cae el cooldown de arti y drones por 5 secs
		{
			_droneSpawnMin -= 5.f;
			_droneSpawnMax -= 5.f;
			_artillerySpawnMin -= 5.f;
			_artillerySpawnMax -= 5.f;
			ClampSpawnWindows();
		} break;

		case 3: // Drones spawnean hasta 3
		{
			_maxDronesOnScene += 1;
		} break;

		case 4:
		{
			_droneSpawnMax -= 2.5f;
			_artillerySpawnMin -= 5.f;
			_artillerySpawnMax -= 5.f;
			ClampSpawnWindows();
		} break;
	}
}

void GameplayScene::ClampSpawnWindows()
{
	// Drones
	if (_droneSpawnMin < _spawnMinClamp) 
		_droneSpawnMin = _spawnMinClamp;

	if (_droneSpawnMax < _droneSpawnMin + 0.5f) 
		_droneSpawnMax = _droneSpawnMin + 0.5f;

	// Artillery
	if (_artillerySpawnMin < _spawnMinClamp) 
		_artillerySpawnMin = _spawnMinClamp;

	if (_artillerySpawnMax < _artillerySpawnMin + 0.5f) 
		_artillerySpawnMax = _artillerySpawnMin + 0.5f;
}