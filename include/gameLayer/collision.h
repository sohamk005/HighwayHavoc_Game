#pragma once

#include <glm/vec2.hpp>
#include <algorithm>

namespace collision
{
    struct AABB
    {
        glm::vec2 pos;
        glm::vec2 size;
    };

    inline bool checkAABBOverlap(const glm::vec2& posA, const glm::vec2& sizeA,
                                 const glm::vec2& posB, const glm::vec2& sizeB)
    {
        if (sizeA.x <= 0.f || sizeA.y <= 0.f || sizeB.x <= 0.f || sizeB.y <= 0.f)
            return false;

        return (posA.x < posB.x + sizeB.x &&
                posA.x + sizeA.x > posB.x &&
                posA.y < posB.y + sizeB.y &&
                posA.y + sizeA.y > posB.y);
    }

    inline bool checkAABBOverlap(const AABB& a, const AABB& b)
    {
        return checkAABBOverlap(a.pos, a.size, b.pos, b.size);
    }

    // Resolves overlap by pushing posA out of posB along the axis of minimum penetration.
    inline bool resolveAABBCollision(glm::vec2& posA, const glm::vec2& sizeA,
                                     const glm::vec2& posB, const glm::vec2& sizeB,
                                     float separationEpsilon = 1.0f)
    {
        if (!checkAABBOverlap(posA, sizeA, posB, sizeB))
            return false;

        float overlapLeft   = (posA.x + sizeA.x) - posB.x;
        float overlapRight  = (posB.x + sizeB.x) - posA.x;
        float overlapTop    = (posA.y + sizeA.y) - posB.y;
        float overlapBottom = (posB.y + sizeB.y) - posA.y;

#ifndef NOMINMAX
#define NOMINMAX
#endif

        float overlapX = (std::min)(overlapLeft, overlapRight);
        float overlapY = (std::min)(overlapTop, overlapBottom);

        if (overlapY < overlapX)
        {
            if ((posA.y + sizeA.y * 0.5f) > (posB.y + sizeB.y * 0.5f))
            {
                posA.y = posB.y + sizeB.y + separationEpsilon;
            }
            else
            {
                posA.y = posB.y - sizeA.y - separationEpsilon;
            }
        }
        else
        {
            if ((posA.x + sizeA.x * 0.5f) > (posB.x + sizeB.x * 0.5f))
            {
                posA.x = posB.x + sizeB.x + separationEpsilon;
            }
            else
            {
                posA.x = posB.x - sizeA.x - separationEpsilon;
            }
        }

        return true;
    }
}
