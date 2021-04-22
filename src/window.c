#include "window.h"

int Initialize(char* filename)
{
    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL video\n");
        return 1;
    }

    // Create main window
    m_window = SDL_CreateWindow(
        "SwagText",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000, 1000,
        SDL_WINDOW_OPENGL);
    if (m_window == NULL) {
        fprintf(stderr, "Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        fprintf(stderr, "Failed to create GL context\n");
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize GL Extension Wrangler (GLEW)
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    InitOpenGL(filename);

    InitTextures();

    return 0;
}

int InitOpenGL(char* filename)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    InitShaderProgram(&shader_program_ids[0], "tutorial4");
    matrix_ids[0] = glGetUniformLocation(shader_program_ids[0], "MVP");

    InitShaderProgram(&shader_program_ids[1], "tutorial5");
    matrix_ids[1] = glGetUniformLocation(shader_program_ids[1], "MVP");
    
    element_init(-1.0f, 1.0f, 0.025f, filename, 0, &test_element);
    glGenBuffers(1, &gui_test_uv_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->UVs_size*sizeof(float), test_element->UVs, GL_STATIC_DRAW);
    
    glGenBuffers(1, &gui_test_vert_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_vert_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->verts_size*sizeof(float), test_element->verts, GL_STATIC_DRAW);

    glGenBuffers(1, &gui_test_cursor_vert_buffer_id);
    glGenBuffers(1, &gui_test_cursor_color_buffer_id);
}

int InitTextures()
{
    textures[1] = loadDDS("../res/tex/results/minefont.dds");
	
	texture_ids[0] = glGetUniformLocation(shader_program_ids[1], "myTextureSampler");
}

void gui_draw()
{
    glUseProgram(shader_program_ids[SHADER_TUTORIAL5]);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(texture_ids[0], 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_vert_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_uv_buffer_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

    glUniformMatrix4fv(glGetUniformLocation(shader_program_ids[SHADER_TUTORIAL5], "MVP"), 1, GL_TRUE, test_element->draw_matrix);

    glDrawArrays(GL_TRIANGLES, 0, (test_element->tri_count-2) * 3);

}

//per frame
int Update()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //JOJ region:

    gui_draw();
    render_cursor(&test_element);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    SDL_GL_SwapWindow(m_window);
    return 0;
}

float cursor_color_data[18] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

void render_cursor(UI_Element** e)
{
    (*e)->cursor_render_frame_count++;
    if ((*e)->cursor_render_frame_count > 30)
    {
        glUseProgram(shader_program_ids[0]);

        glBindBuffer(GL_ARRAY_BUFFER, gui_test_cursor_color_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), cursor_color_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, gui_test_cursor_vert_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), (*e)->verts + (18 * ((*e)->cursor_index)), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, gui_test_cursor_vert_buffer_id);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, gui_test_cursor_color_buffer_id);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

        glUniformMatrix4fv(glGetUniformLocation(shader_program_ids[0], "MVP"), 1, GL_TRUE, test_element->draw_matrix);

        glDrawArrays(GL_TRIANGLES, 0, 2*3);
        
    }
    if ((*e)->cursor_render_frame_count > 60)
    {
        (*e)->cursor_render_frame_count = 0;
    }
}

int Cleanup()
{
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteBuffers(1, &color_buffer_id);
    glDeleteProgram(shader_program_ids[0]);
    glDeleteVertexArrays(1, &vertex_array_id);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return 0;
}