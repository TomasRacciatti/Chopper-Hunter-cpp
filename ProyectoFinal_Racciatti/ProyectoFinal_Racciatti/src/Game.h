#pragma once
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Helicopter.h"

class Game
{
public:
	Game();
	~Game();

	void Play();

protected:

private:
	enum class State {Menu, Play, Pause };

	// Resources
	ResourceManager resourceManager;

	// Window
	sf::RenderWindow _window;
	sf::View _view;
	State _state = State::Menu;

	// Time
	sf::Clock _clock;

	// Scenes
	Level _level;

	// Game Loop
	void HandleEvents();
	void Update(float dt);
	void Draw();

	// Helpers
	void EnterMenu();
	void BeginPlay();

	// Input
	Player::Input _playerInput;
	std::unique_ptr<Player> _player; // unique ptr me permite asegurarme que los punteros de player se borren si se borra el game

	// Enemies
	std::unique_ptr<Helicopter> _heli;

	// Spawners
	void CreatePlayer();
	void SpawnHelicopter();

	//Pools
	size_t poolCapacity = 256;
	Pool<Bullet> _playerBulletPool
	{ 
		poolCapacity, [](Bullet& bullet)
		{
			bullet.SetRadius(4.f);
			bullet.SetColor(sf::Color(240,225,60)); 
		} 
	};

	Pool<Bullet> _enemyBulletPool
	{
		poolCapacity, [](Bullet& bullet)
		{
			bullet.SetRadius(4.f);
			bullet.SetColor(sf::Color(230,50,50));
		}
	};
};

