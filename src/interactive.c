#include <GLFW/glfw3.h>
#include "interactive.h"

static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
static void cursor_position_callback( GLFWwindow* win, double xpos, double ypos);
static void mouse_button_callback( GLFWwindow* win, int button, int action, int mods);

void interactive_init(GLFWwindow* win) {
    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPosCallback(win, cursor_position_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
}

static void key_callback(
    GLFWwindow* win, 
    int key, 
    int scancode, 
    int action, 
    int mods) {

}

static void cursor_position_callback(
    GLFWwindow* win, 
    double xpos, 
    double ypos) {
        mouse_x = xpos;
        mouse_y = ypos;
}

static void mouse_button_callback(
    GLFWwindow* win, 
    int button, 
    int action, 
    int mods) {
}