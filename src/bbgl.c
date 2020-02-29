#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "bbgl.h"
#include "gui.h"
#include "scene.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

static void error_callback(int e, const char *d)
{printf("Error %d: %s\n", e, d);}

/* Platform */
static GLFWwindow *win;
int screen_width = 0, screen_height = 0;
scene_p scene;

void bbgl_init() {

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(1);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cluster cloud", NULL, NULL);
    if(!win) exit(1);

    glfwMakeContextCurrent(win);
    gl3wInit();
    glfwGetWindowSize(win, &screen_width, &screen_height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // glewExperimental = 1;
    // if (glewInit() != GLEW_OK) {
    //     fprintf(stderr, "Failed to setup GLEW\n");
    //     exit(1);
    // }
    
    scene = scene_ctor();
    
    gui_init(win);
    interactive_init(win);
}

void bbgl_loop() {
    
    while (!glfwWindowShouldClose(win)) {
        /* Input */
        glfwPollEvents();
        interactive_update();        
        gui_update(scene);

        /* Draw */
        glfwGetWindowSize(win, &screen_width, &screen_height);
        ratio = (float)screen_width/(float)screen_height;
        glViewport(0, 0, screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        
        //SCENE
        glEnable(GL_BLEND);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        scene_render(scene);
        glDisable(GL_BLEND);
        
        // GUI
        gui_render();
        
        glfwSwapBuffers(win);
    }

    // cleanup
    scene_dtor(scene);
    gui_terminate();
    glfwTerminate();
}

