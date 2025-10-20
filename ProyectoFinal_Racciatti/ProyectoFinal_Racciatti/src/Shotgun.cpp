#include "Shotgun.h"

static inline sf::Vector2f RotDeg(sf::Vector2f vel, float deg)
{
    const float rad = deg * 3.1415926535f / 180.f;
    const float cos = std::cos(rad), sin = std::sin(rad);
    return { vel.x * cos - vel.y * sin, vel.x * sin + vel.y * cos };
}

bool Shotgun::Shoot(sf::Vector2f origin, sf::Vector2f dir)
{
	// Guardrails
	if (_pellets <= 0) return false;

    int spawned = 0;
    if (_pellets == 1) // Nunca deberia entrar aca, pero lo dejo por seguridad
    {
        if (EmitBullet(origin, dir)) ++spawned;
        return spawned > 0;
    }

    const float totalSpread = _spreadAngle * 2.f;
    const float stepAngle = totalSpread / (_pellets - 1);

    float angle = -_spreadAngle;

    for (int i = 0; i < _pellets; ++i, angle += stepAngle)
    {
        const sf::Vector2f dir = RotDeg(dir, angle);
        if (EmitBullet(origin, dir)) ++spawned;
    }
    return spawned > 0;
}
