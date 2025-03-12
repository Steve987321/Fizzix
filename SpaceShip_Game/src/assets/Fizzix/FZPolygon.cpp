#include "framework/Framework.h"
#include "FZRigidbody.h"
#include "FZPolygon.h"
#include "FZMath.h"

namespace fz
{
    Polygon::Polygon(const std::vector<Toad::Vec2f>& points)
        : vertices(points)
    {
        CalculateNormals();
        rb.center = CalcCenterOfMass();

        float inertia = 0.0f;
        size_t numVertices = vertices.size();

        for (size_t i = 0; i < numVertices; i++) 
        {
            Toad::Vec2f v1 = vertices[i];
            Toad::Vec2f v2 = vertices[(i + 1) % numVertices];
            
            // I = m * (r1^2 + r2^2 + r1*r2)
            float r1 = length(v1 - rb.center);
            float r2 = length(v2 - rb.center);
            inertia += (r1 * r1 + r2 * r2 + r1 * r2);
        }
        rb.moment_of_inertia = inertia;
    }

    void Polygon::CalculateNormals()
    {
        normals.clear();
        int n = vertices.size();
        for (int i = 0; i < n; i++)
        {
            Toad::Vec2f edge = vertices[(i + 1) % n] - vertices[i];
            Toad::Vec2f normal(-edge.y, edge.x); 
            normals.push_back(normalize(normal));
        }
    }

    void Polygon::Translate(const Toad::Vec2f& offset)
    {
        for (auto& v : vertices)
            v += offset;
        rb.center = rb.center + offset;
    }

    void Polygon::Rotate(float angle)
    {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        for (auto& v : vertices)
        {
            float x = v.x - rb.center.x;
            float y = v.y - rb.center.y;
            v.x = rb.center.x + x * cosA - y * sinA;
            v.y = rb.center.y + x * sinA + y * cosA;
        }
        CalculateNormals();
    }

    Toad::Vec2f Polygon::CalcCenterOfMass()
    {
        float area = 0;
        float cx = 0, cy = 0;
        int n = vertices.size();

        for (int i = 0; i < n; i++)
        {
            int j = (i + 1) % n;
            float c = cross(vertices[i], vertices[j]);
            area += c;
            cx += (vertices[i].x + vertices[j].x) * c;
            cy += (vertices[i].y + vertices[j].y) * c;
        }

        area *= 0.5f;
        if (std::abs(area) < 1e-6)
            return Toad::Vec2f(0, 0); 

        return Toad::Vec2f(cx / (6 * area), cy / (6 * area));
    }

}