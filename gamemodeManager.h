#ifndef __GM_MANAGER_H__
#define __GM_MANAGER_H__

/** @brief Mode started
 *  @param data Game data
 */
void GmStart(const GamemodeData * data);

/** @brief Mode tick
 *  @param data Game data
 */
GamemodeTickResult GmTick(const GamemodeData * data);

/** @brief Mode ended
 *  @param data Game data
 */
void GmEnd(const GamemodeData * data);

#endif
