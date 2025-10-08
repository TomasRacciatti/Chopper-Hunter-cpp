#include "Drone.h"
#include "Utils.h"
#include "Combat.h"
#include "ExplosionEffect.h"
#include <cmath>

namespace
{
    constexpr float _groundSafety = 32.f;
}

Drone::Drone(const sf::Vector2f& spawnPos,
    AudioSettings& audio,
    ResourceManager& resources,
    const std::string& spritePath,
    const sf::Vector2i& spriteFrameSize,
    float visualScale)
    : Entity(spawnPos, { spriteFrameSize.x * visualScale, spriteFrameSize.y * visualScale }, audio, _hp)
    , _resourceManager(resources)
    , _frameSize(spriteFrameSize)
    , _scale(visualScale)
    , _sprite(*_tex)
    , _explosionSfx(resources.GetSound("../audio/sfx/ExplosionDrone.mp3"))
    , _buzzSfx(resources.GetSound("../audio/sfx/DroneFly.mp3"))
{

}
