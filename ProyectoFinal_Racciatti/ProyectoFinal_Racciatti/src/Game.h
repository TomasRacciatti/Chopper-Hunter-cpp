#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "SceneManager.h"

class Game
{
public:
	Game();
	~Game();

	void Run();


private:
	// Resources
	ResourceManager resourceManager;

	AudioSettings audio;

	// Window
	sf::RenderWindow _window;
	sf::View _view;

	SceneManager _sceneManager;

	// Time
	sf::Clock _clock;
};

