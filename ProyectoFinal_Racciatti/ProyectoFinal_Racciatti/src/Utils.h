#pragma once
#include <SFML/Graphics.hpp>

namespace Utils
{
    inline bool RectIntersects(const sf::FloatRect& a, const sf::FloatRect& b)
    {
        return !(a.position.x + a.size.x <= b.position.x ||
            b.position.x + b.size.x <= a.position.x ||
            a.position.y + a.size.y <= b.position.y ||
            b.position.y + b.size.y <= a.position.y);
    }

    inline float Length(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

    inline sf::Vector2f SafeNormalize(sf::Vector2f v)
    {
        float len = Length(v);
        return (len > 0.f) ? sf::Vector2f{ v.x / len, v.y / len } : sf::Vector2f{ 0.f,0.f };
    }

    inline int RandomInt(int min, int max)
    {
        return rand() % (max - min + 1) + min;
    }

    inline int RandomFloat(float min, float max)
    {
        return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
    }

    inline sf::IntRect FrameRect(int col, int row, int w, int h)
    {
        return sf::IntRect({ col * w, row * h }, { w, h });
    }
}