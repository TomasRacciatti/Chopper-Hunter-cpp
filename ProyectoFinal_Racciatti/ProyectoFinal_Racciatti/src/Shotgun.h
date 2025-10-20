#pragma once
#include "Weapon.h"

class Shotgun : public Weapon
{
public:
	using Weapon::Weapon;

protected:
	bool Shoot(sf::Vector2f origin, sf::Vector2f dir) override;

private:
	int _pellets = 5;
	float _spreadAngle = 10.f;
};

