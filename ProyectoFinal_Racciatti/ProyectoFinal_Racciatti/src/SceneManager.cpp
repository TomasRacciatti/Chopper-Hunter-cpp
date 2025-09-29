#include "SceneManager.h"

SceneManager::SceneManager(sf::RenderWindow& win, ResourceManager& res)
	: _window(win), resourceManager(res)
{
	SetScene(SceneID::MainMenu);
}

void SceneManager::SetScene(SceneID id) 
{
    switch (id) 
    {
    case SceneID::MainMenu:
        _scene = std::make_unique<MainMenu>(_window, resourceManager);
        break;
    case SceneID::Gameplay:
        _scene = std::make_unique<GameplayScene>(_window, resourceManager);
        break;
    }
}