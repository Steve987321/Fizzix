#pragma once

#include "FZRigidbody.h"

namespace fz
{
    class Polygon
    {
    public:
        std::vector<Toad::Vec2f> vertices;
        std::vector<Toad::Vec2f> normals;
    
        Polygon(const std::vector<Toad::Vec2f>& points);

        void CalculateNormals();

        void Translate(const Toad::Vec2f& offset);
    
        void Rotate(float angle);
        
        Toad::Vec2f CalcCenterOfMass();

        // check whether the point is in the polygon
        bool ContainsPoint(const Toad::Vec2f& point);

        Rigidbody rb;
    };
}
