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
}