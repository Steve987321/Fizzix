#include "framework/Framework.h"
#include "FZSim.h"

#include "FZMath.h"

#include "scripts/Sim.h"

namespace fz
{
    using namespace Toad; 

    static bool LineLineIntersection(const Vec2f& p1, const Vec2f& p2, const Vec2f& q1, const Vec2f& q2, Vec2f& intersection)
    {
        Vec2f r = p2 - p1;
        Vec2f s = q2 - q1;
        
        float rxs = r.Cross(s);
        float qpxr = (q1 - p1).Cross(r);

        // check if colinear 
        if (fabs(rxs) < FLT_EPSILON) 
            return false;

        float t = (q1 - p1).Cross(s) / rxs;
        float u = (q1 - p1).Cross(r) / rxs;

        // intersects when t and u are between 0 and 1
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        {
            intersection = p1 + r * t;
            return true;
        }

        return false;
    }

    // find contacts between all edges of a and b 
    static size_t ClipPolygon(const Polygon& a, const Polygon& b, Vec2f& contact)
    {
        size_t intersection_count = 0;
        for (size_t i = 0; i < a.vertices.size(); i++)
        {
            size_t j = (i + 1) % a.vertices.size();
            
            Vec2f p1 = a.vertices[i];
            Vec2f p2 = a.vertices[j];

            for (size_t k = 0; k < b.vertices.size(); k++)
            {
                size_t l = (k + 1) % b.vertices.size();

                Vec2f q1 = b.vertices[k];
                Vec2f q2 = b.vertices[l];

                Vec2f intersection;
                if (LineLineIntersection(p1, p2, q1, q2, intersection))
                {
                    contact += intersection;
                    intersection_count++;
                }
            }
        }

        contact /= intersection_count;

        return intersection_count;
    }

    static void ProjectPolygon(const Polygon& p, const Vec2f& axis, float& min, float& max)
    {
        min = FLT_MAX;
        max = -FLT_MAX; 
        
        for (const Vec2f& v : p.vertices)
        {
            float proj = dot(v, axis);

            if (proj < min)
                min = proj;

            if (proj > max)
                max = proj;
        }
    }

    static bool SAT(const Polygon& a, const Polygon& b, Vec2f& normal, float& overlap, Vec2f& contact, size_t& contact_count)
    {
        float min_overlap = FLT_MAX;
        Vec2f best_normal;
        
        for (const Vec2f& axis : a.normals)
        {
            float min_a, max_a, min_b, max_b;
            ProjectPolygon(a, axis, min_a, max_a);
            ProjectPolygon(b, axis, min_b, max_b);

            if (max_a < min_b || max_b < min_a)
                return false;

            float ab_overlap = std::min(max_a - min_b, max_b - min_a);
            if (ab_overlap < min_overlap)
            {
                min_overlap = ab_overlap;
                best_normal = axis;
            }
        }
        
        for (const Vec2f& axis : b.normals)
        {
            float min_a, max_a, min_b, max_b;
            ProjectPolygon(a, axis, min_a, max_a);
            ProjectPolygon(b, axis, min_b, max_b);

            if (max_a < min_b || max_b < min_a)
                return false;

            float ab_overlap = std::min(max_a - min_b, max_b - min_a);
            if (ab_overlap < min_overlap)
            {
                min_overlap = ab_overlap;
                best_normal = axis;
            }
        }
        
        // check facing direction and flip if needed
        Vec2f ab = b.rb.center - a.rb.center;
        if (ab.Dot(best_normal) < 0)
        {
            best_normal = -best_normal;
        }
    
        normal = best_normal;
        overlap = min_overlap;
    
        // get contact point 
        contact_count = ClipPolygon(a, b, contact);
        
        if (contact_count == 0)
        {
            // fallback
            contact = (a.rb.center + b.rb.center) * 0.5f;
        }
    
        return true;
    }

    static void Resolve(Rigidbody& a, Rigidbody& b, const Vec2f& contact, const Vec2f& normal, float overlap)
    {
        Vec2f diff_a = contact - a.center;
        Vec2f diff_b = contact - b.center;

        Vec2f rel_vel = b.velocity - a.velocity;
        float vel_along_normal = dot(rel_vel, normal);

        if (vel_along_normal > 0) 
        {
            // skip impulse only apply correction
            Vec2f correction = normal * (overlap * 0.5f);
            a.center_correction = -correction;
            b.center_correction = correction;
            return;
        }

        float e = (a.restitution + b.restitution) / 2.f;
        float j = -(1.f + e) * vel_along_normal / (a.inv_mass + b.inv_mass);

        Vec2f impulse = normal * j;

        a.velocity -= impulse * a.inv_mass;
        b.velocity += impulse * b.inv_mass;

        float torque_a = cross(diff_a, impulse); 
        float torque_b = cross(diff_b, impulse);

        Vec2f perp = {-normal.y, normal.x};
        a.angular_velocity -= torque_a / a.moment_of_inertia;
        const float angular_velocity_factor = 10.f;
        Vec2f vel_rot_diff = a.velocity - (perp * (a.angular_velocity * -angular_velocity_factor));
        float grip = std::max(overlap, 1.1f) * ((a.friction + b.friction) / 2.f);
        a.velocity -= vel_rot_diff * grip;

        b.angular_velocity += torque_b / b.moment_of_inertia;
        vel_rot_diff = b.velocity - (perp * (a.angular_velocity * -angular_velocity_factor));
        b.velocity -= vel_rot_diff * grip;

        Vec2f correction = normal * (overlap * 0.5f);
        // DrawText("CORRECTING: {} {}", correction.x, correction.y);
        
        // apply corection and also check for resting
        // #todo change resting check 
        if (a.is_sleeping)
        {
            a.sleeping_ticks = 0;
            a.is_sleeping = false;
        }
        if (!b.is_sleeping)
        {
            b.sleeping_ticks = 0;
            b.is_sleeping = false;       
        }

        a.center_correction = -correction;
        b.center_correction = correction;
    }

    static void BruteForce(std::vector<Polygon>& polygons)
    {
        Vec2f normal;
        float overlap;
        size_t polygons_count = polygons.size(); 

        for (size_t i = 0; i < polygons_count; i++)
        {
            Polygon& a = polygons[i];

            for (size_t j = i + 1; j < polygons_count; j++)
            {
                Polygon& b = polygons[j];

                if (a.rb.is_static && b.rb.is_static)
                    continue;
                if (a.rb.is_sleeping && b.rb.is_sleeping)
                    continue;
                if (a.rb.is_sleeping && b.rb.is_static)
                    continue;
                
                size_t contact_count = 0;
                Vec2f contact;
                bool collide = SAT(a, b, normal, overlap, contact, contact_count);
                
                // DrawText("Collision: {}", collide);
                // DrawText("Overlap: {}", overlap);

                if (collide) 
                    Resolve(a.rb, b.rb, contact, normal, overlap); 
            }
        }
    }

    //
    // SWEEP AND PRUNE USING X AXIS 
    // 

    static bool SortByLeftAxis(const Polygon& a, const Polygon& b)
    {
        return a.aabb.min.x < b.aabb.min.x;
    }

    static void SweepAndPrune(std::vector<Polygon>& polygons)
    {
        // sort by left axis 
        std::ranges::sort(polygons, SortByLeftAxis);

        Vec2f normal; 
        float overlap; 
        size_t polygons_count = polygons.size();

        for (size_t i = 0; i < polygons_count; i++)
        {
            Polygon& a = polygons[i];

            for (size_t j = i + 1; j < polygons_count; j++)
            {
                Polygon& b = polygons[j];

                if (a.rb.is_static && b.rb.is_static)
                    continue;
                if (a.rb.is_sleeping && b.rb.is_sleeping)
                    continue;
                if (a.rb.is_sleeping && b.rb.is_static)
                    continue;
                
                if (b.aabb.min.x > a.aabb.max.x)
                    break;

                // perform sat 
                size_t contact_count = 0;
                Vec2f contact;
                bool collide = SAT(a, b, normal, overlap, contact, contact_count);
                
                if (collide) 
                    Resolve(a.rb, b.rb, contact, normal, overlap); 
            }
        }
    }

    void Sim::Update(float dt)
    {
        for (Spring& spr : springs)
        {
            spr.Update(dt);
        }

        SweepAndPrune(polygons);

        for (Polygon& p : polygons)
        {
            if (p.rb.is_static || p.rb.is_sleeping)
            {
                p.rb.velocity = Vec2f{0, 0};
                p.rb.angular_velocity = 0.f;
                continue;
            }

            p.rb.velocity += gravity * dt;
            
            Vec2f center_prev = p.rb.center; 
            p.rb.Update(dt);
            Vec2f movement = p.rb.center - center_prev;
            p.UpdateCentroid();
            p.Translate(movement);
            p.Rotate(p.rb.angular_velocity * dt);
        }
    }

    fz::Polygon& Sim::AddPolygon(fz::Polygon &polygon)
    {
        polygon.sim = this;
        polygons.emplace_back(polygon);
        return polygons.back();
    }

    fz::Spring& Sim::AddSpring(Polygon &start, Polygon &end, const Vec2f &rel_start, const Vec2f rel_end)
    {
        fz::Spring spring;         
        spring.start_rb = &start.rb;
        spring.start_rel = rel_start;
        spring.end_rb = &end.rb;
        spring.end_rel = rel_end;
        spring.target_len = fz::dist(end.rb.center + rel_end, start.rb.center + spring.start_rel);
        spring.min_len = spring.target_len / 3.f;
        // LOGDEBUGF("{} {} | {} {}", rel_start.x, rel_start.y, rel_end.x, rel_end.y);
        springs.push_back(spring);

        // fz::Spring& res = springs.back();
        // use index this will eventually crash 
        // start.extra_points.emplace_back(&res.start_rel);
        // end.extra_points.emplace_back(&res.end_rel);

        start.attached_spring_points.emplace_back(springs.size() - 1, false);
        end.attached_spring_points.emplace_back(springs.size() - 1, true);
        return springs.back();
    }
}