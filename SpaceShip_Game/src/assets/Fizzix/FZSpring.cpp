#include "framework/Framework.h"
#include "FZMath.h"
#include "FZSpring.h"

namespace fz
{
    void Spring::Update(float dt)
    {
        if (!start_rb || !end_rb)
            return;

        Toad::Vec2f start_pos = start_rb->center + start_rel;
        Toad::Vec2f end_pos = end_rb->center + end_rel;
        Toad::Vec2f dir_norm = normalize(end_pos - start_pos);

        if (target_len != 0)
        {
            float d1 = dist(start_pos, end_pos);
            float correct = target_len - d1;
            Toad::Vec2f start_pos_a = start_rb->center + start_rel;
            Toad::Vec2f end_pos_a = ((end_pos + Toad::Vec2f(-dir_norm * correct / 2.f)) + start_pos_a + dir_norm * correct / 2.f) / 2.f;
            float d = dist(start_pos_a, end_pos_a);
            // Toad::DrawingCanvas::DrawArrow(start_pos, dir_norm * 2.f, 1.f, Toad::Color::Blue);
            Toad::DrawingCanvas::DrawArrow(end_pos_a, {0, 5.f}, 1.f, Toad::Color::Blue);
            // Toad::DrawingCanvas::DrawText(end_pos_a, std::to_string(correct), 10);

            float stiffy = stiffness;
            if (correct > 0)
            {
                dir_norm *= -1;
                stiffy *= compression_damping;
            }
            else 
                stiffy *= rebound_damping;
            
            Toad::Vec2f start_rb_force = dir_norm * d * stiffy / start_rb->mass;
            Toad::Vec2f end_rb_force = dir_norm * d * stiffy / end_rb->mass;

            start_rb->velocity += start_rb_force;
            end_rb->velocity -= end_rb_force;
            
            // apply angular velocity 
            float torque_start = cross(start_rel, start_rb_force * rotation_force_factor);
            float torque_end = cross(end_rel, end_rb_force * rotation_force_factor);
            // Toad::DrawingCanvas::DrawText(end_pos_a, std::to_string(torque_start), 10);

            start_rb->angular_velocity += torque_start / start_rb->moment_of_inertia;
            end_rb->angular_velocity -= torque_end / end_rb->moment_of_inertia;
            
            UpdateRotation(dt);

            return; 
        }
        
        float d = dist(start_pos, end_pos);

        if (d < min_len)
        {
            float correct = min_len - d;
            start_rb->center_correction -= dir_norm * correct / 2.f;
            end_rb->center_correction += dir_norm * correct / 2.f;
            d = min_len;
        }

        start_rb->velocity += dir_norm * d * stiffness / start_rb->mass;
        end_rb->velocity -= dir_norm * d * stiffness / end_rb->mass;
    }

    void Spring::UpdateRotation(float dt)
    {
        float start_angle = start_rb->angular_velocity * dt;
        float end_angle = end_rb->angular_velocity * dt;

        float start_cos = cos(start_angle);
        float start_sin = sin(start_angle);
        start_rel.x = start_rel.x * start_cos - start_rel.y * start_sin;
        start_rel.y = start_rel.x * start_sin + start_rel.y * start_cos;

        float end_cos = cos(end_angle);
        float end_sin = sin(end_angle);
        end_rel.x = end_rel.x * end_cos - end_rel.y * end_sin;
        end_rel.y = end_rel.x * end_sin + end_rel.y * end_cos;
    }
}