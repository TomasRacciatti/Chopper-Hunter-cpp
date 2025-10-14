#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameProgress;
class Entity;
class ResourceManager;

class GameplayUI
{
public:
	GameplayUI(ResourceManager& resources, const sf::Vector2u& winSize);

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
	sf::Text _timeText;
	sf::Text _scoreText;
	const sf::Font* _font = nullptr;

	// hp bar
	const sf::Texture* _hpEmptyTex = nullptr;
	const sf::Texture* _hpFullTex = nullptr;
	sf::Sprite _hpEmpty;
	sf::Sprite _hpFull;

	// Layout
	float _margin = 10.f;
	float _hpScale = 1.0f;

	// Helpers
	static std::string FormatTime(float seconds);
	void Layout();
	void UpdateHpBar();
};

