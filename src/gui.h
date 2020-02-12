#pragma once

float gui_camera_r ;
float gui_camera_rx;
float gui_camera_ry;
int gui_cat_id;

typedef struct GLFWwindow GLFWwindow;
void gui_init(GLFWwindow* win);
void gui_update();
void gui_render();
void gui_terminate();

