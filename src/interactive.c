#include <GLFW/glfw3.h>
#include "interactive.h"
#include "gui.h"

static void key_callback            (GLFWwindow* win, int key, int scancode, int action, int mods);
static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos);
static void mouse_button_callback   (GLFWwindow* win, int button, int action, int mods);
static void mouse_scroll_callback   (GLFWwindow* win, double xoffset, double yoffset);

static char draging = 0;

void 
interactive_init(GLFWwindow* win) {
    glfwSetKeyCallback        (win, key_callback);
    glfwSetCursorPosCallback  (win, cursor_position_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    glfwSetScrollCallback     (win, mouse_scroll_callback);
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
    
    static double old_xpos;
    static double old_ypos; 
    mouse_x = xpos;
    mouse_y = ypos;
    if(!gui_focused) { 
        if(draging) {
            gui_camera_rx -= (old_ypos-ypos)*0.1;
            gui_camera_ry -= (old_xpos-xpos)*0.1;
            // printf("ms %d %d\n", xpos, ypos);
        } 

        // else if(glfwGetMouseButton(win, 1) == GLFW_PRESS) {
        //     gui_rot_u -= (old_xpos-xpos)*0.01;
        //     // gui_rot_v -= (old_ypos-ypos)*0.01;
        //     gui_off_u -= (old_ypos-ypos)*0.1;        
        // }
    }
    old_xpos = xpos;
    old_ypos = ypos;
}


static void 
mouse_button_callback(
    GLFWwindow* win, 
    int button, 
    int action, 
    int mods) {
        
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS && !gui_focused) {
            printf("Press\n");
            draging = 1;
            click_x = mouse_x;
            click_y = mouse_y;
        } else if (action == GLFW_RELEASE && draging) {
            printf("Release\n"); 
            draging = 0;
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
