#include <jo/jo.h>
#include "model.h"
#include "font3D.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Contains meshes for all numeric character */
static jo_3d_mesh *numerics;

/* Contains meshes for all letter charactes */
static jo_3d_mesh *letters;

// -------------------------------------
// Public
// -------------------------------------

void FontInitialize()
{
    int index = 0;
    Uint32 face = 0;
    int loaded = 0;
    numerics = ML_LoadMesh("FNTNUM.TMF", "MODELS", &loaded);

    for (index = 0; index < loaded; index++)
    {
        for (face = 0; face < numerics[index].data.nbPolygon; face++)
        {
            numerics[index].data.attbl[face].sort = SORT_MAX;
        }
    }

    loaded = 0;
    letters = ML_LoadMesh("FNTLET.TMF", "MODELS", &loaded);

    for (index = 0; index < loaded; index++)
    {
        for (face = 0; face < letters[index].data.nbPolygon; face++)
        {
            letters[index].data.attbl[face].sort = SORT_MAX;
        }
    }
}

void FontPrintCentered(const char *text, void (*modifier)(const int column, const int row, FontModifier *data), void *customData)
{
    int letterLineLoc = 0;
    int line = 0;
    jo_3d_mesh *character = JO_NULL;
    int index = 0;
    int length = 0;
    char current = text[index++];

    while (current != '\0')
    {
        length++;
        current = text[index++];
    }

    index = 0;
    current = text[index++];
    jo_fixed offsetText = (length * FONT_HEIGHT) >> 1;

    while (current != '\0')
    {
        character = JO_NULL;

        if (current >= FONT_NUM_START && current <= FONT_NUM_END)
        {
            character = &numerics[current - FONT_NUM_START];
        }
        else if (current >= FONT_LET_UP_START && current <= FONT_LET_UP_END)
        {
            character = &letters[current - FONT_LET_UP_START];
        }
        else if (current >= FONT_LET_START && current <= FONT_LET_END)
        {
            // Only upper case
            character = &letters[current - FONT_LET_START];
        }

        if (character != JO_NULL)
        {
            FontModifier data = {{letterLineLoc * FONT_HEIGHT, line * FONT_WIDTH, 0}, {0, 0, 0}, customData};

            if (modifier != JO_NULL)
            {
                modifier(letterLineLoc, line, &data);
            }

            jo_3d_push_matrix();
            {
                jo_3d_translate_matrix_fixed(-data.Pos.y, -data.Pos.x + offsetText, data.Pos.z);

                if (data.Rotation.ry != 0)
                    jo_3d_rotate_matrix_x(data.Rotation.ry);
                if (data.Rotation.rx != 0)
                    jo_3d_rotate_matrix_y(data.Rotation.rx);
                if (data.Rotation.rz != 0)
                    jo_3d_rotate_matrix_z(data.Rotation.rz);

                jo_3d_mesh_draw(character);
            }
            jo_3d_pop_matrix();
        }

        letterLineLoc++;

        if (current == '\n')
        {
            letterLineLoc = 0;
            line++;
        }

        current = text[index++];
    }
}

void FontPrint(const char *text, void (*modifier)(const int column, const int row, FontModifier *data), void *customData)
{
    int letterLineLoc = 0;
    int line = 0;
    jo_3d_mesh *character = JO_NULL;
    int index = 0;
    char current = text[index++];

    while (current != '\0')
    {
        character = JO_NULL;

        if (current >= FONT_NUM_START && current <= FONT_NUM_END)
        {
            character = &numerics[current - FONT_NUM_START];
        }
        else if (current >= FONT_LET_UP_START && current <= FONT_LET_UP_END)
        {
            character = &letters[current - FONT_LET_UP_START];
        }
        else if (current >= FONT_LET_START && current <= FONT_LET_END)
        {
            // Only upper case
            character = &letters[current - FONT_LET_START];
        }

        if (character != JO_NULL)
        {
            FontModifier data = {{letterLineLoc * FONT_HEIGHT, line * FONT_WIDTH, 0}, {0, 0, 0}, customData};

            if (modifier != JO_NULL)
            {
                modifier(letterLineLoc, line, &data);
            }

            jo_3d_push_matrix();
            {
                jo_3d_translate_matrix_fixed(-data.Pos.y, -data.Pos.x, data.Pos.z);

                if (data.Rotation.ry != 0)
                    jo_3d_rotate_matrix_x(data.Rotation.ry);
                if (data.Rotation.rx != 0)
                    jo_3d_rotate_matrix_y(data.Rotation.rx);
                if (data.Rotation.rz != 0)
                    jo_3d_rotate_matrix_z(data.Rotation.rz);

                jo_3d_mesh_draw(character);
            }
            jo_3d_pop_matrix();
        }

        letterLineLoc++;

        if (current == '\n')
        {
            letterLineLoc = 0;
            line++;
        }

        current = text[index++];
    }
}
