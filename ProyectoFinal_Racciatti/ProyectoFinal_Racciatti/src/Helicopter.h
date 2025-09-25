#pragma once
#include "Entity.h"
#include "Weapon.h"
#include "ResourceManager.h"
#include <memory>

class Player;

class Helicopter : public Entity
{
public:
    explicit Helicopter(sf::Vector2f spawnPos, std::unique_ptr<Weapon> turret, ResourceManager& resources,
        const std::string& sheetPath, int hp = 10);

    void Update(float dt, const Level& lvl) override;
    void Draw(sf::RenderTarget& rt) const override;

    void SetTarget(sf::Vector2f worldPos) { _targetPos = worldPos; }

private:
    // FSM
    enum class State { Entering, Patrol, Hover };
    State _state = State::Entering;


    // ==== Movement ====
    float _altitude = 120.f;     // Altura a la que va a estar
    float _minX = 32.f;          // Limites para el movimiento de lado a lado
    float _maxX = 0.f;

    // Enter
    float _enterSpeed = 500.f;
    bool _entryTargetReady = false;
    
    // Patrol
    float _minSpeed = 200.f;
    float _maxSpeed = 300.f;
    float _patrolSpeed = 250.f;
    float _patrolTargetX = 0.f; // Punto en ventana a donde se va a mover el heli

    // Hover
    float _hoverDuration = 2.f; // Esto lo vamos a randomizar entre 1 y 4
    float _hoverTimer = 0.f;
    float _hoverHorizontalSpeed = 14.f;
    float _hoverBobSpeed = 2.4f;
    float _hoverBobAmp = 8.f;
    float _bobTime = 0.f;       // Control del seno


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
    float _frameTime = 0.12f;
    float _animTimer = 0.f;

    // Helpers
    void EnterPatrol(const Level& lvl);
    void EnterHover();
    void PickNewPatrolTarget(const Level& lvl);

    void UpdateEntering(float dt, const Level& lvl);
    void UpdatePatrol(float dt, const Level& lvl);
    void UpdateHover(float dt, const Level& lvl);

    void UpdateAnimation(float dt);

    sf::Vector2f Muzzle() const;

    void SetFiring(bool canFire) { _firing = canFire; }
};

