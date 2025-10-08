#include "Helicopter.h"
#include "Level.h"
#include "Utils.h"
#include "AudioSettings.h"
#include <algorithm>
#include <cmath>

Helicopter::Helicopter(sf::Vector2f spawnPos, std::unique_ptr<Weapon> turret, AudioSettings& audio, ResourceManager& resources,
    const std::string& sheetPath, int hp)
    : Entity(spawnPos, { 140.f, 51.f }, audio, hp)
    , _turret(std::move(turret))
    , _tex(&resources.GetTexture(sheetPath, false, {}))
    , _sprite(*_tex)
    , _resources(resources)
    , _explosionSfx(resources.GetSound("../audio/sfx/ExplosionHeli.mp3"))
    , _flightSfx(resources.GetSound("../audio/sfx/helicopter.mp3"))
{
    _sprite.setTextureRect(sf::IntRect({ 0, 0 }, { _frameSize.x, _frameSize.y }));
    
    _visualScale = 2.f;
    _sprite.setScale(sf::Vector2f(_visualScale, _visualScale));

    _sprite.setOrigin(sf::Vector2f(_frameSize.x * 0.5f, _frameSize.y * 0.5f));

    _body.setSize({ _frameSize.x * _visualScale, _frameSize.y * _visualScale }); // Aca sobreescribo el tamaño
    _body.setOrigin(_body.getSize() * 0.5f);
    _body.setPosition(spawnPos);
    _sprite.setPosition(spawnPos);

    _flightSfx.setLooping(true);
    _flightSfx.setVolume(_audio.GetSfxVolume());
    _flightSfx.play();
}

void Helicopter::Update(float dt, const Level& lvl)
{
    if (!_dying && _health <= 0)
        StartExplosion();

    _flightSfx.setVolume(_audio.GetSfxVolume());

    if (_dying)
    {
        if (_explosion)
        {
            _explosion->Update(dt);
            _explosion->SetPosition(_sprite.getPosition());

            if (_turret) 
                _turret->UpdateProjectiles(dt, lvl);

            if (_explosion->Finished())
                _alive = false;
        }

        _explosionSfx.setVolume(_audio.GetSfxVolume());

        if (!_explosion && _explosionSfx.getStatus() != sf::SoundSource::Status::Playing)
        {
            _alive = false;
        }

        return;
    }
    
    switch (_state) 
    {
        case State::Entering: UpdateEntering(dt, lvl); break;
        case State::Patrol:   UpdatePatrol(dt, lvl); break;
        case State::Hover:    UpdateHover(dt, lvl); break;
    }

    // Anim
    _sprite.setPosition(_body.getPosition());
    UpdateDamageVisual();
    UpdateAnimation(dt);

    // Turret 
    const sf::Vector2f heliPos = _body.getPosition();

    // siempre esta apuntando al jugador
    if (_turret) 
    {
        const sf::Vector2f turretPos = TurretBaseWorld();
        _turret->Update(dt, /*fireHeld*/ _firing, turretPos, _targetPos, lvl);
    }
}

void Helicopter::Draw(sf::RenderTarget& rt) const
{
    if (_dying)
    {
        if (_turret) _turret->DrawProjectiles(rt);

        if (!_explosion || !_explosion->ShouldHideSource())
            rt.draw(_sprite);

        if (_explosion)
            _explosion->Draw(rt);
        return;
    }
    
    if (_turret) _turret->Draw(rt);
    rt.draw(_sprite);
}

void Helicopter::TakeDamage(int dmg)
{
    if (_dying) return;

    const int before = _health; // Controlador para que se corra la anim de muerte
    Entity::TakeDamage(dmg);

    if (before > 0 && _health <= 0)
    {
        _alive = true; // Lo sobreescribimos para que no lo mate antes de la anim
        StartExplosion();
    }
}


//      ===== FSM =====

// Trancisiones
void Helicopter::EnterPatrol(const Level& lvl)
{
    _state = State::Patrol;
    _firing = true;
    _hoverTimer = 0.f;
    _bobTime = 0.f;
    PickNewPatrolTarget(lvl);
}

void Helicopter::EnterHover()
{
    _state = State::Hover;
    _firing = true;
    _hoverDuration = Utils::RandomFloat(_minHoverDuration, _maxHoverDuration);
    _hoverTimer = 0.f;
    _bobTime = 0.f;
}

void Helicopter::PickNewPatrolTarget(const Level& lvl)
{
    const auto win = lvl.WindowSize();
    const float bodyWidth = _body.getSize().x;

    _minX = 32.f;
    _maxX = static_cast<float>(win.x) - bodyWidth - 32.f;

    _patrolSpeed = Utils::RandomFloat(_minSpeed, _maxSpeed);
    _patrolTargetX = Utils::RandomFloat(_minX, _maxX);
}


// Updates

void Helicopter::UpdateEntering(float dt, const Level& lvl)
{
    if (!_entryTargetReady) 
    {
        PickNewPatrolTarget(lvl);
        _entryTargetReady = true;
    }

    _firing = false;
    
    auto pos = _body.getPosition();
    const sf::Vector2f target{ _patrolTargetX, _altitude };

    const sf::Vector2f velocity{ target.x - pos.x, target.y - pos.y };
    const float len = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

    if (len > 0.f) 
    {
        const float step = _enterSpeed * dt;
        if (step >= len) 
        {
            _body.setPosition(target);
            EnterHover();
        }
        else
        {
            const sf::Vector2f dir{ velocity.x / len, velocity.y / len };
            pos.x += dir.x * step;
            pos.y += dir.y * step;
            _body.setPosition(pos);
        }
    }
    else
    {
        EnterHover();
    }
}

void Helicopter::UpdatePatrol(float dt, const Level& lvl)
{
    auto pos = _body.getPosition();
    if (_maxX <= _minX) PickNewPatrolTarget(lvl);

    float deltaX = _patrolTargetX - pos.x;
    int dir = (deltaX >= 0.f) ? +1 : -1;
    float step = dir * _patrolSpeed * dt;

    if (std::abs(step) > std::abs(deltaX)) 
        step = deltaX;

    pos.x += step;
    _body.setPosition(pos);

    if (std::abs(_patrolTargetX - pos.x) < 2.f) 
    {
        EnterHover();
    }
}

void Helicopter::UpdateHover(float dt, const Level& lvl)
{
    _hoverTimer += dt;
    _bobTime += dt;

    auto pos = _body.getPosition();
    pos.y = _altitude + std::sin(_bobTime * _hoverBobSpeed) * _hoverBobAmp;
    pos.x += std::sin(_bobTime * 1.8f) * _hoverHorizontalSpeed * dt;

    const auto win = lvl.WindowSize();
    const float bodyWidth = _body.getSize().x;
    _minX = 32.f;
    _maxX = static_cast<float>(win.x) - bodyWidth - 32.f;
    pos.x = std::clamp(pos.x, _minX, _maxX);

    _body.setPosition(pos);

    if (_hoverTimer >= _hoverDuration) 
    {
        EnterPatrol(lvl);
    }
}

// ==== Anim ====

void Helicopter::UpdateAnimation(float dt)
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

void Helicopter::UpdateDamageVisual()
{
    const float hp = static_cast<float>(_health);
    const float maxHp = static_cast<float>(_maxHealth);
    const float ratio = (maxHp > 0.f) ? hp / maxHp : 0.f;

    int desiredRow;
    if (ratio > 2.f / 3.f)
        desiredRow = 0; // 100% a 66% de la vida
    else if (ratio > 1.f / 3.f)
        desiredRow = 1; // 66% a 33%
    else
        desiredRow = 2; // menos de 33%

    // clamp manual
    if (desiredRow < 0) desiredRow = 0;
    if (desiredRow >= _animRows) desiredRow = _animRows - 1;

    _animRow = desiredRow;
}

// ===== Explosions =====

void Helicopter::StartExplosion()
{
    if (_dying) return;
    _dying = true;

    _flightSfx.stop();

    const sf::Vector2i frameSize(32, 32);
    const int   frames = 9;
    const int   hideAt = 3;
    const float scale = 5.5f;
    const char* texPath = "../sprites/enemies/HeliExplosion.png";
    const char* sfxPath = "../audio/sfx/ExplosionHeli.mp3";

    _explosion = std::make_unique<ExplosionEffect>(
        _resources, 
        texPath, 
        _sprite.getPosition(),
        frameSize, 
        frames, 
        hideAt, 
        scale);

    sf::SoundBuffer& buffer = _resources.GetSound(sfxPath);
    _explosionSfx.setBuffer(buffer);
    _explosionSfx.setLooping(false);
    _explosionSfx.setVolume(_audio.GetSfxVolume());
    _explosionSfx.play();
}

// ==== Turret ====

sf::Vector2f Helicopter::Muzzle() const
{
    const auto rect = _body.getGlobalBounds();
    const float muzzleX = rect.position.x + rect.size.x * 0.80f;
    const float muzzleY = rect.position.y + rect.size.y * 0.45f;
    return { muzzleX, muzzleY };
}

sf::Vector2f Helicopter::TurretBaseWorld() const
{
    const sf::Vector2f heliPos = _sprite.getPosition();

    const float localX = kTurretOffsetPx.x - (_frameSize.x * 0.5f);
    const float localY = kTurretOffsetPx.y - (_frameSize.y * 0.5f);

    return {
        heliPos.x + localX * _visualScale,
        heliPos.y + localY * _visualScale
    };
}

sf::FloatRect Helicopter::GetBounds() const
{
    const sf::Vector2f c = _body.getPosition();

    const float scaleX = _body.getSize().x / static_cast<float>(_frameSize.x);
    const float scaleY = _body.getSize().y / static_cast<float>(_frameSize.y);

    const float fullW = static_cast<float>(_frameSize.x) * scaleX;
    const float fullH = static_cast<float>(_frameSize.y) * scaleY;

    const float left = c.x - fullW * 0.5f;
    const float top = c.y - fullH * 0.5f;

    const float collW = 140.f * scaleX;

    return sf::FloatRect({ left, top }, { collW, fullH });
}
