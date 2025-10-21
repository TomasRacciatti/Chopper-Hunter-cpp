#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Level;

enum class CrateType { Health, ShotgunAmmo, RpgAmmo };
class Crate
{
public:
    Crate(sf::Vector2f spawn, CrateType type, sf::Texture& tex, float scale = 1.f);

    void Update(float dt, const Level& lvl);
    void Draw(sf::RenderTarget& rt) const;

    // State
    bool Alive() const { return _alive; }
    void Kill() { _alive = false; }

    CrateType Type()   const { return _type; }
    sf::FloatRect Bounds() const { return _sprite.getGlobalBounds(); }
    sf::Vector2f  Position() const { return _sprite.getPosition(); }

    int HealAmount() const { return _healAmount; }
    int ShotgunAmount() const { return _shotgunAmount; }
    int RpgAmount() const { return _rpgAmount; }

private:
    CrateType _type;
    sf::Sprite _sprite;
    bool _alive = true;

    float _speedY = 260.f;
    bool  _onFloor = false;

    float _lifespanSec = 10.f;
    float _flashLastSec = 2.5f;
    float _ageSec = 0.f;
    float _flashTimer = 0.f;  
    bool  _flashOn = false;

    // Recompensa default
    int _healAmount = 3;
    int _shotgunAmount = 5;
    int _rpgAmount = 2;

    // Helpers
    void ApplyFlash(float dt);
    void ClampToFloor(const Level& lvl);
};