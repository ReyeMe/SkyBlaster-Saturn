#include <jo/jo.h>
#include "analogPad.h"

// -------------------------------------
// Public
// -------------------------------------

bool AnalogPadIsAvailable(const unsigned int port)
{
    return jo_inputs[port].id == ANALOGPAD_TYPE;
}

void AnalogPadGetData(const unsigned int port, AnalogPad *data)
{
    PerAnalog *analogData = (PerAnalog *)(&jo_inputs[port]);

    if (AnalogPadIsAvailable(port))
    {
        data->AxisX = analogData->x - 0x80;
        data->AxisY = analogData->y - 0x80;
        data->TriggerR = analogData->z;
        data->TriggerL = analogData->dummy1;
        data->IsAvailable = true;
    }
    else
    {
        data->AxisX = 0;
        data->AxisY = 0;
        data->TriggerR = 0;
        data->TriggerL = 0;
        data->IsAvailable = false;
    }
}
