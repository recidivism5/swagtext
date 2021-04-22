#include <GL/glew.h>

#include "shader.h"
#include "matrices.h"

typedef struct UI_Element {
    float* verts;
    float* UVs;
    int verts_size;
    int UVs_size;
    int tri_count;
    float screen_pos_x;
    float screen_pos_y;
    float font_size;
    char* text;
    size_t text_length;
    int cursor_index;
    int cursor_render_frame_count;
    float* draw_matrix;
} UI_Element;

typedef struct Paper {
    float* page_verts;
    float* text_verts;
    float* text_UVs;
} Paper;

UI_Element* test_element;
GLuint gui_test_vert_buffer_id;
GLuint gui_test_uv_buffer_id;

GLuint gui_test_cursor_vert_buffer_id;
GLuint gui_test_cursor_color_buffer_id;

GLuint font_shader_program_id;
GLuint font_vao_id;

void element_init(float screen_pos_x, float screen_pos_y, float font_size, char* filename, int cursor_index, UI_Element** e);
void font_gen_UVs(char* input, int size, float** output);

void move_cursor(UI_Element** e, int direction);
int move_line(UI_Element** e, int direction);
void add_character(UI_Element** e, char c);
void backspace(UI_Element** e);
void element_vertical_shift(UI_Element** e, int direction);
void element_horizontal_shift(UI_Element** e, int direction);
void update_gui_buffers();