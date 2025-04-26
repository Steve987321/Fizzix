#include "framework/Framework.h"
#include "FZRigidbody.h"

namespace fz
{
    void Rigidbody::Update(float dt)
    {
        center += velocity * velocity_damping * dt;
        angular_velocity *= angular_damping;

        center += center_correction;
        center_correction = Toad::Vec2f{0, 0};

        if (sleeping_ticks * dt > 1.f)
            is_sleeping = true;
        else 
        {
            float abs_vel = abs(velocity.x + velocity.y);
            if (abs_vel <= FLT_EPSILON)
                sleeping_ticks++;
        }
    }
}
