/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include "window.h"
#include "time.h"

/*
 * Program entry point
 */

int main(int argc, char *argv[])
{
    int should_run;

    printf("Initializing...\n");
    if (Initialize(argv[1])) {
        return 1;
    }

    static const int FPS = 60;

    clock_t timer_start = clock();
    clock_t timer_end = clock();
    double timer_diff = ((double) (timer_end - timer_start)) / CLOCKS_PER_SEC;

    static const float FRAME_TIME = 1 / FPS;
    //SDL_SetRelativeMouseMode(SDL_TRUE);

    printf("Running...\n");
    int kbshift = 0;
    int kbctrl = 0;
    for (should_run = 1; should_run; ) {

        if (timer_diff > FRAME_TIME){
            timer_start = clock();

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    should_run = 0;
                    break;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                        add_character(&test_element, '\n');
                        break;

                        case SDLK_BACKSPACE:
                        backspace(&test_element);
                        break;

                        case SDLK_LEFT:
                        move_cursor(&test_element, -1);
                        break;

                        case SDLK_RIGHT:
                        move_cursor(&test_element, 1);
                        break;

                        case SDLK_UP:
                        if (move_line(&test_element, -1))
                            move_cursor(&test_element, -2/(test_element->font_size)-1);
                        break;

                        case SDLK_DOWN:
                        if (move_line(&test_element, 1))
                            move_cursor(&test_element, 2/(test_element->font_size)+1);
                        break;

                        default:
                        if (((event.key.keysym.mod | KMOD_LCTRL) == event.key.keysym.mod) && event.key.keysym.sym == SDLK_s)
                        {
                            FILE* ofptr = fopen(argv[1], "wb");
                            fputs(test_element->text, ofptr);
                            fclose(ofptr);
                        }
                    }
                }
                if (event.type == SDL_MOUSEWHEEL)
                {
                    const Uint8 *state = SDL_GetKeyboardState(NULL);
                    if (state[SDL_SCANCODE_LSHIFT])
                        element_horizontal_shift(&test_element, event.wheel.y);
                    else
                        element_vertical_shift(&test_element, -event.wheel.y);
                }
                if (event.type == SDL_TEXTINPUT)
                {
                    add_character(&test_element, event.text.text[0]);
                }
            }

            Update();
        }

        timer_end = clock();
        timer_diff = ((double) (timer_end - timer_start)) / CLOCKS_PER_SEC;
    }

    printf("Exiting...\n");
    Cleanup();
    return 0;
}