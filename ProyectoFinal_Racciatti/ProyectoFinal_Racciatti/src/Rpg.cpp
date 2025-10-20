#include "Rpg.h"
#include "Level.h"
#include "Utils.h"
#include <cmath>
#include <algorithm>

bool Rpg::Shoot(sf::Vector2f origin, sf::Vector2f dir)
{
    // Que no dispare si ya hay un misil en el aire, por mas que pueda haber pasado el cooldown
    if (_active && _active->Alive())
        return false;

    _active = EmitBullet(origin, dir);
    if (!_active) return false;

    // Escondemos el circulo amarillo original
    _active->SetRadius(0.01f);

    return true;
}

void Rpg::PostUpdate(float dt, const Level& /*lvl*/)
{
    if (!_active || !_active->Alive()) return;

    const sf::Vector2f pos = _active->GetPosition();
    const sf::Vector2f vel = _active->GetVelocity();

    const sf::Vector2f targetDir{ _lastTarget.x - pos.x, _lastTarget.y - pos.y };
    sf::Vector2f desiredDir = Utils::Normalize(targetDir);
    if (desiredDir.x == 0.f && desiredDir.y == 0.f) return;

    sf::Vector2f currentDir = Utils::Normalize(vel);
    if (currentDir.x == 0.f && currentDir.y == 0.f) currentDir = desiredDir;

    const float maxTurn = _turnRateDeg * dt;
    const float dot = std::clamp(currentDir.x * desiredDir.x + currentDir.y * desiredDir.y, -1.f, 1.f);
    const float angRad = std::acos(dot);
    const float angDeg = angRad * 180.f / 3.1415926535f;

    float t = 1.f;
    if (angDeg > maxTurn && angDeg > 0.f)
        t = maxTurn / angDeg;

    sf::Vector2f newDir = Utils::Normalize({
        currentDir.x * (1.f - t) + desiredDir.x * t,
        currentDir.y * (1.f - t) + desiredDir.y * t
        });

    _active->SetVelocity({ newDir.x * _bulletSpeed, newDir.y * _bulletSpeed });

    const float ang = std::atan2(newDir.y, newDir.x) * 180.f / 3.1415926535f;
    _missileSprite->setRotation(sf::degrees(ang));
    _missileSprite->setPosition(pos);
}

void Rpg::PostDraw(sf::RenderTarget& rt) const
{
    if (_active && _active->Alive())
        rt.draw(*_missileSprite);
}
