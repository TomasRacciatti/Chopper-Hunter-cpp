#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "Level.h"
#include "Player.h"
#include "Helicopter.h"
#include "Pistol.h"
#include "Combat.h"
#include "PauseMenu.h"

class GameplayScene : public Scene
{
public:
	GameplayScene(ResourceManager& resource, sf::RenderWindow& window, AudioSettings& audio);
	~GameplayScene() override;

	void HandleEvents(const sf::Event& ev) override;
	void Input() override;
	void Update(float dt) override;
	void Draw() override;

private:
	ResourceManager& resourceManager;

	// Pools
	size_t poolCapacity = 64;
	Pool<Bullet> _playerBulletPool{
	  poolCapacity, [](Bullet& b) { b.SetRadius(4.f); b.SetColor(sf::Color(240,225,60)); b.SetOutline(1.f, sf::Color::Black); }
	};
	Pool<Bullet> _enemyBulletPool{
	  poolCapacity, [](Bullet& b) { b.SetRadius(4.f); b.SetColor(sf::Color(230,50,50)); b.SetOutline(1.f, sf::Color::Black); }
	};

	// World
	Level _level;
	std::unique_ptr<Player> _player;
	std::unique_ptr<Helicopter> _heli;

	// Input
	Player::Input _playerInput{};

	// Pause
	PauseMenu* _pause = nullptr;
	bool _wasPaused = false;

	// Audio
	sf::Music music;
	AudioSettings& _audio;

	// helpers
	void CreatePlayer();
	void SpawnHelicopter();
};

