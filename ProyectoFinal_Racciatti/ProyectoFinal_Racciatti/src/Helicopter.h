#pragma once
#include "Entity.h"
#include "Weapon.h"
#include "ResourceManager.h"
#include "ExplosionEffect.h"
#include <memory>
#include <SFML/Audio.hpp> 

class Player;

class Helicopter : public Entity
{
public:
    explicit Helicopter(sf::Vector2f spawnPos, std::unique_ptr<Weapon> turret, AudioSettings& audio, ResourceManager& resources,
        const std::string& sheetPath, int hp = 10);

    void Update(float dt, const Level& lvl) override;
    void Draw(sf::RenderTarget& rt) const override;

    void SetTarget(sf::Vector2f worldPos) { _targetPos = worldPos; }

    sf::FloatRect GetBounds() const override;

    void TakeDamage(int dmg) override;
    bool IsDying() const { return _dying && _explosion && !_explosion->Finished(); }


private:
    ResourceManager& _resources;
    
    // FSM
    enum class State { Entering, Patrol, Hover };
    State _state = State::Entering;


    // ==== Movement ====
    float _altitude = 160.f;     // Altura a la que va a estar
    float _minX = 32.f;          // Limites para el movimiento de lado a lado
    float _maxX = 0.f;

    // Enter
    float _enterSpeed = 500.f;
    bool _entryTargetReady = false;
    
    // Patrol
    float _minSpeed = 250.f;
    float _maxSpeed = 350.f;
    float _patrolSpeed = 250.f;
    float _patrolTargetX = 0.f; // Punto en ventana a donde se va a mover el heli

    // Hover
    float _minHoverDuration = 0.5f;
    float _maxHoverDuration = 3.5f;
    float _hoverDuration = 2.f; // Esto lo vamos a randomizar
    float _hoverTimer = 0.f;
    float _hoverHorizontalSpeed = 14.f;
    float _hoverBobSpeed = 2.4f;
    float _hoverBobAmp = 8.f;
    float _bobTime = 0.f; // Control del seno


    // Aim
    sf::Vector2f _targetPos{ 0.f, 0.f };

    // Turret
    std::unique_ptr<Weapon> _turret;
    bool _firing = false;
    static constexpr sf::Vector2f kTurretOffsetPx{ 118.f, 49.f };
    sf::Vector2f TurretBaseWorld() const;

    // ====== Anim ======
    sf::Texture* _tex = nullptr;
    sf::Sprite _sprite;
    sf::Vector2i _frameSize{ 177, 51 };
    int _frameCount = 4;
    int _frame = 0;
    int  _animRows = 3;
    int  _animRow = 0;
    float _frameTime = 0.12f;
    float _animTimer = 0.f;

    // Explosion
    bool _dying = false;
    std::unique_ptr<ExplosionEffect> _explosion;

    // Audio
    sf::Sound _explosionSfx;
    sf::Sound _flightSfx;

    // Helpers
    void EnterPatrol(const Level& lvl);
    void EnterHover();
    void PickNewPatrolTarget(const Level& lvl);

    void UpdateEntering(float dt, const Level& lvl);
    void UpdatePatrol(float dt, const Level& lvl);
    void UpdateHover(float dt, const Level& lvl);

    void UpdateAnimation(float dt);
    void UpdateDamageVisual();

    sf::Vector2f Muzzle() const;

    void SetFiring(bool canFire) { _firing = canFire; }

    void StartExplosion();
};

