#pragma once
#include <memory>
#include <string>
#include <array>
#include "Entity.h"
#include "Level.h"
#include "ResourceManager.h"
#include "AudioSettings.h"

class ExplosionEffect;

class Artillery : public Entity
{
public:
    Artillery(const sf::Vector2f& spawnPos,
        AudioSettings& audio,
        ResourceManager& resources,
        const std::string& spritePath,
        const sf::Vector2i& spriteFrameSize,
        float visualScale = 1.0f);

    ~Artillery() override;

    void Update(float dt, const Level& lvl) override;
    void Draw(sf::RenderTarget& rt) const override;
    void TakeDamage(int dmg) override;
    sf::FloatRect GetBounds() const override;

    void SetAoETarget(Entity* target) { _aoeTarget = target; }

private:
    static constexpr int hp = 1;

    // Movimiento puramente vertical
    float _speedY = 400.f;
    sf::Vector2f _vel{ 0.f, 0.f };

    // Visual
    ResourceManager& _resourceManager;
    sf::Texture* _tex = nullptr; 
    sf::Sprite _sprite;
    sf::Vector2i _frameSize{ 64, 64 };
    float _scale = 1.0f;

    // Explosion parameters
    bool _exploding = false;
    int   _damage = 5;
    std::unique_ptr<ExplosionEffect> _explosionFx;
    Entity* _aoeTarget = nullptr;

    // Explosion anim
    sf::Vector2i _explosionFrameSize{ 128, 128 };
    float _explosionFrameTime = 0.05f;
    int _explosionFrameCount = 12;
    int _hideSourceFromFrame = 2;
    float _explosionScale = 1.0f;

    // Audio
    sf::Sound _whistleSfx;
    sf::Sound _explosionSfx;

    // AoE por frame
    std::array<float, 6> _aoeRadii{ 17.f, 23.f, 28.f, 41.f, 45.f, 51.f };
    int _aoeLastFrame = 5;
    bool _aoeApplied = false;
    float _explosionElapsed = 0.f;
    sf::Vector2f _explosionCenterPixel{ 64.f, 102.f };

    // Helpers
    void StartExplosion();
    sf::Vector2f ExplosionAoECenterWorld() const;
};

