#pragma once
#include <memory>
#include "Scene.h"
#include "MainMenu.h"
#include "GameplayScene.h"

class SceneManager
{
public:
	SceneManager(sf::RenderWindow& win, ResourceManager& resources);
	void SetScene(SceneID id);

private:
	sf::RenderWindow& _window;
	ResourceManager& resourceManager;
	std::unique_ptr<Scene> _scene;
};

