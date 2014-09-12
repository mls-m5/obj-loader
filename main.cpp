
#include <stdio.h>
#include <stdlib.h>
/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <GL/gl.h>

#include <SDL2/SDL.h>

#include "draw.h"
#include "modelobject.h"

/* A simple function that prints a message, the error code returned by SDL,
 * and quits the application */
void sdldie(const char *msg)
{
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}


void checkSDLError(int line = -1)
{
#ifndef NDEBUG
        const char *error = SDL_GetError();
        if (*error != '\0')
        {
                printf("SDL Error: %s\n", error);
                if (line != -1)
                        printf(" + line: %i\n", line);
                SDL_ClearError();
        }
#endif
}


/* Our program's entry point */
int main(int argc, char *argv[])
{
    SDL_Window *mainwindow; /* Our window handle */
    SDL_GLContext maincontext; /* Our opengl context handle */

    if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
        sdldie("Unable to initialize SDL"); /* Or die on error */

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mainwindow = SDL_CreateWindow("model-loader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512 * 2, 512 * 2, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mainwindow) /* Die if creation failed */
        sdldie("Unable to create window");

    checkSDLError(__LINE__);

    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
    checkSDLError(__LINE__);


    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    initDrawModule(512 * 2, 512 * 2);
	ModelObject modelObject;
	modelObject.init();
//	modelObject.load("ship2-smooth.obj", true);
//	modelObject.load("untitled.obj", false);
	modelObject.load("ship3.obj", true);
//	modelObject.load("ship.obj", true);

	bool running = true;

	while (running){

		/* Clear our buffer with a red background */
		glClearColor ( .2, 0.0, 0.0, 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-.1,.1, -.1, .1, 1.f, 50.f);
		glTranslated(0,0,-15);
		glMatrixMode(GL_MODELVIEW);
		modelObject.render();

		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(mainwindow);
//		SDL_Delay(50);

		SDL_Event event;
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT){
				running = 0;
			}
		}
	}

	QuitDrawModule();

    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}
