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

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
// #include <imgui_impl_glfw.h>

#include "gui.h"
#include "data.h"



float gui_camera_r  = 10.0;
float gui_camera_rx = 30.0;
float gui_camera_ry = 30.0;

int gui_col_id = 0;
float gui_min = 0;
float gui_max = 600;

struct ImGuiContext* ctx;
struct ImGuiIO* io;

void gui_init(GLFWwindow* win) {

    // IMGUI_CHECKVERSION();
    ctx = igCreateContext(NULL);
    io  = igGetIO();
    //io.Fonts->AddFontDefault();
    ImWchar* range = ImFontAtlas_GetGlyphRangesCyrillic(io->Fonts);

    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "./Roboto-Regular.ttf", 18.0f, NULL, range);

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    igStyleColorsDark(NULL);
    
}

data_p data;
void gui_update() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    igBegin("Test", NULL, 0);
    igSliderScalar("min",ImGuiDataType_Float, &gui_min, &data->min[gui_col_id], &data->max[gui_col_id], NULL, 1.f);
    igSliderScalar("max",ImGuiDataType_Float, &gui_max, &data->min[gui_col_id], &data->max[gui_col_id], NULL, 1.f);
    int min = 0; int max = data->cols-1;
    bool gui_col_changed = igSliderScalar("column",    ImGuiDataType_U32, &gui_col_id, &min, &max,  "%u", 1.f);
    gui_col_changed = gui_col_changed || igListBoxStr_arr("column", &gui_col_id, data->header, data->cols, 5);
    if(gui_col_changed) {
        gui_min = data->min[gui_col_id];
        gui_max = data->max[gui_col_id];
    };
    igEnd();

    igShowDemoWindow(NULL);

}

void gui_render(){
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
};


void gui_terminate(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);
};
