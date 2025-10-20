#include "Weapon.h"
#include "Level.h"
#include "Utils.h"
#include "AudioSettings.h"
#include <algorithm>
#include <cmath>
#include <utility>

Weapon::Weapon(float cooldown, float bulletSpeed,
	float bulletLife, int bulletDamage,
	Pool<Bullet>* sharedPool, AudioSettings& audio,
	std::unique_ptr<sf::Sound> fireSfx)
	: _bulletSpeed(bulletSpeed)
	, _bulletLifeTime(bulletLife)
	, _bulletDamage(bulletDamage)
	, _cooldown(cooldown)
	, _pool(sharedPool)
	, _audio(audio)
	, _fireSfx(std::move(fireSfx))
{
	
}

void Weapon::Update(float dt, bool fireHeld, sf::Vector2f origin, sf::Vector2f target, const Level& lvl)
{
	// Apuntar
	sf::Vector2f dir = Utils::SafeNormalize({ target.x - origin.x, target.y - origin.y });
	_angleDeg = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;

	// Rotar el visual
	_sprite->setPosition(origin);
	_sprite->setRotation(sf::degrees(_angleDeg));

	// Flip
	const float spriteScale = _baseScale;
	_sprite->setScale({ spriteScale, (dir.x < 0.f ? -spriteScale : spriteScale) });
	
	// Disparo
	_timer -= dt;

	if (fireHeld && _timer <= 0.f && HasAmmo())
	{
		sf::Vector2f dir = Utils::SafeNormalize({ target.x - origin.x, target.y - origin.y });
		//Shoot(origin, dir);
		if (Shoot(origin, dir))
		{
			_timer = _cooldown;

			if (_ammo.has_value())
				*_ammo -= 1;

			if (_fireSfx) 
			{
				_fireSfx->setVolume(_audio.GetSfxVolume());
				_fireSfx->play();
			}
		}
	}

	// Reciclo el pool
	for (auto& bullet : _pool->Items()) 
	{
		bullet.Update(dt, lvl);
		if (!bullet.Alive()) 
			_pool->Despawn(&bullet);
	}
}

void Weapon::Draw(sf::RenderTarget& rt) const
{
	// Visual del arma
	rt.draw(*_sprite);
	
	// Visual de balas
	for (const auto& bullet : _pool->Items())
		if (bullet.Alive()) bullet.Draw(rt);
}

Bullet* Weapon::EmitBullet(sf::Vector2f origin, sf::Vector2f dirUnit)
{
	if (!_pool) return nullptr;

	if (dirUnit.x == 0.f && dirUnit.y == 0.f) return nullptr;

	const sf::Vector2f muzzle = 
	{
	origin.x + dirUnit.x * _muzzleDistance,
	origin.y + dirUnit.y * _muzzleDistance
	};

	if (auto* bullet = _pool->Spawn()) 
	{
		bullet->Activate(muzzle,
			{ dirUnit.x * _bulletSpeed, dirUnit.y * _bulletSpeed },
			_bulletLifeTime, _bulletDamage);
		return bullet;
	}
	return nullptr; // pool agotada
}

void Weapon::UpdateProjectiles(float dt, const Level& lvl)
{
	for (auto& bullet : _pool->Items())
	{
		bullet.Update(dt, lvl);
		if (!bullet.Alive())
			_pool->Despawn(&bullet);
	}
}

void Weapon::DrawProjectiles(sf::RenderTarget& rt) const
{
	for (const auto& bullet : _pool->Items())
		if (bullet.Alive())
			bullet.Draw(rt);
}
