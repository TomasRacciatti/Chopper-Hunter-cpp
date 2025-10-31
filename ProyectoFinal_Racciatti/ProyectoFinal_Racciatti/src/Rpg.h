#pragma once
#include "Weapon.h"
#include <memory>

class ResourceManager;
class ExplosionEffect;

class Rpg : public Weapon
{
public:
	using Weapon::Weapon;

    void SetMissileSprite(sf::Texture& tex, sf::Vector2f originPx, float scale = 1.f)
    {
        _missileSprite = std::make_unique<sf::Sprite>(tex);
        _missileSprite->setOrigin(originPx);
        _missileSprite->setScale({ scale, scale });
    }

    void SetTurnRate(float degPerSec) { _turnRateDeg = degPerSec; }

    void SetupExplosion(ResourceManager& resources);

protected:
    bool Shoot(sf::Vector2f origin, sf::Vector2f dir) override;
    void PostUpdate(float dt, const Level& lvl) override;
    void PostDraw(sf::RenderTarget& rt) const override;

private:
    Bullet* _active = nullptr;

    std::unique_ptr<sf::Sprite> _missileSprite;

    // Hoaming
    sf::Vector2f _target{ 0.f,0.f };
    float _turnRateDeg = 180.f; // Velocidad de rot

    // Explosion
    std::unique_ptr<ExplosionEffect> _explosionFx;
    std::unique_ptr<sf::Sound> _explosionSfx;
    ResourceManager* _resources = nullptr;
    sf::Vector2f _lastPos{ 0.f, 0.f };

    // Sprite explosion
    const char* _explosionTexPath = "res/sprites/enemies/DroneExplosion.png";
    const sf::Vector2i _explosionFrameSize{ 48, 48 };
    const int _explosionFrameCount = 6;
    const int _hideSourceFromFrame = 2;
    const float _explosionScale = 3.0f;
};

