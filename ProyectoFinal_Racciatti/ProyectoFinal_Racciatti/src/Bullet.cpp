#include "Bullet.h"
#include "Level.h"
#include "Utils.h"

void Bullet::Update(float dt, const Level& lvl)
{
    if (!_alive) return;

    auto pos = _shape.getPosition();
    pos.x += _velocity.x * dt;
    pos.y += _velocity.y * dt;
    _shape.setPosition(pos);

    // lifetime
    _lifeTime -= dt;
    if (_lifeTime <= 0)
    {
        _alive = false;
        return;
    }

    // "Destruir" en colision
    const auto& solids = lvl.Colliders();
    const auto myRect = _shape.getGlobalBounds();
    for (const auto& coll : solids)
    {
        if (Utils::RectIntersects(myRect, coll))
        {
            _alive = false;
            break;
        }
    }
}

void Bullet::Draw(sf::RenderTarget& rt) const
{
    if (_alive) rt.draw(_shape);
}