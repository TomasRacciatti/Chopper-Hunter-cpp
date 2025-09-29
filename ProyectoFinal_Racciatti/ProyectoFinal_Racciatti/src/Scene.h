#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

enum class SceneID
{
	MainMenu,
	Gameplay
};

class Scene
{
protected:
	SceneID nextSceneID;
	sf::RenderWindow& _window;
	bool wantsChange;

public:
	Scene(sf::RenderWindow& window);
	virtual ~Scene() = 0;

	virtual void Input() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	virtual void HandleEvents(const sf::Event& ev) = 0;

	virtual bool GetWantsChange();
	virtual void SetWantsChange(bool value);

	virtual SceneID GetNextSceneID();
};

