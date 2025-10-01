#pragma once
#include <memory>
#include "Scene.h"
#include "MainMenu.h"
#include "GameplayScene.h"

class SceneManager
{
public:
	SceneManager(sf::RenderWindow& win, ResourceManager& resources, AudioSettings& audio);
	void SetScene(SceneID id);

	Scene* Current() { return _scene.get(); }
	const Scene* Current() const { return _scene.get(); }

private:
	AudioSettings& _audio;
	sf::RenderWindow& _window;
	ResourceManager& resourceManager;
	std::unique_ptr<Scene> _scene;
};

