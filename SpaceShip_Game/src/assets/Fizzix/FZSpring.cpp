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

        float d = dist(start_pos, end_pos);
        
        Toad::Vec2f dir_norm = normalize(end_pos - start_pos);

        start_rb->velocity += dir_norm * d * stiffness / start_rb->mass;
        end_rb->velocity -= dir_norm * d * stiffness / end_rb->mass;
    }
}