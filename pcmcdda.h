#ifndef __PCMCDDA_H__
#define __PCMCDDA_H__

/** @brief Set CD playback volume (7 is max)
 *  @param left Left channel volume
 *  @param right Right channel volume
 */
void CDDASetVolume(const Uint8 left, const Uint8 right);

/** @brief Initialize CD playback (just sets volume to 7)
 */
void CDDAInitialize();

/** @brief Play range of tracks
 *  @param fromTrack Starting track
 *  @param toTrack Ending track
 *  @param loop Whether to play the range of track again after it ends
 */
void CDDAPlay(const int fromTrack, const int toTrack, const bool loop);

/** @brief Play single CD track
 *  @param track Track number (in jo starts at 3)
 *  @param loop Whether to loop track after it ends
 */
void CDDAPlaySingle(const int track, const bool loop);

/** @brief Stop CD music playback
 */
void CDDAStop();

#endif
