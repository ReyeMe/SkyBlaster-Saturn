#include <jo/jo.h>
#include "tools.h"

jo_fixed ToolsFastVectorLength(const jo_vector_fixed *vector)
{
    // Alpha is 0.9398086351723256
    // Beta is 0.38928148272372454
    // Gama is 0.2987061876143797

    // Get absolute values of the vector components
    jo_fixed x = JO_ABS(vector->x);
    jo_fixed y = JO_ABS(vector->y);
    jo_fixed z = JO_ABS(vector->z);

    // Get min, mid, max
    jo_fixed minYZ = JO_MIN(y, z);
    jo_fixed maxYZ = JO_MAX(y, z);
    jo_fixed min = JO_MIN(x, minYZ);
    jo_fixed max = JO_MAX(x, maxYZ);
    jo_fixed mid = (y < x) ? ((y < z) ? ((z < x) ? z : x) : y) : ((x < z) ? ((z < y) ? z : y) : x);

    // Aproximate vector length (alpha * max + beta * mid + gama * min)
    jo_fixed approximation = jo_fixed_mult(61591, max) + jo_fixed_mult(25512, mid) + jo_fixed_mult(19576, min);
    return JO_MAX(max, approximation);
}

void ToolsEnableLayer(Uint16 layer)
{
    slSynch(); // Clear framebuffer
    slBack1ColSet((void *)BACK_CRAM, CD_Black);
    slColOffsetOn(layer);
    slColOffsetBUse(layer);
    slColOffsetB(0, 0, 0);
}

void ToolsDisableLayer(Uint16 layer)
{
    slBack1ColSet((void *)BACK_CRAM, CD_Black);
    slColOffsetOn(layer);
    slColOffsetBUse(layer);
    slColOffsetB(-TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET);
    slScrPosNbg0(0, 0);
    slScrPosNbg1(0, 0);
}

void ToolsFadeIn(Uint16 layer, void (*drawLoop)(void))
{
    slBack1ColSet((void *)BACK_CRAM, CD_Black);
    slColOffsetB(-TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET);
    slColOffsetOn(layer);
    slColOffsetBUse(layer);

    jo_core_tv_on();

    for (int step = TOOLS_FADE_OFFSET; step >= 0; step -= TOOLS_FADE_SPEED)
    {
        slColOffsetB(-step, -step, -step);

        if (drawLoop != JO_NULL)
        {
            drawLoop();
        }

        slSynch();
    }

    slColOffsetB(0, 0, 0);
}

void ToolsFadeOut(Uint16 layer, void (*drawLoop)(void))
{
    slBack1ColSet((void *)BACK_CRAM, CD_Black);
    slColOffsetOn(layer);
    slColOffsetBUse(layer);

    for (int step = 0; step <= TOOLS_FADE_OFFSET; step += TOOLS_FADE_SPEED)
    {
        slColOffsetB(-step, -step, -step);

        if (drawLoop != JO_NULL)
        {
            drawLoop();
        }

        slSynch();
    }

    slColOffsetB(-TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET);
    slScrPosNbg0(0, 0);
    slScrPosNbg1(0, 0);
    jo_core_tv_off();
}