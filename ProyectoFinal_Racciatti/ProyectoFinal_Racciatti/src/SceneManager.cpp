#include "SceneManager.h"

SceneManager::SceneManager(sf::RenderWindow& win, ResourceManager& res, AudioSettings& audio)
	: _window(win), resourceManager(res), _audio(audio)
{
	SetScene(SceneID::MainMenu);
}

void SceneManager::SetScene(SceneID id) 
{
    switch (id) 
    {
    case SceneID::MainMenu:
        _scene = std::make_unique<MainMenu>(resourceManager, _window, _audio);
        break;
    case SceneID::Gameplay:
        _scene = std::make_unique<GameplayScene>(resourceManager, _window, _audio);
        break;
    }
}