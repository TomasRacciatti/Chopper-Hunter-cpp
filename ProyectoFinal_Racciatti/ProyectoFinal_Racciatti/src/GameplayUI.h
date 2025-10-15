#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"

class GameProgress;
class Entity;

class GameplayUI
{
public:
	GameplayUI(ResourceManager& resources, const sf::Vector2u& winSize);
	~GameplayUI();

	void SetProgress(GameProgress* progress) { _progress = progress; }
	void SetPlayer(Entity* player) { _player = player; }

	void Update(float dt);
	void Draw(sf::RenderTarget& rt) const;

private:
	ResourceManager& _resourceManager;
	sf::Vector2u _win{ 0,0 };

	GameProgress* _progress = nullptr;
	Entity* _player = nullptr;

	// Text
	sf::Text text;

	sf::Text* _timeText = nullptr;
	sf::Text* _scoreText = nullptr;
	static constexpr int _characterSize = 28;

	// hp bar
	sf::Sprite* _hpEmpty = nullptr;
	sf::Sprite* _hpFull = nullptr;

	// Layout
	float _margin = 10.f;
	float _hpScale = 2.0f;
	float _bottomY = 0.f;

	// Helpers
	void Layout();
	void UpdateHpBar();
};

