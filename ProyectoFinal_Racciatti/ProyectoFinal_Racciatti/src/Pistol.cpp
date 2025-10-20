#include "Pistol.h"

// Disparo singular en la direccion donde se este apuntando
bool Pistol::Shoot(sf::Vector2f origin, sf::Vector2f dir)
{
    return EmitBullet(origin, dir) != nullptr;
}
