#pragma once

float gui_camera_zoom;
float gui_camera_rx;
float gui_camera_ry;
float gui_off_u;
float gui_rot_u;
float gui_rot_v;
int gui_focused;
int gui_col_id;
float gui_min;
float gui_max;
float gui_alpha_1;
float gui_alpha_2;
float gui_point_size;

typedef struct GLFWwindow GLFWwindow;
typedef struct scene_t scene_t;
void gui_init(GLFWwindow* win);
void gui_update(scene_t* scene);
void gui_render();
void gui_terminate();

