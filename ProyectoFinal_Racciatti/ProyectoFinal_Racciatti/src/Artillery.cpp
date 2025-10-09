#include "Artillery.h"
#include "Utils.h"
#include "Combat.h"
#include "ExplosionEffect.h"
#include <algorithm>
#include <cmath>
#include <SFML/System/Angle.hpp>

namespace
{
    constexpr float groundSafety = 32.f;
    constexpr float explosionYOffset = 29.f;
}

Artillery::Artillery(const sf::Vector2f& spawnPos, AudioSettings& audio, ResourceManager& resources, const std::string& spritePath, const sf::Vector2i& spriteFrameSize, float visualScale)
    : Entity(spawnPos, { spriteFrameSize.x * visualScale, spriteFrameSize.y * visualScale }, audio, hp)
    , _resourceManager(resources)
    , _tex(&resources.GetTexture(spritePath, /*useMipmap*/false, {}))
    , _sprite(*_tex)
    , _frameSize(spriteFrameSize)
    , _scale(visualScale)
    , _explosionSfx(resources.GetSound("../audio/sfx/ExplosionArti.mp3"))
    , _whistleSfx(resources.GetSound("../audio/sfx/ArtilleryWhistle.mp3"))
{
    _body.setOrigin(_body.getSize() * 0.5f);
    _body.setPosition(spawnPos);
    _sprite.setTextureRect(sf::IntRect({ 0, 0 }, { _frameSize.x, _frameSize.y }));

    _sprite.setOrigin({ _frameSize.x * 0.5f, _frameSize.y * 0.5f });
    _sprite.setScale({ _scale, _scale });
    _sprite.setPosition(spawnPos);

    _sprite.setRotation(sf::degrees(90.f)); // Esto lo rotamos para que este mirando para abajo

    _vel = { 0.f, _speedY };

    _whistleSfx.setLooping(true);
    _whistleSfx.setVolume(_audio.GetSfxVolume());
    _whistleSfx.play();
}

Artillery::~Artillery() = default;

void Artillery::Update(float dt, const Level& lvl)
{
    if (_exploding)
    {
        if (_explosionFx)
        {
            _explosionElapsed += dt;

            const int frame = std::min(_explosionFrameCount - 1,
                static_cast<int>(_explosionElapsed / _explosionFrameTime));

            if (!_aoeApplied && frame <= _aoeLastFrame && _aoeTarget && _aoeTarget->IsAlive())
            {
                // Cambiamos el radio por frame (se va expandiendo)
                const float radius = _aoeRadii[frame] * _explosionScale;
                const sf::Vector2f center = ExplosionAoECenterWorld();

                if (Utils::CircleIntersectsRect(center, radius, _aoeTarget->GetBounds()))
                {
                    Combat::ExplosionAoE(center, radius, _damage, _aoeTarget);
                    _aoeApplied = true;
                }

                _explosionFx->Update(dt);
                _explosionFx->SetPosition(_sprite.getPosition());

                if (_explosionFx->Finished())
                    _alive = false;
            }
        }
        return;
    }
}