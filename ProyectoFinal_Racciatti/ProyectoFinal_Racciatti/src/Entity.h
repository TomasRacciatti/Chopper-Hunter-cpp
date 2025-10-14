#pragma once
#include <SFML/Graphics.hpp>

class Level;
class AudioSettings;
class GameProgress;

class Entity
{
public:
	Entity(sf::Vector2f pos, sf::Vector2f size, AudioSettings& audio, int health = 1);
	virtual ~Entity() = default;

	virtual void Update(float dt, const Level& lvl) = 0;
	virtual void Draw(sf::RenderTarget& rt) const;

	// Progress
	void SetScoreReward(int value) { _scoreReward = value; }
	void SetProgress(GameProgress* progress) { _progress = progress; }

	// Vida
	int GetLife() const { return _health; }
	void SetLife(int value);
	virtual void TakeDamage(int dmg);

	// Lifetime
	bool IsAlive() const { return _alive; }
	void Die();

	// Helpers que nos van a ayudar con colisiones
	sf::Vector2f Center() const;
	virtual sf::FloatRect GetBounds() const;

protected:
	sf::RectangleShape _body;
	float _visualScale = 1.f;

	int _maxHealth = 10;
	int _health = 10;
	bool _alive = true;

	AudioSettings& _audio;

	// Progress
	int _scoreReward = 0;
	bool _rewarded = false;
	GameProgress* _progress = nullptr;

private:

};

