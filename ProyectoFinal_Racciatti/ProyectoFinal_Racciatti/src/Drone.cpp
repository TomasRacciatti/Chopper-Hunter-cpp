#include "Drone.h"
#include "Utils.h"
#include "Combat.h"
#include "ExplosionEffect.h"
#include <cmath>

namespace
{
    constexpr float groundSafety = 32.f;
}

Drone::Drone(const sf::Vector2f& spawnPos,
    AudioSettings& audio,
    ResourceManager& resources,
    const std::string& spritePath,
    const sf::Vector2i& spriteFrameSize,
    float visualScale)
    : Entity(spawnPos, { spriteFrameSize.x * visualScale, spriteFrameSize.y * visualScale }, audio, _hp)
    , _resourceManager(resources)
    , _frameSize(spriteFrameSize)
    , _scale(visualScale)
    , _tex(&resources.GetTexture(spritePath, false, {}))
    , _sprite(*_tex)
    , _explosionSfx(resources.GetSound("../audio/sfx/ExplosionDrone.mp3"))
    , _buzzSfx(resources.GetSound("../audio/sfx/DroneFly.mp3"))
{
    _body.setOrigin(_body.getSize() * 0.5f);
    _body.setPosition(spawnPos);
    _sprite.setTextureRect(sf::IntRect({ 0, 0 }, { _frameSize.x, _frameSize.y }));

    _sprite.setOrigin({ _frameSize.x * 0.5f, _frameSize.y * 0.5f });
    _sprite.setScale({ _scale, _scale });
    _sprite.setPosition(spawnPos);

    _buzzSfx.setLooping(true);
    _buzzSfx.setVolume(_audio.GetSfxVolume());
    _buzzSfx.play();

    _aoeRadius = 0.5f * _explosionFrameSize.x;
}

Drone::~Drone() = default;

void Drone::Update(float dt, const Level& lvl)
{
    if (_exploding)
    {
        if (_explosionFx)
        {
            _explosionFx->Update(dt);
            _explosionFx->SetPosition(_sprite.getPosition());

            if (_explosionFx->Finished())
                _alive = false;
        }
        return;
    }

    _buzzSfx.setVolume(_audio.GetSfxVolume());

    UpdateMotion(dt);
    UpdateFlipFromVelocity();

    const auto floor = lvl.FloorRect();
    const float floorY = floor.position.y;

    sf::Vector2f pos = _sprite.getPosition();
    const float halfHeight = _frameSize.y * _scale * 0.5f;
    const float maxY = floorY - groundSafety - halfHeight;
    if (pos.y > maxY) pos.y = maxY;

    _sprite.setPosition(pos);
    _body.setPosition(pos);
    UpdateAnimation(dt);
}

void Drone::Draw(sf::RenderTarget& rt) const
{
    const bool hideSource = _explosionFx && _explosionFx->ShouldHideSource();

    if (!hideSource)
        rt.draw(_sprite);

    if (_explosionFx)
        _explosionFx->Draw(rt);
}

void Drone::TakeDamage(int dmg)
{
    if (_exploding) return;

    const int before = _health;
    Entity::TakeDamage(dmg);

    if (before > 0 && _health <= 0)
    {
        _alive = true;
        StartExplosion();
    }
}

sf::FloatRect Drone::GetBounds() const
{
    sf::FloatRect body = _body.getGlobalBounds();

    // Le sacamos 10% de cada borde al cuerpo, para que la bala no colisione con un espacio vacio
    const float insetX = body.size.x * 0.10f;
    const float insetY = body.size.y * 0.10f;

    body.position.x += insetX;
    body.position.y += insetY;
    body.size.x -= 2.f * insetX;
    body.size.y -= 2.f * insetY;

    return body;
}

void Drone::UpdateAnimation(float dt)
{
    _animTimer += dt;
    if (_animTimer >= _frameTime)
    {
        _animTimer -= _frameTime;
        _frame = (_frame + 1) % _frameCount;

    }

    const sf::IntRect frameRect = Utils::FrameRect(
        _frame,
        _animRow,
        _frameSize.x,
        _frameSize.y
    );

    _sprite.setTextureRect(frameRect);
}

void Drone::UpdateMotion(float dt)
{
    const sf::Vector2f pos = _sprite.getPosition();
    const sf::Vector2f dir = Utils::SafeNormalize(_target - pos);
    _vel = dir * _speed;

    const sf::Vector2f delta = _vel * dt;
    _sprite.move(delta);
    _body.move(delta);
}

void Drone::UpdateFlipFromVelocity()
{
    const bool shouldFaceLeft = (_vel.x < 0.f);

    if (shouldFaceLeft != _faceLeft)
    {
        _faceLeft = shouldFaceLeft;
        const float sizeX = _faceLeft ? -_scale : _scale;
        _sprite.setScale({ sizeX, _scale });
    }
}

void Drone::StartExplosion()
{
    _exploding = true;
    _buzzSfx.stop();

    const sf::Vector2f explosionSpawn = _sprite.getPosition();
    const std::string explosionTexPath = "../sprites/enemies/DroneExplosion.png";

    _explosionFx = std::make_unique<ExplosionEffect>(
        _resourceManager,
        explosionTexPath,
        explosionSpawn,
        _explosionFrameSize,
        _explosionFrameCount,
        _hideSourceFromFrame,
        _explosionScale);

    _explosionSfx.setLooping(false);
    _explosionSfx.setVolume(_audio.GetSfxVolume());
    _explosionSfx.play();

    _aoeRadius = 0.5f * _explosionFrameSize.x;
    
    if (_aoeTarget)
        Combat::ExplosionAoE(explosionSpawn, _aoeRadius, _damage, _aoeTarget);
}