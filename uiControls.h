#ifndef __UICONTROLS_H__
#define __UICONTROLS_H__

/** @brief Initialize widget control resources
 */
void WidgetsControlsInitialize();

/** @brief Create widgets label
 *  @param x X location
 *  @param y Y location
 *  @param text Label text
 *  @return Created widget
 */
WidgetsWidget *WidgetsCreateLabel(int x, int y, const char *const text);

/** @brief Create widgets button
 *  @param x X location
 *  @param y Y location
 *  @param text Button text
 *  @param action Button action
 *  @return Created widget
 */
WidgetsWidget *WidgetsCreateButton(int x, int y, char *text, void (*action)(bool));

#endif
