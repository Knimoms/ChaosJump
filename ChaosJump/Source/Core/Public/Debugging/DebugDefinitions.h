#pragma once

#define DRAW_DEBUG_LINES 0
#define DRAW_FPS 1
#define DEBUG_SAT_POLYGON_COLLISION 0
#define DEBUG_SAT_CIRCLE_COLLISION 0
#define DEBUG_SAT_RECTANGLE_COLLISION 0


inline bool ensureCondition(bool condition)
{
    if (!condition)
    {
        __debugbreak();
    }

    return condition;
}

#define ensure(x) ensureCondition(x)
