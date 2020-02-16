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
#include <linmath.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
// #include <imgui_impl_glfw.h>

#include "gui.h"
#include "data.h"
#include "scene.h"

float gui_camera_zoom = 3.0;
float gui_camera_rx = 30.0;
float gui_camera_ry = 30.0;

float gui_off_u = 10.0;
float gui_rot_u = 0.0;
float gui_rot_v = 0.0;

float gui_point_size = 1.0;
float gui_alpha_1    = 0.10;
float gui_alpha_2    = 0.75;

int gui_col_id = 0;
float gui_min = 0.0;
float gui_max = 600.0;

struct ImGuiContext* ctx;
struct ImGuiIO* io;
ImDrawData idd;

void gui_init(GLFWwindow* win) {

    // IMGUI_CHECKVERSION();
    ctx = igCreateContext(NULL);
    io  = igGetIO();
    //io.Fonts->AddFontDefault();
    ImWchar* range = ImFontAtlas_GetGlyphRangesCyrillic(io->Fonts);

    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "./font.ttf", 16.f, NULL, range);

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    igStyleColorsDark(NULL);
    
}

data_p data;
void gui_update(scene_t* scene) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    float alpha_slider_min = 0.0;
    float alpha_slider_max = 1.0;
    float point_size_min = 1.0;
    float point_size_max = 9.0;

    igBegin("Column", NULL, 0);
    char* col_name = data->header[gui_col_id]; 
    igText(data->header[gui_col_id]);
    igSliderScalar("min",ImGuiDataType_Float, &gui_min, &data->min[gui_col_id], &data->max[gui_col_id], NULL, 1.f);
    if(gui_max<gui_min) gui_max = gui_min;
    igSliderScalar("max",ImGuiDataType_Float, &gui_max, &data->min[gui_col_id], &data->max[gui_col_id], NULL, 1.f);
    if(gui_min>gui_max) gui_min = gui_max;
    igSliderScalar("point size",ImGuiDataType_Float, &gui_point_size, &point_size_min, &point_size_max, NULL, 1.f);
    igSliderScalar("alpha 1",ImGuiDataType_Float, &gui_alpha_1, &alpha_slider_min, &alpha_slider_max, NULL, 1.f);
    igSliderScalar("alpha 2",ImGuiDataType_Float, &gui_alpha_2, &alpha_slider_min, &alpha_slider_max, NULL, 1.f);
    
    int min = 0; int max = data->cols-1;
    bool gui_col_changed = igSliderScalar("col #", ImGuiDataType_U32, &gui_col_id, &min, &max,  "%u", 1.f);
    gui_col_changed = gui_col_changed || igListBoxStr_arr("col", &gui_col_id, data->header, data->cols, 10);
    if(gui_col_changed) {
        gui_min = data->min[gui_col_id] + (data->max[gui_col_id]-data->min[gui_col_id])*0.01;
        gui_max = data->max[gui_col_id];
    };
    igEnd();
    if(0) {
    ImDrawList* idl = igGetBackgroundDrawList();
    ImVec2 pos  = igGetCursorScreenPos();
    ImVec2 size = igGetContentRegionAvail();
    ImVec2 corner = (ImVec2){pos.x+size.x, pos.y+size.y};
    for(int col=5; col< data->cols; col++) {
        unsigned int row = data->max_id[col];
        float* max_row = &data->data[row*data->cols];
        vec4 pos = {max_row[0], max_row[1], max_row[2], 1.0};
        vec4 prj = {0};
        mat4x4_mul_vec4(prj, scene->mvp, pos);
        // mat4x4_mul_vec4(prj, scene->rot, prj);
        
        ImDrawList_AddText(
            idl, 
            (ImVec2){prj[0]*100, -prj[1]*100}, 
            0xAFFFFFFF, 
            data->header[col], 
            NULL);
    }    
    }
    igShowDemoWindow(NULL);
    
    gui_focused = igIsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void gui_render(){
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    // ImGui_ImplOpenGL3_RenderDrawData(idd);
};


void gui_terminate(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);
};
