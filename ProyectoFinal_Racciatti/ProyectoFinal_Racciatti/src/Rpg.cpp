#include "Rpg.h"
#include "Level.h"
#include "Utils.h"
#include "ResourceManager.h"
#include "ExplosionEffect.h"
#include <cmath>
#include <algorithm>

bool Rpg::Shoot(sf::Vector2f origin, sf::Vector2f dir)
{
    // Que no dispare si ya hay un misil en el aire, por mas que pueda haber pasado el cooldown
    if (_active && _active->Alive())
        return false;

    _active = EmitBullet(origin, dir);
    if (!_active) return false;

    _lastPos = origin;
    return true;
}

void Rpg::SetupExplosion(ResourceManager& resources)
{
    _resources = &resources;
    _explosionSfx = std::make_unique<sf::Sound>(resources.GetSound("res/audio/sfx/ExplosionDrone.mp3"));
}

void Rpg::PostUpdate(float dt, const Level& /*lvl*/)
{
    if (_active && !_active->Alive())
    {
        if (!_explosionFx && _resources)
        {
            _explosionFx = std::make_unique<ExplosionEffect>(
                *_resources,
                _explosionTexPath,
                _lastPos,
                _explosionFrameSize,
                _explosionFrameCount,
                _hideSourceFromFrame,
                _explosionScale
            );

            if (_explosionSfx)
            {
                _explosionSfx->setLooping(false);
                _explosionSfx->setVolume(_audio.GetSfxVolume());
                _explosionSfx->play();
            }
        }

        _active = nullptr;
    }

    if (_active && _active->Alive())
    {
        const sf::Vector2f pos = _active->GetPosition();
        const sf::Vector2f vel = _active->GetVelocity();
        _lastPos = pos;

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
    
    if (_explosionFx)
    {
        _explosionFx->Update(dt);
        _explosionFx->SetPosition(_lastPos);

        if (_explosionFx->Finished())
            _explosionFx.reset();
    }
}

void Rpg::PostDraw(sf::RenderTarget& rt) const
{
    if (_active && _active->Alive())
        rt.draw(*_missileSprite);

    if (_explosionFx)
        _explosionFx->Draw(rt);
}
