#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AudioSettings.h"
#include "Bullet.h"
#include "Pool.h"
#include <memory>
#include <optional>

class Level;

class Weapon
{
public:
	Weapon(float cooldown, float bulletSpeed,
		float bulletLife, int bulletDamage,
		Pool<Bullet>* sharedPool, AudioSettings& audio,
		std::unique_ptr<sf::Sound> fireSfx);

	virtual ~Weapon() = default;

	// Origin es el soothing point, Target es el mouse
	void Update(float dt, bool fireHeld, sf::Vector2f origin, sf::Vector2f target, const Level& lvl);
	void Draw(sf::RenderTarget& rt) const;

	void DrawExtras(sf::RenderTarget& rt) const { PostDraw(rt); }

	// Getters & Setters
	float GetCooldownNormalized()
	{ 
		float t = _timer; 
		if (t < 0.f) t = 0.f; 
		float fill = 1.f - (t / _cooldown);
		return fill < 0.f ? 0.f : (fill > 1.f ? 1.f : fill);
	}
	void SetCooldown(float cd) { _cooldown = cd; }
	void SetBulletSpeed(float speed) { _bulletSpeed = speed; }
	void SetBulletLife(float life) { _bulletLifeTime = life; }
	void SetBulletDamage(int dmg) { _bulletDamage = dmg; }

	void TickPassive(float dt, const Level& lvl);

	float AngleDegrees() const { return _angleDeg; }

	void SetVisualSprite(sf::Texture& tex, sf::Vector2f originPx, float spriteScale = 1.f) 
	{
		_sprite = std::make_unique<sf::Sprite>(tex);
		_sprite->setOrigin(originPx);
		_baseScale = spriteScale;
		_sprite->setScale({ _baseScale, _baseScale });
	}
	void SetMuzzleDistance(float d) { _muzzleDistance = d; }

	// Ammo
	int  GetAmmo() const { return _ammo.has_value() ? *_ammo : 99; } // El numero es para el UI del de balas infinitas
	void SetAmmo(int value) { _ammo = value; }
	void SetInfiniteAmmo() { _ammo.reset(); }
	bool HasAmmo() const { return !_ammo.has_value() || *_ammo > 0;}
	void AddAmmo(int amount)
	{
		if (!_ammo.has_value() || amount == 0) return; 
		*_ammo += amount;                              
		if (*_ammo < 0) *_ammo = 0;                    
	}


	// necesario para que no se cuelguen las balas cuando se muere
	void UpdateProjectiles(float dt, const Level& lvl);
	void DrawProjectiles(sf::RenderTarget& rt) const;

protected:
	// Disparo particular de cada arma
	virtual bool Shoot(sf::Vector2f origin, sf::Vector2f dir) = 0;

	// Helper para spawnear una bullet del pool con los defaults del arma
	Bullet* EmitBullet(sf::Vector2f origin, sf::Vector2f dirUnit);

	// Para el RPG
	virtual void PostUpdate(float /*dt*/, const Level& /*lvl*/) {}
	virtual void PostDraw(sf::RenderTarget& /*rt*/) const {}

	sf::Vector2f _lastTarget{ 0.f, 0.f };

	float _bulletSpeed;
	float _bulletLifeTime;
	int   _bulletDamage;

	// Visual
	std::unique_ptr<sf::Sprite> _sprite;
	float _muzzleDistance = 25.f;
	float _angleDeg = 0.f;

	// Audio
	AudioSettings& _audio;
	std::unique_ptr < sf::Sound> _fireSfx;

private:
	float _cooldown; // (Fire rate)
	float _timer = 0.f;
	
	float _baseScale = 1.f;

	Pool<Bullet>* _pool = nullptr;

	// Ammo
	std::optional<int> _ammo;
};

