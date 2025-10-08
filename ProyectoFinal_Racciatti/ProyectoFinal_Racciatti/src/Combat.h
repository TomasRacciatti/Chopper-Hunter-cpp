#pragma once
#include <vector>
#include "Pool.h"
#include "Bullet.h"
#include "Entity.h"
#include "Utils.h"

namespace Combat 
{
    // A cuales objetivos le puede pegar la bala
    inline void ResolveHits(Pool<Bullet>& pool, const std::vector<Entity*>& targets)
    {
        for (auto& bullet : pool.Items()) {
            if (!bullet.Alive()) continue;

            const auto bulletBounds = bullet.GetBounds();
            for (Entity* target : targets) {
                if (!target || !target->IsAlive()) continue;
                if (Utils::RectIntersects(bulletBounds, target->GetBounds())) 
                {
                    target->TakeDamage(bullet.Damage());
                    bullet.Deactivate();
                    break;
                }
            }
        }
    }

    // Sobrecarga de ResolveHits para un target singular
    inline void ResolveHits(Pool<Bullet>& pool, Entity* target)
    {
        if (!target || !target->IsAlive()) return;
        for (auto& bullet : pool.Items()) {
            if (!bullet.Alive()) continue;

            if (Utils::RectIntersects(bullet.GetBounds(), target->GetBounds())) 
            {
                target->TakeDamage(bullet.Damage());
                bullet.Deactivate();
            }
        }
    }

    inline void ExplosionAoE(const sf::Vector2f& center, float radius, int damage, Entity* target)
    {
        if (!target || !target->IsAlive()) return;

        const sf::Vector2f dist = target->Center() - center;
        const float distSquared = dist.x * dist.x + dist.y * dist.y;

        if (distSquared <= radius * radius)
        {
            target->TakeDamage(damage);
        }
    }
}