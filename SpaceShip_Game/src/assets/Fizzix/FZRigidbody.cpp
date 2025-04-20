#include "framework/Framework.h"
#include "FZRigidbody.h"

namespace fz
{
    void Rigidbody::Update(float dt)
    {
        Toad::Vec2f damping = velocity_damping;
        float rot_damp = angular_damping;
        if (resting)
        {
            damping.x = std::lerp(damping.x, 0.1f, dt * 10);
            damping.y = std::lerp(damping.y, 0.1f, dt * 10);
            rot_damp =std::lerp(rot_damp, 0.1f, dt * 10);
            
            // check unrest 
            if (velocity.Length() > 0.5f || angular_velocity > 0.1f)
                resting = false;
        }
        
        center += velocity * damping * dt;
        angular_velocity *= rot_damp;

        center += center_correction;
        center_correction = Toad::Vec2f{0, 0};
    }
}
