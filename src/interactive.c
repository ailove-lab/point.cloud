#include <GLFW/glfw3.h>
#include "interactive.h"
#include "gui.h"

int int_dragging = 0;
int int_dragging_started = 0;
int int_dragging_stoped = 0;

static void key_callback            (GLFWwindow* win, int key, int scancode, int action, int mods);
static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos);
static void mouse_button_callback   (GLFWwindow* win, int button, int action, int mods);
static void mouse_scroll_callback   (GLFWwindow* win, double xoffset, double yoffset);


void 
interactive_init(GLFWwindow* win) {
    glfwSetKeyCallback        (win, key_callback);
    glfwSetCursorPosCallback  (win, cursor_position_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    glfwSetScrollCallback     (win, mouse_scroll_callback);
}

void interactive_update() {
    static int old_dragging = 0;
    int_dragging_started =  int_dragging && int_dragging!=old_dragging;
    int_dragging_stoped  = !int_dragging && int_dragging!=old_dragging; 
    old_dragging = int_dragging;
}

static void 
key_callback(
    GLFWwindow* win, 
    int key, 
    int scancode, 
    int action, 
    int mods) {
    // printf("key %d\n", key);
}


static void 
cursor_position_callback(
    GLFWwindow* win, 
    double xpos, 
    double ypos) {
    
    static vec4 old_pos;

    vec4_set(int_mouse, xpos, ypos, 0.0, 1.0);    
    vec4_dup(old_pos, int_mouse);
}


static void 
mouse_button_callback(
    GLFWwindow* win, 
    int button, 
    int action, 
    int mods) {
        
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS && !gui_focused) {
            int_dragging = 1;
            vec4_dup(int_click, int_mouse);
        }
        if (action == GLFW_RELEASE && int_dragging) {
            int_dragging = 0;
        }
    }
}


static void 
mouse_scroll_callback(
    GLFWwindow* win,
    double xoffset,
    double yoffset) {
    if(!gui_focused) {
        gui_camera_radius -= yoffset;
    }
}
