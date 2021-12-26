#ifndef __SCORE_H__
#define __SCORE_H__

#define SCORE_WIGGLE_AMPLITUDE JO_FIXED_1
#define SCORE_WIGGLE_AMPLITUDE_DECAY 15000
#define SCORE_TEXT_LEN 8

/* Score data */
typedef struct {

    /* Score value */
    int Value;

    /* Score text wiggle amplitude */
    jo_fixed ScoreWiggleAmplitude[SCORE_TEXT_LEN];
} Score;

/** @brief Initialize score data
 *  @param score Score to initialize
 */
void ScoreInitialize(Score * score);

/** @brief Set score value
 *  @param score Score data
 *  @param newValue New score value
 */
void ScoreSetValue(Score * score, int newValue);

/** @brief Add score value
 *  @param score Score data
 *  @param value Value to be added to score
 *  @return New set value
 */
int ScoreAddValue(Score * score, int value);

/** @brief Draw score text
 *  @param score Score data
 */
void ScoreDraw(Score * score);

#endif
