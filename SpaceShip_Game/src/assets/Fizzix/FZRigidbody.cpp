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

        time += dt;
        if (time > sleep_check_delay)
        {
            time = 0;
            // get center diff 
            Toad::Vec2f center_diff = center - center_prev;
            float abs_vel = abs(center_diff.x + center_diff.y);
            if (abs_vel <= 0.01f)
            {
                LOGDEBUGF("{}", abs_vel);
                is_sleeping = true;
            }
            center_prev = center;
        }
    }
}
