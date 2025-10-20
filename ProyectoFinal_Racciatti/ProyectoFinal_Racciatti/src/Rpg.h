#pragma once
#include "Weapon.h"
#include <memory>

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


};

