#include "Game.h"
#include <ctime>


Game::Game()
	: _window(sf::VideoMode({ 1280u, 720u }), "Chopper Hunter")
	, _sceneManager(_window, resourceManager)
{
	_window.setFramerateLimit(60);
	_view = _window.getDefaultView();
}
Game::~Game() = default;

void Game::Run()
{
	srand(time(nullptr));
	
	while (_window.isOpen())
	{
		float dt = _clock.restart().asSeconds();
		if (dt > 0.05f) 
			dt = 0.05f;

		while (auto ev = _window.pollEvent()) 
		{
			if (ev->is<sf::Event::Closed>())
			{
				_window.close();
				continue;
			}

			if (auto* scene = _sceneManager.Current())
				scene->HandleEvents(*ev);
		}

		if (auto* scene = _sceneManager.Current()) 
		{
			scene->Input();
			scene->Update(dt);

			_window.clear();
			_window.setView(_view);

			scene->Draw();
			_window.display();
		}
		else
		{
			_window.clear();
			_window.display();
		}
	}
}