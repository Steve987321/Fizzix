#pragma once
#include "FZRigidbody.h"

namespace fz
{
    class Spring
    {
    public:
        void Update(float dt);

        float stiffness = 0.1f; 

        float min_len = 0.f;
        float target_len = 0.f;
        float compression_damping = 1.f;
        float rebound_damping = 1.f;
        float rotation_force_factor = 30.f;

        // relative to center of rbs
        Toad::Vec2f start_rel;
        Toad::Vec2f end_rel;

        Rigidbody* start_rb = nullptr;
        Rigidbody* end_rb = nullptr;

    private:
        // update start_rel and end_rel posititions using the rigidbodies angular velocities
        void UpdateRotation(float dt);
    };
}