#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "ResourceManager.h"
#include "Utils.h"

class ExplosionEffect
{
public:
    ExplosionEffect(ResourceManager& resourceManager,
                    const std::string& texturePath,
                    const sf::Vector2f& worldPos,
                    sf::Vector2i frameSize,
                    int frameCount,
                    int hideSourceAtFrame = 3,
                    float visualScale = 1.f)
        :_frameSize(frameSize)
        , _frameCount(frameCount)
        , _hideSourceAt(hideSourceAtFrame)
        , _scale(visualScale)
        , _sprite(resourceManager.GetTexture(texturePath, false, {}))
    {
        _sprite.setOrigin({ _frameSize.x * 0.5f, _frameSize.y * 0.5f });

        _sprite.setScale({ _scale, _scale });
        _sprite.setPosition(worldPos);

        _sprite.setTextureRect(Utils::FrameRect(0, 0, _frameSize.x, _frameSize.y));
    }

    void Update(float dt)
    {
        if (_finished) return;

        _timer += dt;
        while (_timer >= _frameTime && !_finished) {
            _timer -= _frameTime;
            _frame++;

            if (_frame >= _frameCount) {
                _finished = true;
                _frame = _frameCount - 1;
            }
        }

        // Los sprite sheets de explosiones son siempre de una unica fila, entonces podemos hardcodear a 0
        const sf::IntRect rect = Utils::FrameRect(_frame, /*row*/0, _frameSize.x, _frameSize.y);
        _sprite.setTextureRect(rect);
        _sprite.setOrigin({ _frameSize.x * 0.5f, _frameSize.y * 0.5f });
    }

    void Draw(sf::RenderTarget& renderTarget) const
    {
        if (_finished) return;
        renderTarget.draw(_sprite);
    }

    bool Finished() const { return _finished; }
    bool ShouldHideSource() const { return _frame >= _hideSourceAt; }

    void SetPosition(const sf::Vector2f& pos) { _sprite.setPosition(pos); }
    void SetScale(float scale) { _scale = scale; _sprite.setScale({ scale, scale }); }

private:
    sf::Vector2i _frameSize;
    int _frameCount = 1;
    float _frameTime = 0.1f;

    float _timer = 0.f;
    int _frame = 0;
    int _hideSourceAt = 0;

    float _scale = 1.f;

    bool  _finished = false;

    sf::Sprite _sprite;
};