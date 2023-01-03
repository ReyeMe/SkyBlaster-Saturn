#ifndef __MUSIC_HANDLER_H__
#define __MUSIC_HANDLER_H__

/** @brief Set music volume
 *  @param volume Music volume 0-7
 */
void MusicSetVolumeDirect(const int volume);

/** @brief Set the music to play
 *  @param musicToSet Music ID
 *  @param repeat Repeat song
 */
void MusicSetCurrentDirect(const int musicToSet, bool repeat);

/** @brief Set the music to play
 *  @param musicToSet Music ID
 *  @param repeat Repeat song
 */
void MusicSetCurrent(const int musicToSet, bool repeat);

#endif
