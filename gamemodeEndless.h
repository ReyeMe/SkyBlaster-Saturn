#ifndef __GM_ENDLESS_H__
#define __GM_ENDLESS_H__

/** @brief Mode started
 *  @param data Game data
 */
void GmStartEndless(const GamemodeData * data);

/** @brief Mode tick
 *  @param data Game data
 */
GamemodeTickResult GmTickEndless(const GamemodeData * data);

/** @brief Mode ended
 *  @param data Game data
 */
void GmEndEndless(const GamemodeData * data);

#endif
