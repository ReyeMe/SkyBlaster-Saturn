#ifndef __GM_Story_H__
#define __GM_Story_H__

/** @brief Mode started
 *  @param data Game data
 */
void GmStartStory(const GamemodeData * data);

/** @brief Mode tick
 *  @param data Game data
 */
GamemodeTickResult GmTickStory(const GamemodeData * data);

/** @brief Mode ended
 *  @param data Game data
 */
void GmEndStory(const GamemodeData * data);

#endif
