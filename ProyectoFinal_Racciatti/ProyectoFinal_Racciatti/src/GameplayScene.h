#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "Level.h"
#include "Player.h"
#include "Helicopter.h"
#include "Pistol.h"
#include "Combat.h"

class GameplayScene : public Scene
{
public:
	GameplayScene(sf::RenderWindow& window, ResourceManager& resourceManager);
	~GameplayScene() override;

	void HandleEvents(const sf::Event& ev) override;
	void Input() override;
	void Update(float dt) override;
	void Draw() override;

private:
	ResourceManager& resourceManager;

	// Pools
	size_t poolCapacity = 128;
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

	// helpers
	void CreatePlayer();
	void SpawnHelicopter();
};

