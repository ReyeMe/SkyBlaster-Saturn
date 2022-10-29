#include <jo/jo.h>
#include "tools.h"

void CreateSpriteQuad(jo_3d_quad * quad, const int spriteId)
{
    int width = JO_DIV_BY_2(jo_sprite_get_width(spriteId));
    int height = JO_DIV_BY_2(jo_sprite_get_height(spriteId));
    jo_vertice * vertices = (jo_vertice *)jo_malloc_with_behaviour(4 * sizeof(jo_vertice), JO_MALLOC_TRY_REUSE_BLOCK);
    jo_3d_create_plane(quad, vertices);
    vertices[0].x = JO_MULT_BY_65536(-width);
    vertices[1].x = JO_MULT_BY_65536(width);
    vertices[2].x = JO_MULT_BY_65536(width);
    vertices[3].x = JO_MULT_BY_65536(-width);
    vertices[0].y = JO_MULT_BY_65536(-height);
    vertices[1].y = JO_MULT_BY_65536(-height);
    vertices[2].y = JO_MULT_BY_65536(height);
    vertices[3].y = JO_MULT_BY_65536(height);
    JO_ZERO(vertices[0].z);
    JO_ZERO(vertices[1].z);
    JO_ZERO(vertices[2].z);
    JO_ZERO(vertices[3].z);
    jo_3d_set_texture(quad, spriteId);
    jo_3d_set_screen_doors(quad, false);
    jo_3d_set_light(quad, false);
}

void FreeSpriteQuadData(jo_3d_quad * quad)
{
    jo_free(quad->data.attbl);
    jo_free(quad->data.pltbl);
    jo_free(quad->data.pntbl);
}

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