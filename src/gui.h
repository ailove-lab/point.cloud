#pragma once

float gui_camera_r ;
float gui_camera_rx;
float gui_camera_ry;
int gui_focused;
int gui_col_id;
float gui_min;
float gui_max;
float gui_alpha_1;
float gui_alpha_2;

typedef struct GLFWwindow GLFWwindow;
void gui_init(GLFWwindow* win);
void gui_update();
void gui_render();
void gui_terminate();

