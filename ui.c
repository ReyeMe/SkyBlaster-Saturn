#include <jo/jo.h>
#include "ui.h"

/* Current screen widgets */
static jo_list *widgets;

/* Current active widget */
static jo_node *currentWidget;

void WidgetsClear(WidgetsWidget *widget)
{
    jo_node *node = JO_NULL;

    for (node = widgets->first; node != JO_NULL && widgets->count != 0; node = node->next)
    {
        // Find widget
        if (node->data.ptr == widget)
        {
            // De-select
            if (currentWidget->data.ptr == widget)
                currentWidget = JO_NULL;

            // Send free mesage and clear widget
            WidgetsWidget *toFree = ((WidgetsWidget *)node->data.ptr);
            toFree->wProc(toFree, WIDGET_FREE);
            jo_free(toFree);

            // clear entry from list
            jo_node *prev = node->prev;
            jo_node *next = node->next;

            if (prev != JO_NULL)
            {
                if (next != JO_NULL)
                {
                    prev->next = next;
                    next->prev = prev;
                }
                else
                {
                    prev->next = JO_NULL;
                    widgets->last = prev;
                }
            }
            else
            {
                if (next != JO_NULL)
                {
                    next->prev = JO_NULL;
                    widgets->first = next;
                }
                else
                {
                    widgets->first = JO_NULL;
                    widgets->last = JO_NULL;
                }
            }

            jo_free(node);
            --widgets->count;
            return;
        }
    }
}

void WidgetsClearAll()
{
    jo_node *node = JO_NULL;

    for (node = widgets->first; node != JO_NULL; node = node->next)
    {
        WidgetsWidget *widget = ((WidgetsWidget *)node->data.ptr);
        widget->wProc(widget, WIDGET_FREE);
        jo_free(widget);
    }

    currentWidget = JO_NULL;
    jo_list_clear(widgets);
}

WidgetsWidget *WidgetsCreateWithData(int x, int y, void (*wProc)(WidgetsWidget *, WidgetMessages), void *data)
{
    WidgetsWidget *created = jo_malloc(sizeof(WidgetsWidget));
    
    if (created == JO_NULL)
    {
        jo_core_error("Out of memory");
    }

    created->x = x;
    created->y = y;
    created->IsVisible = true;
    created->IsSelectable = true;
    created->wProc = wProc;
    created->data = data;
    created->wProc(created, WIDGET_INIT);
    jo_list_add_ptr(widgets, created);
    return created;
}

WidgetsWidget *WidgetsCreate(int x, int y, void (*wProc)(WidgetsWidget *, WidgetMessages))
{
    return WidgetsCreateWithData(x, y, wProc, JO_NULL);
}

void WidgetsInvokeInput()
{
    if (currentWidget != JO_NULL)
    {
        WidgetsWidget *widget = ((WidgetsWidget *)currentWidget->data.ptr);
        widget->wProc(widget, WIDGET_INPUT);
    }
}

WidgetsWidget *WidgetsById(const int id)
{
    if (id < 0 || id >= widgets->count)
    {
        return JO_NULL;
    }

    return (WidgetsWidget *)jo_list_at(widgets, id)->data.ptr;
}

int WidgetsGetId(const WidgetsWidget * widget)
{
    jo_node *node;
    int counter = 0;

    for (node = widgets->first; node != NULL && widgets->count != 0; node = node->next)
    {
        if ((WidgetsWidget *)node->data.ptr == widget)
        {
            return counter;
        }

        counter++;
    }

    return -1;
}

jo_list *WidgetsGetAll()
{
    return widgets;
}

WidgetsWidget *WidgetsGetCurrent()
{
    return currentWidget != JO_NULL ? ((WidgetsWidget *)currentWidget->data.ptr) : JO_NULL;
}

WidgetsWidget *WidgetsGetNextSelectable()
{
    if (currentWidget != JO_NULL)
    {
        jo_node *node;

        for (node = currentWidget->next; node != NULL && widgets->count != 0; node = node->next)
        {
            if (((WidgetsWidget *)node->data.ptr)->IsVisible && ((WidgetsWidget *)node->data.ptr)->IsSelectable)
            {
                return (WidgetsWidget *)node->data.ptr;
            }
        }
    }

    return JO_NULL;
}

WidgetsWidget *WidgetsGetPrevSelectable()
{
    if (currentWidget != JO_NULL)
    {
        jo_node *node;

        for (node = currentWidget->prev; node != NULL && widgets->count != 0; node = node->prev)
        {
            if (((WidgetsWidget *)node->data.ptr)->IsVisible && ((WidgetsWidget *)node->data.ptr)->IsSelectable)
            {
                return (WidgetsWidget *)node->data.ptr;
            }
        }
    }
    return JO_NULL;
}

void WidgetsInitialize()
{
    widgets = jo_malloc(sizeof(jo_list));

    if (widgets == JO_NULL)
    {
        jo_core_error("Out of memory");
    }

    jo_list_init(widgets);
}

void WidgetsRedraw()
{
    jo_node *node;

    for (node = widgets->first; node != JO_NULL && widgets->count != 0; node = node->next)
    {
        WidgetsWidget *widget = ((WidgetsWidget *)node->data.ptr);

        if (widget->IsVisible)
        {
            widget->wProc(widget, WIDGET_DRAW);
        }
    }
}

bool WidgetsSetCurrent(WidgetsWidget *widget)
{
    if (widget != JO_NULL)
    {
        jo_node *node;
        WidgetsWidget *current = currentWidget != JO_NULL ? (WidgetsWidget *)currentWidget->data.ptr : JO_NULL;

        for (node = widgets->first; node != JO_NULL && widgets->count != 0; node = node->next)
        {
            if (((WidgetsWidget *)node->data.ptr) == widget && 
                ((WidgetsWidget *)node->data.ptr)->IsSelectable &&
                ((WidgetsWidget *)node->data.ptr)->IsVisible)
            {
                currentWidget = node;

                // Send deselect and select messages
                if (current != JO_NULL)
                    current->wProc(current, WIDGET_DESELECTED);

                widget->wProc(widget, WIDGET_SELECTED);
                return true;
            }
        }
    }

    return false;
}
