#ifndef __TOOLS_H_
#define __TOOLS_H_

#define TOOLS_FADE_OFFSET 255
#define TOOLS_FADE_SPEED 8

/** @brief Create a Sprite Quad object
 * @param quad Quad data
 * @param spriteId Sprite ID
 */
void CreateSpriteQuad(jo_3d_quad * quad, const int spriteId);

/** @brief Free sprite quad object
 * @param quad Sprite quad
 */
void FreeSpriteQuadData(jo_3d_quad * quad);

/** @brief Fast length of 3D vector (Thx GValiente for solution).
 *  For more info about how it works see: https://math.stackexchange.com/questions/1282435/alpha-max-plus-beta-min-algorithm-for-three-numbers
 *  And also: https://en.wikipedia.org/wiki/Alpha_max_plus_beta_min_algorithm
 *  @param vector Vector to measure
 *  @return Approximation of the vector length
 */
jo_fixed ToolsFastVectorLength(const jo_vector_fixed *vector);

/** @brief Enable specific layer
 *  @param layer Layer number
 */
void ToolsEnableLayer(Uint16 layer);

/** @brief Disable specific layer
 *  @param layer Layer number
 */
void ToolsDisableLayer(Uint16 layer);

/** @brief Fade in effect
 *  @param layer Layer number
 *  @param draw_loop Draw function to call while fading
 */
void ToolsFadeIn(Uint16 layer, void (*drawLoop)(void));

/** @brief Fade out effect
 *  @param layer Layer number
 *  @param draw_loop Draw function to call while fading
 */
void ToolsFadeOut(Uint16 layer, void (*drawLoop)(void));

/** @brief Draw rectangle line
 *  @param first First point
 *  @param second Second point
 *  @param z Depth coordinate
 *  @param color Line color
 */
void ToolsDrawLine(
    const jo_pos2D *first, 
    const jo_pos2D *second,
    int z,
    jo_color color);

/** @brief Draw polygon
 *  @param first First point
 *  @param second Second point
 *  @param third Third point
 *  @param fourth Fourth point
 *  @param z Depth coordinate
 *  @param color Line color
 *  @param filled Is polygon filled
 */
void ToolsDrawPolygon(
    const jo_pos2D *first, 
    const jo_pos2D *second,
    const jo_pos2D *third,
    const jo_pos2D *fourth,
    const int z,
    const jo_color color,
    const bool filled);

/** @brief Draw rectangle line
 *  @param center Center point
 *  @param size Rectangle size
 *  @param angle Rotation angle
 *  @param z Depth coordinate
 *  @param color Line color
 *  @param filled Is rectangle filled
 */
void ToolsDrawRotatedRectangle(
    const jo_pos2D *center, 
    const jo_size *size,
    const ANGLE angle,
    const int z,
    const jo_color color,
    const bool filled);

/** @brief Draw rectangle
 *  @param center Center point
 *  @param size Rectangle size
 *  @param z Depth coordinate
 *  @param color Line color
 *  @param filled Is rectangle filled
 */
void ToolsDrawRectangle(
    const jo_pos2D *center, 
    const jo_size *size,
    const int z,
    const jo_color color,
    const bool filled);

/** @brief Really fast acos, can have up to 10 deg of error, but good enough
 * @param angle Angle to get arcus cosin of
 * @return jo_fixed Arcus cosin
 */
jo_fixed ToolsFastAcos(const jo_fixed angle);

#endif
