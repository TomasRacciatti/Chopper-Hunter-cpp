#pragma once
#include "Weapon.h"
class Pistol: public Weapon
{
public:
	using Weapon::Weapon;

protected:
	void Shoot(sf::Vector2f origin, sf::Vector2f dir) override;
};

