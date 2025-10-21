#include "Crate.h"
#include "Level.h"
#include <cstdint>  

constexpr float flashFrequency = 8.f;

Crate::Crate(sf::Vector2f spawn, CrateType type, sf::Texture& tex, float scale)
    : _type(type), _sprite(tex)
{
    _sprite.setScale({ scale, scale });
    _sprite.setPosition(spawn);
    _sprite.setColor(sf::Color(255, 255, 255, 255));
}

void Crate::Update(float dt, const Level& lvl)
{
    if (!_alive) return;

    if (!_onFloor)
    {
        _sprite.move({ 0.f, _speedY * dt });
        ClampToFloor(lvl);
    }

    _ageSec += dt;

    if (_lifespanSec > 0.f && (_lifespanSec - _ageSec) <= _flashLastSec)
        ApplyFlash(dt);

    if (_lifespanSec > 0.f && _ageSec >= _lifespanSec)
        _alive = false;
}

void Crate::Draw(sf::RenderTarget& rt) const
{
    if (_alive)
        rt.draw(_sprite);
}

void Crate::ApplyFlash(float dt)
{
    _flashTimer += dt;
    const float period = 1.f / flashFrequency;
    if (_flashTimer >= period)
    {
        _flashTimer -= period;
        _flashOn = !_flashOn;

        const uint8_t alpha = _flashOn ? 255 : 70;
        sf::Color color = _sprite.getColor();
        color.a = alpha;
        _sprite.setColor(color);
    }
}

void Crate::ClampToFloor(const Level& lvl)
{
    const auto floor = lvl.FloorRect();
    const float floorY = floor.position.y;

    const sf::FloatRect bounds = _sprite.getGlobalBounds();
    const float bottom = bounds.position.y + bounds.size.y;

    if (bottom >= floorY)
    {
        const float correction = floorY - bottom;
        _sprite.move({ 0.f, correction });
        _onFloor = true;
    }
}
