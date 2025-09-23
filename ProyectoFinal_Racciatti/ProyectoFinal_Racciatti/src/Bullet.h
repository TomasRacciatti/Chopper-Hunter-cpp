#pragma once
#include <SFML/Graphics.hpp>

class Level;

class Bullet
{
public:
    Bullet() = default;

    void Update(float dt, const Level& lvl);
    void Draw(sf::RenderTarget& rt) const;

    bool Alive() const { return _alive; }
    sf::FloatRect GetBounds() const { return _shape.getGlobalBounds(); }

    int Damage() const { return _damage; }

    // Pool
    void Activate(sf::Vector2f pos, sf::Vector2f vel, float life, int dmg) {
        _shape.setPosition(pos);
        _velocity = vel;
        _lifeTime = life;
        _damage = dmg;
        _alive = true;
    }
    void Deactivate() { _alive = false; }

    void SetRadius(float radius) { _shape.setRadius(radius); _shape.setOrigin({ radius, radius }); }
    void SetColor(const sf::Color& color) { _shape.setFillColor(color); }

private:
    sf::CircleShape _shape;
    sf::Vector2f _velocity {0.f, 0.f};
    float _lifeTime = 5.f;
    int _damage = 1;
    bool _alive = false;
};

