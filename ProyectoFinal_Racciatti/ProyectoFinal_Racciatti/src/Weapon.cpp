#include "Weapon.h"
#include "Level.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>

Weapon::Weapon(float cooldown, float bulletSpeed,
	float bulletLife, int bulletDamage,
	Pool<Bullet>* sharedPool)
	: _bulletSpeed(bulletSpeed)
	, _bulletLifeTime(bulletLife)
	, _bulletDamage(bulletDamage)
	, _cooldown(cooldown)
	, _pool(sharedPool)
{
	// Rectangulo para visualizar, reemplazar por sprite de arma
	_barrel.setSize({ 20.f, 4.f });
	_barrel.setOrigin({ 2.f, 2.f }); // Lo ponemos un poco mas atras para que rote desde la base
	_barrel.setFillColor(sf::Color(60, 110, 235));
}

void Weapon::Update(float dt, bool fireHeld, sf::Vector2f origin, sf::Vector2f target, const Level& lvl)
{
	// Apuntar
	sf::Vector2f dir = Utils::SafeNormalize({ target.x - origin.x, target.y - origin.y });
	_angleDeg = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;

	// Rotar el visual
	_barrel.setPosition(origin);
	_barrel.setRotation(sf::degrees(_angleDeg));
	
	// Disparo
	_timer -= dt;

	if (fireHeld && _timer <= 0.f)
	{
		sf::Vector2f dir = Utils::SafeNormalize({ target.x - origin.x, target.y - origin.y });
		Shoot(origin, dir);
		_timer = _cooldown;
	}

	// Reciclo el pool
	for (auto& b : _pool->Items()) 
	{
		if (!b.Alive()) 
			continue;

		b.Update(dt, lvl);
		if (!b.Alive()) 
			_pool->Despawn(&b);
	}
}

void Weapon::Draw(sf::RenderTarget& rt) const
{
	// Visual del arma
	rt.draw(_barrel);
	
	// Visual de balas
	for (const auto& bullet : _pool->Items())
		if (bullet.Alive()) bullet.Draw(rt);
}

Bullet* Weapon::EmitBullet(sf::Vector2f origin, sf::Vector2f dirUnit)
{
	if (!_pool) return nullptr;

	if (dirUnit.x == 0.f && dirUnit.y == 0.f) return nullptr;

	if (auto* b = _pool->Spawn()) 
	{
		b->Activate(origin,
			{ dirUnit.x * _bulletSpeed, dirUnit.y * _bulletSpeed },
			_bulletLifeTime, _bulletDamage);
		return b;
	}
	return nullptr; // pool agotada
}
