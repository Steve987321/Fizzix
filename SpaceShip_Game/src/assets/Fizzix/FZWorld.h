#pragma once

#include "FZPolygon.h"
#include "FZRigidbody.h"
#include "FZSpring.h"

namespace fz
{
    class World
    {
    public:
        World();
        
        void Update(float dt);

        fz::Polygon& AddPolygon(fz::Polygon& polygon);

        // spawns spring with min_len set to third of the spring distance (target_len)
        fz::Spring& AddSpring(Polygon& start, Polygon& end, const Toad::Vec2f& rel_start, const Toad::Vec2f rel_end);

        void UpdatePolygonVertices(uint32_t polygon_index);
        void ResetRenderingState();

        Toad::Vec2f gravity{0.f, 9.8f};
        std::vector<Polygon> polygons {};
        std::vector<Spring> springs;

        Toad::DrawingCanvas dc;
    };
}
