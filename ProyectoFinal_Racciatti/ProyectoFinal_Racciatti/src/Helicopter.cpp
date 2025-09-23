#include "Helicopter.h"
#include "Level.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>

Helicopter::Helicopter(sf::Vector2f spawnPos, std::unique_ptr<Weapon> turret)
    : Entity(spawnPos, { 96.f, 40.f }, _hp)   // Placeholder para debug, despues reemplazar con sprite
    , _turret(std::move(turret))
{
    _body.setFillColor(sf::Color(160, 160, 160));   // Color temporal
    _body.setOutlineThickness(1.f);
    _body.setOutlineColor(sf::Color(255, 255, 255));
}

void Helicopter::Update(float dt, const Level& lvl)
{
    switch (_state) 
    {
        case State::Entering: UpdateEntering(dt, lvl); break;
        case State::Patrol:   UpdatePatrol(dt, lvl); break;
        case State::Hover:    UpdateHover(dt, lvl); break;
    }

    // Turret siempre esta apuntando al jugador
    if (_turret) 
    {
        const auto muzzle = Muzzle();
        _turret->Update(dt, /*fireHeld*/true, muzzle, _targetPos, lvl);
    }
}

void Helicopter::Draw(sf::RenderTarget& rt) const
{
    Entity::Draw(rt);
    if (_turret) _turret->Draw(rt);
}


//      ===== FSM =====

// Trancisiones
void Helicopter::EnterPatrol(const Level& lvl)
{
    _state = State::Patrol;
    _hoverTimer = 0.f;
    _bobTime = 0.f;
    PickNewPatrolTarget(lvl);
}

void Helicopter::EnterHover()
{
    _state = State::Hover;
    _hoverDuration = Utils::RandomFloat(1.f, 4.f);
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

void Helicopter::UpdateEntering(float dt, const Level&)
{
    auto pos = _body.getPosition();
    if (pos.y < _altitude) 
    {
        pos.y += _enterSpeed * dt;
        if (pos.y >= _altitude) 
        {
            pos.y = _altitude;
            EnterHover();
        }
    }
    _body.setPosition(pos);
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

sf::Vector2f Helicopter::Muzzle() const
{
    const auto rect = _body.getGlobalBounds();
    const float muzzleX = rect.position.x + rect.size.x * 0.80f;
    const float muzzleY = rect.position.y + rect.size.y * 0.45f;
    return { muzzleX, muzzleY };
}
