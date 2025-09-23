#include "Game.h"
#include "Pistol.h"
#include "Combat.h"


Game::Game()
	: _window(sf::VideoMode({ 1280u, 720u }), "Chopper Hunter")
	, _level(_window.getSize(), "Sprites/background.png")
{
	_window.setFramerateLimit(60);
	_view = _window.getDefaultView();

	EnterMenu();
}
Game::~Game() = default;

void Game::Play()
{
	while (_window.isOpen())
	{
		HandleEvents();

		float dt = _clock.restart().asSeconds();
		if (dt > 0.05f)
			dt = 0.05f;

		Update(dt);
		Draw();
	}
}

void Game::HandleEvents()
{
	while (auto ev = _window.pollEvent())
	{
		if (ev->is<sf::Event::Closed>())
			_window.close();

		if (ev->is<sf::Event::KeyPressed>())
		{
			auto key = ev->getIf<sf::Event::KeyPressed>();
			if (key->scancode == sf::Keyboard::Scancode::Escape) {
				if (_state == State::Play) _state = State::Pause;
				else _window.close();
			}
			else if (_state == State::Menu && key->scancode == sf::Keyboard::Scancode::Enter) {
				BeginPlay();
			}

			// Movement
			if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = true;
			if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = true;
			if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = true;
		}

		if (ev->is<sf::Event::KeyReleased>())
		{
			auto key = ev->getIf<sf::Event::KeyReleased>();
			if (key->scancode == sf::Keyboard::Scancode::A) _playerInput.left = false;
			if (key->scancode == sf::Keyboard::Scancode::D) _playerInput.right = false;
			if (key->scancode == sf::Keyboard::Scancode::S) _playerInput.crouch = false;
		}

		if (ev->is<sf::Event::MouseButtonPressed>())
		{
			auto mouseButton = ev->getIf<sf::Event::MouseButtonPressed>();
			if (mouseButton->button == sf::Mouse::Button::Left) _playerInput.fireHeld = true;
		}

		if (ev->is<sf::Event::MouseButtonReleased>())
		{
			auto mouseButton = ev->getIf<sf::Event::MouseButtonReleased>();
			if (mouseButton->button == sf::Mouse::Button::Left) _playerInput.fireHeld = false;
		}
	}
}

void Game::Update(float dt)
{
	if (_state != State::Play) return;

	_window.setView(_view);

	auto mousePos = sf::Mouse::getPosition(_window);

	_playerInput.mouseWorld = _window.mapPixelToCoords(mousePos);

	if (_player)
	{
		_player->SetInput(_playerInput);
		_player->Update(dt, _level);
	}

	// Helis
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


	// Colision Bullet con Entity

	// Bullet Player a Helicopteros
	if (_heli && _heli->IsAlive()) 
		Combat::ResolveHits(_playerBulletPool, _heli.get());

	// Bullet de Helis le pegan al Player
	if (_player && _player->IsAlive())
		Combat::ResolveHits(_enemyBulletPool, _player.get());
}

void Game::Draw()
{
	_window.clear();
	_window.setView(_view);

	_level.Draw(_window);

	if (_player) _player->Draw(_window);
	if (_heli) _heli->Draw(_window);

	_window.display();
}

void Game::EnterMenu()
{
	_state = State::Menu;
	_view = _window.getDefaultView();
	_window.setView(_view);
}

void Game::BeginPlay()
{
	_state = State::Play;
	_view = _window.getDefaultView();
	_window.setView(_view);

	//Spawn player
	sf::Vector2f spawnPos(_window.getSize().x * 0.5f, _window.getSize().y - 64.f);
	_player = std::make_unique<Player>(spawnPos);

	// Spawn de arma
	_player->EquipWeapon(std::make_unique<Pistol>(
		0.4f,						// Cooldown
		300.f,						// Bullet speed
		5.f,						// Bullet lifetime
		1,							// Bullet damage
		&_playerBulletPool
	));

	// Spawn de Heli
	SpawnHelicopter();
}


// Helper para no sobrecargar mucho BeginPlay

void Game::SpawnHelicopter() {
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
		0.7f,     // cooldown
		300.f,    // bullet speed
		5.f,      // bullet life
		1,        // damage
		&_enemyBulletPool);

	_heli = std::make_unique<Helicopter>(heliSpawn, std::move(turret));
}
