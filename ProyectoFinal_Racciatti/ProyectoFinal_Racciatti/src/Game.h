#pragma once
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "Player.h"

class Game
{
public:
	Game();
	~Game();

	void Play();

protected:

private:
	enum class State {Menu, Play, Pause };

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

	//Pools
	size_t poolCapacity = 256;
	Pool<Bullet> _playerBulletPool
	{ 
		poolCapacity, [](Bullet& bullet)
		{
			bullet.SetRadius(3.f);
			bullet.SetColor(sf::Color(240,225,60)); 
		} 
	};

	Pool<Bullet> _enemyBulletPool
	{
		poolCapacity, [](Bullet& bullet)
		{
			bullet.SetRadius(3.f);
			bullet.SetColor(sf::Color(230,50,50));
		}
	};
};

