#include <gui.h>

void font_gen_UVs(char* input, int size, float** output)
{
    static int i;
    static int twi;
    *output = malloc(size*12*sizeof(float));
    static float x;
    static float y;
    const float od16 = 1.0f/16.0f;
    for (i = 0; i < size; i++)
    {
        twi = i*12;
        x = (input[i] % 16) / 16.0;
        y = (input[i] / 16) / 16.0;

        (*output)[twi++] = x + od16;
        (*output)[twi++] = y;
        (*output)[twi++] = x;
        (*output)[twi++] = y;
        (*output)[twi++] = x;
        (*output)[twi++] = y + od16;

        (*output)[twi++] = x;
        (*output)[twi++] = y + od16;
        (*output)[twi++] = x + od16;
        (*output)[twi++] = y + od16;
        (*output)[twi++] = x + od16;
        (*output)[twi++] = y;
    }
}

void element_init(float screen_pos_x, float screen_pos_y, float font_size, char* filename, int cursor_index, UI_Element** e)
{
    FILE* fptr = fopen(filename, "rb");
    fseek(fptr, 0, SEEK_END);
    long length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    char* text;
    if (length > 0)
    {
        text = malloc (length+1);
        fread(text, 1, length, fptr);
        text[length] = '\0';
    }
    else
    {
        text = malloc(sizeof(char));
        text[0] = '\0';
        length = 0;
    }
    fclose (fptr);

    int text_length = length+1;
    (*e) = malloc(sizeof(UI_Element));
    font_gen_UVs(text, text_length, &((*e)->UVs));
    (*e)->verts = malloc(text_length * 18 * sizeof(float));
    (*e)->verts_size = text_length * 18;
    (*e)->UVs_size = text_length * 12;
    (*e)->tri_count = text_length * 2;

    (*e)->screen_pos_x = screen_pos_x;
    (*e)->screen_pos_y = screen_pos_y;
    (*e)->font_size = font_size;
    (*e)->text = malloc(text_length);
    strcpy((*e)->text, text);
    (*e)->text_length = text_length;
    (*e)->cursor_index = cursor_index;
    (*e)->cursor_render_frame_count = 0;

    (*e)->draw_matrix = malloc(16*sizeof(float));
    memcpy((*e)->draw_matrix, identity44, sizeof(identity44));

    float og_screen_pos_x = screen_pos_x;
    int step;
    for (int i = 0; i < text_length; i++)
    {
        step = i * 18;
        //tri1
        (*e)->verts[step++]   = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        //tri2
        (*e)->verts[step++]  = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        screen_pos_x += font_size;
        if (text[i] == '\n')
        {
            screen_pos_x = og_screen_pos_x;
            screen_pos_y -= font_size;
        }
    }
}

void element_refill(UI_Element** e, char* text)
{
    int text_length = strlen(text)+1;
    free((*e)->UVs);
    font_gen_UVs(text, text_length, &((*e)->UVs));
    free((*e)->verts);
    (*e)->verts = malloc(text_length * 18 * sizeof(float));
    (*e)->verts_size = text_length * 18;
    (*e)->UVs_size = text_length * 12;
    (*e)->tri_count = text_length * 2;

    free((*e)->text);
    (*e)->text = malloc(text_length);
    strcpy((*e)->text, text);
    (*e)->text_length = text_length;

    float og_screen_pos_x = (*e)->screen_pos_x;
    float screen_pos_x = (*e)->screen_pos_x;
    float screen_pos_y = (*e)->screen_pos_y;
    float font_size = (*e)->font_size;
    int step;
    for (int i = 0; i < text_length; i++)
    {
        step = i * 18;
        //tri1
        (*e)->verts[step++] = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        //tri2
        (*e)->verts[step++] = screen_pos_x;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y - font_size;
        (*e)->verts[step++] = 0;

        (*e)->verts[step++] = screen_pos_x + font_size;
        (*e)->verts[step++] = screen_pos_y;
        (*e)->verts[step++] = 0;

        screen_pos_x += font_size;
        if (text[i] == '\n')
        {
            screen_pos_x = og_screen_pos_x;
            screen_pos_y -= font_size;
        }
    }
}

void gui_init()
{
    
}

void update_gui_buffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->UVs_size*sizeof(float), test_element->UVs, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_vert_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->verts_size*sizeof(float), test_element->verts, GL_STATIC_DRAW);
}

void element_free(UI_Element** e)
{
    free((*e)->verts);
    free((*e)->UVs);
    free((*e)->text);
    free(*e);
}

void add_character(UI_Element** e, char c)
{
    int newlen = (*e)->text_length+1;
    char* newtext = malloc(newlen);
    memcpy(newtext, (*e)->text, (*e)->cursor_index+1);
    newtext[(*e)->cursor_index] = c;
    memcpy(newtext+(*e)->cursor_index+1, (*e)->text+(*e)->cursor_index, (*e)->text_length - (*e)->cursor_index-1); 
    newtext[newlen-1] = '\0';

    element_refill(e, newtext);
    free(newtext);
    update_gui_buffers();

    move_cursor(e, 1);
}

void backspace(UI_Element** e)
{
    int newlen = (*e)->text_length - 1;
    char* newtext = malloc(newlen);
    memcpy(newtext, (*e)->text, (*e)->cursor_index-1);
    memcpy(newtext + (*e)->cursor_index - 1, (*e)->text + (*e)->cursor_index, (*e)->text_length - (*e)->cursor_index);

    element_refill(e, newtext);
    free(newtext);
    update_gui_buffers();

    move_cursor(e, -1);
}

void move_cursor(UI_Element** e, int direction)
{
    (*e)->cursor_index += direction;
}

void element_vertical_shift(UI_Element** e, int direction)
{
    (*e)->draw_matrix[7] += (direction * ((*e)->font_size));
}

void element_horizontal_shift(UI_Element** e, int direction)
{
    (*e)->draw_matrix[3] += (direction * ((*e)->font_size));
}

int move_line(UI_Element** e, int direction)
{
    for (int i = (*e)->cursor_index + direction; (0 <= i) && (i < (*e)->text_length); i+=direction)
    {
        if ((*e)->text[i] == '\n')
        {
            (*e)->cursor_index = i;
            return 0;
        }
    }
    return 1;
}