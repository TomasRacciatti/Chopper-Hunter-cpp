#pragma once
#include <SFML/Graphics.hpp>

class Level;

class Entity
{
public:
	Entity(sf::Vector2f pos, sf::Vector2f size, int health = 1);
	virtual ~Entity() = default;

	virtual void Update(float dt, const Level& lvl) = 0;
	virtual void Draw(sf::RenderTarget& rt) const;

	// Vida
	int GetLife() const { return _health; }
	void SetLife(int value);
	void TakeDamage(int dmg);

	// Lifetime
	bool IsAlive() const { return _alive; }
	void Die() { _alive = false; }

	// Helpers que nos van a ayudar con colisiones
	sf::Vector2f Center() const;
	virtual sf::FloatRect GetBounds() const;

protected:
	sf::RectangleShape _body;
	float _visualScale = 1.f;

	int _maxHealth = 10;
	int _health = 10;
	bool _alive = true;

private:

};

