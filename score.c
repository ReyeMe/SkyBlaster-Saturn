#include <jo/jo.h>
#include "model.h"
#include "font3D.h"
#include "score.h"

// -------------------------------------
// Internal
// -------------------------------------

/** @brief Wiggle score text
 *  @param column Character column
 *  @param row Character row
 *  @param data Character font modifier data
 */
static void ScoreUpdateEffect(const int column, const int row, FontModifier * data)
{
    Score * score = (Score*)data->customData;

    if (column == 0 && row != -1)
    {
        for (int index = 0; index < SCORE_TEXT_LEN; index++)
        {
            if (score->ScoreWiggleAmplitude[index] > 0)
            {
                score->ScoreWiggleAmplitude[index] -= SCORE_WIGGLE_AMPLITUDE_DECAY;

                if (score->ScoreWiggleAmplitude[index] < 0 && index + 1 < SCORE_TEXT_LEN)
                {
                    score->ScoreWiggleAmplitude[index + 1] = SCORE_WIGGLE_AMPLITUDE;
                }
            }
        }
    }

    data->Pos.y += JO_MAX(score->ScoreWiggleAmplitude[column], 0);
}

// -------------------------------------
// Public
// -------------------------------------

void ScoreInitialize(Score * score)
{
    score->Value = 0;
    
    for (int column = 0; column < SCORE_TEXT_LEN; column++)
    {
        score->ScoreWiggleAmplitude[column] = 0;
    }
}

void ScoreSetValue(Score * score, const int newValue)
{
    score->Value = newValue;
}

int ScoreAddValue(Score * score, const int value)
{
    if (score->Value + value >= 0)
    {
        score->Value += value;
        score->ScoreWiggleAmplitude[0] = SCORE_WIGGLE_AMPLITUDE;
    }
    
    return score->Value;
}

void ScoreDraw(Score * score, int loc)
{
    // Prepare text
    char text[SCORE_TEXT_LEN + 1];
    sprintf(text, "%d", score->Value);
    
    // Draw text
    jo_3d_push_matrix();
    {
        jo_3d_translate_matrix(40, loc, -70);
        FontPrint(text, ScoreUpdateEffect, score);
    }
    jo_3d_pop_matrix();
}
