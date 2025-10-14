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
    SetScoreReward(scoreReward);
    
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
            }
                _explosionFx->Update(dt);
                _explosionFx->SetPosition(_explosionWorldPos);

                if (_explosionFx->Finished())
                    Die();
        }
        return;
    }

    _whistleSfx.setVolume(_audio.GetSfxVolume());

    const sf::Vector2f delta = _vel * dt;
    _sprite.move(delta);
    _body.move(delta);

    const auto floor = lvl.FloorRect();
    const float floorY = floor.position.y;

    const sf::FloatRect bounds = _sprite.getGlobalBounds();
    const float bottom = bounds.position.y + bounds.size.y;

    if (bottom >= floorY - groundSafety)
    {
        const float groundY = floorY - groundSafety;
        const float correction = groundY - bottom;
        _sprite.move({ 0.f, correction });
        _body.setPosition(_sprite.getPosition());
        StartExplosion(groundY);
    }
}

void Artillery::Draw(sf::RenderTarget& rt) const
{
    const bool hideSource = _explosionFx && _explosionFx->ShouldHideSource();

    if (!hideSource)
        rt.draw(_sprite);

    if (_explosionFx)
        _explosionFx->Draw(rt);
}

sf::FloatRect Artillery::GetBounds() const
{
    sf::FloatRect bounds = _sprite.getGlobalBounds();
    const float insetX = bounds.size.x * 0.10f;
    const float insetY = bounds.size.y * 0.10f;

    bounds.position.x += insetX;
    bounds.position.y += insetY;
    bounds.size.x -= 2.f * insetX;
    bounds.size.y -= 2.f * insetY;

    return bounds;
}

void Artillery::TakeDamage(int dmg)
{
    // Lo sobreescribimos de entity para que no pueda tomar daño
}

void Artillery::StartExplosion(float groundY)
{
    if (_exploding) return;
    _exploding = true;

    _whistleSfx.stop();

    const float centerOffsetY = (_explosionGroundPixelY - _explosionFrameSize.y * 0.5f) * _explosionScale;
    _explosionWorldPos.x = _sprite.getPosition().x;
    _explosionWorldPos.y = groundY - centerOffsetY;

    const std::string explosionPath = "../sprites/enemies/ArtilleryExplosion.png";

    _explosionFx = std::make_unique<ExplosionEffect>(
        _resourceManager,
        explosionPath,
        _explosionWorldPos,
        _explosionFrameSize,
        _explosionFrameCount,
        _hideSourceFromFrame,
        _explosionScale
    );

    _explosionSfx.setLooping(false);
    _explosionSfx.setVolume(_audio.GetSfxVolume());
    _explosionSfx.play();

    _explosionElapsed = 0.f;
    _aoeApplied = false;
}

sf::Vector2f Artillery::ExplosionAoECenterWorld() const
{
    const float deltaY = (_explosionCenterPixel.y - _explosionFrameSize.y * 0.5f) * _explosionScale;
    return { _explosionWorldPos.x, _explosionWorldPos.y + deltaY };
}