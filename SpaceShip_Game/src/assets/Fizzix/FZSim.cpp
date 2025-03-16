#include "framework/Framework.h"
#include "FZSim.h"

#include "FZMath.h"

#include "scripts/Sim.h"

namespace fz
{
    bool SegmentIntersection(const Toad::Vec2f& p1, const Toad::Vec2f& p2, const Toad::Vec2f& q1, const Toad::Vec2f& q2, Toad::Vec2f& intersection)
    {
        Toad::Vec2f r = p2 - p1;
        Toad::Vec2f s = q2 - q1;
        
        float rxs = r.Cross(s);
        float qpxr = (q1 - p1).Cross(r);

        if (fabs(rxs) < 1e-6) 
            return false;

        float t = (q1 - p1).Cross(s) / rxs;
        float u = (q1 - p1).Cross(r) / rxs;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        {
            intersection = p1 + r * t;
            return true;
        }

        return false;
    }

    void ClipPolygon(const Polygon& polyA, const Polygon& polyB, const Toad::Vec2f& normal, std::vector<Toad::Vec2f>& contacts)
    {
        for (size_t i = 0; i < polyA.vertices.size(); i++)
        {
            size_t j = (i + 1) % polyA.vertices.size();
            Toad::Vec2f p1 = polyA.vertices[i];
            Toad::Vec2f p2 = polyA.vertices[j];

            for (size_t k = 0; k < polyB.vertices.size(); k++)
            {
                size_t l = (k + 1) % polyB.vertices.size();
                Toad::Vec2f q1 = polyB.vertices[k];
                Toad::Vec2f q2 = polyB.vertices[l];

                Toad::Vec2f intersection;
                if (SegmentIntersection(p1, p2, q1, q2, intersection))
                    contacts.push_back(intersection);
            }
        }
    }

    void ProjectPolygon(const Polygon& poly, const Toad::Vec2f& axis, float& min, float& max)
    {
        min = max = dot(poly.vertices[0], axis);
        for (const Toad::Vec2f& v : poly.vertices)
        {
            float proj = dot(v, axis);
            if (proj < min) min = proj;
            if (proj > max) max = proj;
        }
    }

    bool SAT(const Polygon& a, const Polygon& b, Toad::Vec2f& normal, float& penetration, Toad::Vec2f& contact)
    {
        float min_penetration = std::numeric_limits<float>::max();
        Toad::Vec2f best_normal;
        
        for (const auto& poly : {a, b})
        {
            for (const Toad::Vec2f& axis : poly.normals)
            {
                float minA, maxA, minB, maxB;
                ProjectPolygon(a, axis, minA, maxA);
                ProjectPolygon(b, axis, minB, maxB);
    
                if (maxA < minB || maxB < minA)
                    return false; // no collission
    
                float pen = std::min(maxA - minB, maxB - minA);
                if (pen < min_penetration)
                {
                    min_penetration = pen;
                    best_normal = axis;
                }
            }
        }
        
        // check facing direction and flip if needed
        Toad::Vec2f ab = b.rb.center - a.rb.center;
        if (ab.Dot(best_normal) < 0)
        {
            best_normal = -best_normal;
        }
    
        normal = best_normal;
        penetration = min_penetration;
    
        std::vector<Toad::Vec2f> contacts;
    
        ClipPolygon(a, b, normal, contacts);
        ClipPolygon(b, a, normal, contacts);
    
        if (!contacts.empty())
        {
            contact = Toad::Vec2f(0, 0);
            for (const auto& pt : contacts)
                contact += pt;
            contact /= static_cast<float>(contacts.size());
        }
        else
        {
             // falback
            contact = (a.rb.center + b.rb.center) * 0.5f;
        }
    
        return true;
    }

    void Sim::Update(float dt )
    {
        for (Polygon& p : polygons)
        {
            if (p.rb.is_static)
                continue;

            p.rb.velocity += gravity * dt;
            
            Toad::Vec2f center_prev = p.rb.center; 
            p.rb.Update(dt);
            Toad::Vec2f movement = p.rb.center - center_prev;
            p.rb.center = p.CalcCenterOfMass();
            p.Translate(movement);

            p.Rotate(p.rb.angular_velocity * dt);
        }
    
        for (int i = 0; i < polygons.size(); i++)
        {
            for (int j = i + 1; j < polygons.size(); j++)
            {
                Toad::Vec2f contact, normal;
                float penetration;
                
                bool collide = SAT(polygons[i], polygons[j], normal, penetration, contact);
                
                DrawText("Collision: {}", collide);
                DrawText("Penetration: {}", penetration);

                Toad::DrawingCanvas::DrawArrow(contact, normal * 10.f, 0.5f);

                if (collide) 
                {
                    for (int k = 0; k < 10; k++)
                        Resolve(polygons[i].rb, polygons[j].rb, contact, normal, penetration); // Resolve collision
                }
            }
        }

        for (Spring& spr : springs)
        {
            spr.Update(dt);
        }
    }

    void Sim::Resolve(Rigidbody& a, Rigidbody& b, const Toad::Vec2f& contact, const Toad::Vec2f& normal, float penetration)
    {
        float mass_sum = a.mass + b.mass;

        if (abs(mass_sum) <= FLT_EPSILON)
            return;

        float move_a = (b.mass / mass_sum) * penetration;
        float move_b = (a.mass / mass_sum) * penetration;

        if (!a.is_static)
            a.center -= normal * move_a;
        if (!b.is_static)
            b.center += normal * move_b;

        Toad::Vec2f diff_a = contact - a.center;
        Toad::Vec2f diff_b = contact - b.center;

        Toad::Vec2f rel_vel = b.velocity - a.velocity;
        float vel_along_normal = dot(rel_vel, normal);

        if (vel_along_normal > 0) 
            return; 

        float e = (a.restitution + b.restitution) / 2.f;
        float j = -(1.f + e) * vel_along_normal / (1.f / a.mass + 1.f / b.mass);

        Toad::Vec2f impulse = normal * j;

        if (!a.is_static)
            a.velocity -= impulse / a.mass;
        if (!b.is_static)
            b.velocity += impulse / b.mass;

        // detect sliding and use friction 
        Toad::Vec2f perp = {-normal.y, normal.x};
        
        // Toad::DrawingCanvas::DrawArrow(contact, perp * 10, 1.f);
        // a.slide = dot(perp, normalize(a.velocity));
        // b.slide = dot(perp, normalize(b.velocity));
        // // LOGDEBUGF("PERP: {},{} {},{} A {} B {}",perp.x, perp.y, a.velocity.x, a.velocity.y, vel_perp_a, vel_perp_b);
        // a.velocity += a.slide * a.friction;
        // b.velocity -= b.slide * b.friction;
        // // DrawText("{} {}", a.slide, a.friction);

        // a.velocity *= a.friction;
        // b.velocity *= b.friction;

        float torque_a = cross(diff_a, impulse); 
        float torque_b = cross(diff_b, impulse);

        if (!a.is_static)
        {
            a.angular_velocity -= torque_a / a.moment_of_inertia;
            
            const float angular_velocity_factor = 10.f;
            Toad::Vec2f vel_rot_diff = a.velocity - (perp * (a.angular_velocity * -angular_velocity_factor));
            float grip = std::max(penetration, 1.1f) * ((a.friction + b.friction) / 2.f);

            a.velocity -= vel_rot_diff * grip;
        }
        if (!b.is_static)
            b.angular_velocity += torque_b / b.moment_of_inertia;

        if (penetration > 0.01f)
        {
            Toad::Vec2f correction = normal * (penetration * 0.5f);
            DrawText("CORRECTING: {} {}", correction.x, correction.y);
            
            // apply corection and also check for resting
            // #todo change resting check 
            if (!a.is_static || !a.resting) 
            {
                a.center_correction = -correction;
                if (a.velocity_average.Length() < 0.3f && a.angular_velocity_average < 0.1f)
                {
                    a.resting = true;
                }
            }
            if (!b.is_static || !b.resting)
            {
                b.center_correction = correction;
                if (b.velocity_average.Length() < 0.3f && b.angular_velocity_average < 0.1f)
                    b.resting = true;
            }
        }
    }

}