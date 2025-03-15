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
        float compression_damping = 0.f;
        float rebound_damping = 0.f;

        // relative to center of rbs #todo: rotate with polygons 
        Toad::Vec2f start_rel;
        Toad::Vec2f end_rel;

        Rigidbody* start_rb;
        Rigidbody* end_rb;
    };
}