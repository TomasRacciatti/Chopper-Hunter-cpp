#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "GameProgress.h" 
#include "Level.h"
#include "Player.h"
#include "Helicopter.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "Combat.h"
#include "PauseMenu.h"
#include "Drone.h"
#include "Artillery.h"
#include "GameplayUI.h"
#include "DeathPanel.h"
#include "HighScores.h"

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
	GameProgress _progress;
	GameplayUI* _gameplayUI = nullptr;

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
	std::vector<std::unique_ptr<Drone>> _drones;
	std::vector<std::unique_ptr<Artillery>> _artilleryRounds;

	// Input
	Player::Input _playerInput{};

	// Pause
	PauseMenu* _pause = nullptr;
	bool _wasPaused = false;
	bool _freezeOneFrame = false;

	// Audio
	sf::Music music;
	AudioSettings& _audio;

	// ---- Suba de dificultad ----

	int _difficultyStage = 0;
	int _incrementAddition = 75;
	int _difficultyIncrement = 100;
	int _nextDifficultyScore = 100;
	float _spawnMinClamp = 0.5f;

	void CheckAdvanceDifficulty();
	void ApplyDifficultyStep();
	void ClampSpawnWindows(); 

	// Drone
	float _droneSpawnMin = 10.f; 
	float _droneSpawnMax = 15.f; 
	float _droneSpawnTimer = 0.f;
	int   _maxDronesOnScene = 1;

	// Arti
	float _artillerySpawnMin = 15.f; 
	float _artillerySpawnMax = 20.f;
	float _artillerySpawnTimer = 0.f;
	int   _maxArtilleryOnScene = 1;

	// Highscore
	HighScores _scores;
	DeathPanel* _death = nullptr;

	// helpers
	void CreatePlayer();
	void SpawnHelicopter();
	void SpawnDrone();
	void SpawnArtillery();
};

