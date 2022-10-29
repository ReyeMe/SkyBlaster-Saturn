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

#endif
