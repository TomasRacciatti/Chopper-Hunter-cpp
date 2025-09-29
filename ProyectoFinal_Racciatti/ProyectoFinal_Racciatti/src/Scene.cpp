#include "Scene.h"

Scene::Scene(sf::RenderWindow& window) 
	: _window(window)
{
	wantsChange = false;
}

Scene::~Scene() {}

bool Scene::GetWantsChange()
{
	return wantsChange;
}

void Scene::SetWantsChange(bool value)
{
	wantsChange = value;
}

SceneID Scene::GetNextSceneID()
{
	return nextSceneID;
}