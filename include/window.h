#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>

#include "shader.h"
#include "cube.h"
#include "matrices.h"
#include "texture.h"
#include "world.h"
#include "hash.h"
#include "noise1234.h"
#include "gl_util.h"
#include "gui.h"

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         vertex_array_id, vertex_buffer_id, color_buffer_id;
GLuint         triNormals_buffer_id;
GLuint         world_vertex_buffer_id;

#define SHADER_TUTORIAL5 1
#define SHADER_DIFFUSE 2
#define SHADER_WD 3
GLuint         shader_program_ids[4];
GLuint         matrix_ids[4];
GLuint         texture_ids[3];

GLuint         textures[2];
GLuint         uv_buffers[1];

float cam_pos[3];
float cam_rot_x;
float cam_rot_y;

float* view;
float model_to_world[16];
float world_to_camera[16];
float final_wtc[16];
float perspective_proj[16];
float final_matrix[16];
float final_matrix_2[16];

int Initialize(char* filename);
int Update();
int Cleanup();
int InitOpenGL(char* filename);
int InitShaderProgram(GLuint* program_id, char* name);
void InitMatrices();
void UpdateMatrices();
int InitTextures();

void world_init();
void world_fill_vert_buffer();
void world_draw();

int move_direction_z;
int move_direction_x;
void move();

int perlin_period;
float perlin_cutoff;

int toggle_full_draw;

float i_shift;

GLuint lightPos_id;

char* text_data;

void render_cursor(UI_Element** e);