#pragma once

#include <linmath.h>

typedef struct GLFWwindow GLFWwindow;
void interactive_init(GLFWwindow* win);

int int_dragging;
int int_dragging_started;
int int_dragging_stoped;

vec4 int_mouse;
vec4 int_click;
