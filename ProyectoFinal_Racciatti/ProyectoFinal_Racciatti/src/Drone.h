#pragma once
#include <memory>
#include <string>
#include "Entity.h"
#include "Level.h"
#include "ResourceManager.h"
#include "AudioSettings.h"

class ExplosionEffect;

class Drone : public Entity
{
public:
    Drone(const sf::Vector2f& spawnPos,
        AudioSettings& audio,
        ResourceManager& resources,
        const std::string& spritePath,
        const sf::Vector2i& spriteFrameSize,
        float visualScale = 1.0f);

    ~Drone() override;

    void Update(float dt, const Level& lvl) override;
    void Draw(sf::RenderTarget& rt) const override;
    void TakeDamage(int dmg) override;
    sf::FloatRect GetBounds() const override;

    void SetTarget(const sf::Vector2f& worldPos) { _target = worldPos; }
    void SetAoETarget(Entity* target) { _aoeTarget = target; }

    bool IsExploding() const { return _exploding; }

private:
    sf::Texture* _tex = nullptr;
    
    static constexpr int hp = 1;
    
    // Movement
    float _speed = 350.f;
    sf::Vector2f _target{ 0.f, 0.f };
    sf::Vector2f _vel{ 0.f, 0.f };

    // Visuals
    ResourceManager& _resourceManager;
    sf::Sprite _sprite;
    sf::Vector2i _frameSize{ 40, 29 };
    float _scale = 1.0f;
    bool _faceLeft = false;
   
    // Explosion parameters
    bool _exploding = false;
    int _damage = 2;
    float _aoeRadius = 0.f;
    std::unique_ptr<ExplosionEffect> _explosionFx;
    Entity* _aoeTarget = nullptr;

    // Anim
    int _frame = 0;
    int _frameCount = 8;
    float _frameTime = 0.1f;
    float _animTimer = 0.f;
    int _animRow = 0;

    // Explosion anim
    sf::Vector2i _explosionFrameSize{ 48, 48 };
    float _explosionFrameTime = 0.05f;
    int _explosionFrameCount = 6;
    int _hideSourceFromFrame = 2;
    float _explosionScale = 3.0f;

    // Audio
    sf::Sound _buzzSfx;
    sf::Sound _explosionSfx;

    // Helpers
    void UpdateMotion(float dt);
    void UpdateFlipFromVelocity();
    void StartExplosion();
    void UpdateAnimation(float dt);
};

