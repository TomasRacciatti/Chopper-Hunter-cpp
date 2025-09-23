#include "Pistol.h"

// Disparo singular en la direccion donde se este apuntando
void Pistol::Shoot(sf::Vector2f origin, sf::Vector2f dir)
{
    EmitBullet(origin, dir);
}
