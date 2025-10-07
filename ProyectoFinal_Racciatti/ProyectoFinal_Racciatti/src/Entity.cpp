#include "Entity.h"


Entity::Entity(sf::Vector2f pos, sf::Vector2f size, AudioSettings& audio, int health)
    : _audio(audio), _health(health), _maxHealth(health)
{
    _body.setSize(size);
    _body.setPosition(pos);
    _body.setFillColor(sf::Color(225, 225, 225)); // color de placeholder
}

void Entity::Draw(sf::RenderTarget& rt) const
{
    rt.draw(_body);
}

// No creo que lo use en el juego final pero viene bien para cheats a la hora de debugear
void Entity::SetLife(int value)
{
    _health = value;
    _alive = (_health > 0);
}

void Entity::TakeDamage(int dmg)
{
    if (!_alive) 
        return;

    _health -= dmg;

    if (_health <= 0) 
    { 
        _health = 0; 
        _alive = false; 
    }
}

sf::FloatRect Entity::GetBounds() const
{
    // Puedo hacer esta func virtual, El player la overridea y 
    // si esta normal deja el get bound, pero si esta agachado retorno el getbounds 
    // achicado en Y
    return _body.getGlobalBounds();
}

sf::Vector2f Entity::Center() const
{
    const auto body = _body.getGlobalBounds();
    return body.getCenter();
}