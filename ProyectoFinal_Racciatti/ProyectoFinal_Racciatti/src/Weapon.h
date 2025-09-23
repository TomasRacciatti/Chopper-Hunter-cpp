#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "Pool.h"

class Level;

class Weapon
{
public:
	Weapon(float cooldown, float bulletSpeed,
		float bulletLife, int bulletDamage,
		Pool<Bullet>* sharedPool);

	virtual ~Weapon() = default;

	// Origin es el soothing point, Target es el mouse
	void Update(float dt, bool fireHeld, sf::Vector2f origin, sf::Vector2f target, const Level& lvl);
	void Draw(sf::RenderTarget& rt) const;

	// Modificadores
	void SetCooldown(float cd) { _cooldown = cd; }
	void SetBulletSpeed(float speed) { _bulletSpeed = speed; }
	void SetBulletLife(float life) { _bulletLifeTime = life; }
	void SetBulletDamage(int dmg) { _bulletDamage = dmg; }

	float AngleDegrees() const { return _angleDeg; }

protected:
	// Disparo particular de cada arma
	virtual void Shoot(sf::Vector2f origin, sf::Vector2f dir) = 0;

	// Helper para spawnear una bullet del pool con los defaults del arma
	Bullet* EmitBullet(sf::Vector2f origin, sf::Vector2f dirUnit);

	float _bulletSpeed;
	float _bulletLifeTime;
	int   _bulletDamage;

	// Visual
	sf::RectangleShape _barrel;  // Debug, ahora es un rectangulo nomas, despues reemplazar con el sprite
	float _angleDeg = 0.f;

private:
	float _cooldown; // (Fire rate)
	float _timer = 0.f;
	

	Pool<Bullet>* _pool = nullptr;
};

