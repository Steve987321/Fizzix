#pragma once

#include "FZPolygon.h"
#include "FZRigidbody.h"

namespace fz
{
    class Sim
    {
    public:
        void Update(float dt);
        void Resolve(Rigidbody& a, Rigidbody& b, const Toad::Vec2f& contact, const Toad::Vec2f& normal, float penetration);

        Toad::Vec2f gravity{0.f, 9.8f};
        std::vector<Polygon> polygons {};
    };
}
