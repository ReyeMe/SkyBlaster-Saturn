#ifndef __UI_H__
#define __UI_H__

/* Widget messages */
typedef enum WidgetMessages
{
    /* Empty message */
    WIDGET_EMPTY = 0,

    /* Instructs widget to read gamepad */
    WIDGET_INPUT = 1,

    /* Instructs widget to draw it self */
    WIDGET_DRAW = 2,

    /* Instruct widget to initialize */
    WIDGET_INIT = 3,

    /* Instruct widget to release all resources */
    WIDGET_FREE = 4,

    /* Notifies widget that it has been selected */
    WIDGET_SELECTED = 5,

    /* Notifies widget that it has been deselected */
    WIDGET_DESELECTED = 6
} WidgetMessages;

/*
 * UI widget element
 */
typedef struct _W_widget WidgetsWidget;
typedef struct _W_widget
{
    /* X screen coordinate */
    int x;

    /* Y screen coordinate */
    int y;

    /* Widget message process fuction 
     * MUST NOT BE EMPTY!
     */
    void (*wProc)(WidgetsWidget*,WidgetMessages);

    /* User data storage, user must initialize and clear it by him self*/
    void * data;

    /* If set to TRUE, widget will not recieve draw message */
    bool IsVisible;

    /* If set to TRUE, widget is selectable */
    bool IsSelectable;
} WidgetsWidget;

/* Delete widget
 * @param widget Widget to delete
 */
void WidgetsClear(WidgetsWidget * widget);

/* Clear all controls
 */
void WidgetsClearAll();

/*
 * Create widget and register it
 * @param x Widget X position
 * @param y Widget Y position
 * @param wProc Widget message process function
 * @return Created widget instance
 */
WidgetsWidget * WidgetsCreate(int x, int y, void (*wProc)(WidgetsWidget*,WidgetMessages));

/*
 * Create widget and register it
 * @param x Widget X position
 * @param y Widget Y position
 * @param wProc Widget message process function
 * @param data User data
 * @return Created widget instance
 */
WidgetsWidget * WidgetsCreateWithData(int x, int y, void (*wProc)(WidgetsWidget*,WidgetMessages), void * data);

/* Sends process input message to currently selected widget*/
void WidgetsInvokeInput();

/* Get list of all widget nodes */
jo_list * WidgetsGetAll();

/* Get current widget node */
WidgetsWidget * WidgetsGetCurrent();

/* Get next available widget node */
WidgetsWidget * WidgetsGetNextSelectable();

/* Get previous available widget node */
WidgetsWidget * WidgetsGetPrevSelectable();

/* Initialize UI */
void WidgetsInitialize();

/* Sends redraw message to visible widgets */
void WidgetsRedraw();

/* Set active widget node 
 * @widget Widget to set as active
 * @return True on success
 */
bool WidgetsSetCurrent(WidgetsWidget * widget);

#endif
