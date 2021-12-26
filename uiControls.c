#include <jo/jo.h>
#include "pcmsys.h"
#include "model.h"
#include "font3D.h"
#include "ui.h"
#include "uiControls.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Button change selection sound */
static short ButtonChange;

/* Button accept click sound */
static short ButtonAccept;

/** @brief Button data
 */
typedef struct
{
    char * Text;
    void (*Action)(bool);
    int Scale;
} ButtonData;

/* Generic button control
 * @widget Button widget
 * @message Control message
 */
void ControlLabel(WidgetsWidget * widget, WidgetMessages message)
{
    if (message == WIDGET_DRAW)
    {
        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix(widget->y, widget->x, -60);
            FontPrintCentered((char *)widget->data, JO_NULL, JO_NULL);
        }
        jo_3d_pop_matrix();
    }
}

/* Generic button control
 * @widget Button widget
 * @message Control message
 */
void ControlButton(WidgetsWidget * widget, WidgetMessages message)
{
    if (message == WIDGET_DRAW)
    {
        jo_3d_push_matrix();
        {
            jo_fixed scale = JO_FIXED_1 + (jo_sin(((ButtonData *)widget->data)->Scale) >> 4);

            if (WidgetsGetCurrent() == widget)
            {
                // If active, make it animate
                ((ButtonData *)widget->data)->Scale += 2;

                if (((ButtonData *)widget->data)->Scale > 179)
                {
                    ((ButtonData *)widget->data)->Scale -= 180;
                }
            }
            else
            {
                if (((ButtonData *)widget->data)->Scale > 90)
                {
                    ((ButtonData *)widget->data)->Scale += 8;
                }
                else if (((ButtonData *)widget->data)->Scale <= 90)
                {
                    ((ButtonData *)widget->data)->Scale -= 8;
                }

                if (((ButtonData *)widget->data)->Scale > 179 || ((ButtonData *)widget->data)->Scale < 1)
                {
                    ((ButtonData *)widget->data)->Scale = 0;
                }
            }
            
            jo_3d_translate_matrix(widget->y, widget->x, -60);
            jo_3d_set_scale_fixed(scale, scale, JO_FIXED_1);
            FontPrintCentered(((ButtonData *)widget->data)->Text, JO_NULL, JO_NULL);
        }
        jo_3d_pop_matrix();
    }
    else if (message == WIDGET_FREE)
    {
        jo_free(widget->data);
    }
    else if (message == WIDGET_INPUT)
    {
        WidgetsWidget * newSelection = JO_NULL;

        if (jo_is_pad1_key_down(JO_KEY_UP))
        {
            newSelection = WidgetsGetPrevSelectable();
        }
        else if (jo_is_pad1_key_down(JO_KEY_DOWN))
        {
            newSelection = WidgetsGetNextSelectable();
        }
        else if (jo_is_pad1_key_down(JO_KEY_A) || jo_is_pad1_key_down(JO_KEY_B) || jo_is_pad1_key_down(JO_KEY_START))
        {
            ((ButtonData *)widget->data)->Action(jo_is_pad1_key_down(JO_KEY_B));
	        pcm_play(ButtonAccept, PCM_PROTECTED, 6);
        }

        if (newSelection != JO_NULL)
        {
            WidgetsSetCurrent(newSelection);
	        pcm_play(ButtonChange, PCM_PROTECTED, 6);
        }
    }
}

// -------------------------------------
// Public
// -------------------------------------

void WidgetsControlsInitialize()
{
	ButtonChange = load_16bit_pcm((Sint8 *)"BUTTON.PCM", 15360);
	ButtonAccept = load_16bit_pcm((Sint8 *)"CLICK.PCM", 15360);
}

WidgetsWidget * WidgetsCreateLabel(int x, int y, const char * const text)
{
    WidgetsWidget * widget = WidgetsCreateWithData(x, y, &ControlLabel, (void *)text);
    widget->IsSelectable = false;
    return widget;
}

WidgetsWidget * WidgetsCreateButton(int x, int y, char * text, void (*action)(bool))
{
    ButtonData * data = jo_malloc(sizeof(ButtonData));
    data->Text = text;
    data->Action = action;
    data->Scale = 0;

    return WidgetsCreateWithData(x, y, &ControlButton, (void *)data);
}
