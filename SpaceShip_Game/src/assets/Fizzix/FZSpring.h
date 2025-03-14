#include "FZRigidbody.h"

namespace fz
{
    class Spring
    {
    public:
        void Update(float dt);

        float stiffness = 0.1f; 

        // relative to center of rbs
        Toad::Vec2f start_rel;
        Toad::Vec2f end_rel;

        Rigidbody* start_rb;
        Rigidbody* end_rb;
    };
}