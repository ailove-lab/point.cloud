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

#include "globals.h"
#include "interactive.h"
#include "bbgl.h"
#include "gui.h"
#include "data.h"
#include "scene.h"

float gui_camera_radius = 40.0;
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
float gui_max = 50.0;

int gui_focused = 0;

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

static w2s(vec4 r, mat4x4 mvp, vec4 p);
static s2w(vec4 r, mat4x4 mvp, vec4 p);

data_p data;

static char* format_float(const char* format, float f);
void clusters_window();
void columns_window();
void draw_axis(scene_t* scene);
void draw_markers(scene_t* scene);


void
gui_update(scene_t* scene) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    columns_window();
    clusters_window(); 
    draw_axis(scene);
    draw_markers(scene);    

    // igShowDemoWindow(NULL);
    
    gui_focused = igIsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}


void
clusters_window() {
    // clusters
    if(cluster_col > 0) {
        char buf[256];
        igBegin("Кластеры", NULL, 0);
        
        static int cid=0; //(int)data->min[data->cluster_col];
        // int min = (int) data->min[cluster_col];
        // int max = (int) data->max[cluster_col];
        if(igDragInt("Кластер", &cid, 0.25, 0, data->clusters_cnt-1, "%d")) {
            gui_col_id = cluster_col;
            gui_min = data->clusters[cid].id-0.1;
            gui_max = data->clusters[cid].id+0.1; 
        };
       
        sprintf(buf, 
            "id:%d\nРазмер кастера: %d\nДоход кластера: %s",
            data->clusters[cid].id,
            data->clusters[cid].cnt,
            format_float("%.2f руб",data->clusters[cid].sum)); 
        igText(buf);
        igSeparator();

        igColumns(3, "cols", false);
        ImVec2 win_size = igGetWindowSize(); 
        igSetColumnWidth(0,win_size.x*0.70);
        igSetColumnWidth(1,win_size.x*0.15);
        igSetColumnWidth(2,win_size.x*0.15);
        igText("Категория"  ); igNextColumn();
        igText("Доход [руб]"); igNextColumn();
        igText("Доля [%]"); igNextColumn();
        igSeparator();
        
        for(int i = 0; i<data->cols; i++) {
            float sum = data->clusters[cid].cat_sum[i].sum;
            int catid = data->clusters[cid].cat_sum[i].id;
            float sump = sum / data->clusters[cid].sum * 100.0;
            if(catid >= categories_start && sum > 0.0) {
                if (igSelectable(data->header[catid], false, ImGuiSelectableFlags_SpanAllColumns, (ImVec2){0.0,0.0})) {
                    gui_col_id = catid;
                    gui_min = data->min[gui_col_id] + (data->max[gui_col_id]-data->min[gui_col_id])*0.01;
                    gui_max = data->max[gui_col_id];
                }
                // bool hovered = igIsItemHovered();
                // igText(data->header[catid]); 
                igNextColumn();
                // align right
                // igSetCursorPosX(igGetCursorPosX()+igGetColumnWidth(1)-igCalcTextSize(buf, NULL, 0, 500).x-igGetStyle()->ItemSpacing.x);
                igText(format_float("%.2f",sum)); igNextColumn();
                sprintf(buf, "% 5.2f", sump);
                igText(buf); igNextColumn();
            }
        }
        igColumns(1, NULL, false);
        igEnd();
    }
}


void 
columns_window() {

    char buf[2048];

    float alpha_slider_min = 0.0;
    float alpha_slider_max = 1.0;
    float point_size_min   = 1.0;
    float point_size_max   = 9.0;

    igBegin("Колонки", NULL, 0);
    char* col_name = data->header[gui_col_id]; 
    sprintf(buf, "%s\nCnt:\t%5.0f\nSum:\t%5.2f\nMin:\t%5.2f\nMax:\t%5.2f", 
            col_name,
            data->notzero[gui_col_id], data->sum[gui_col_id],
            data->min[gui_col_id], data->max[gui_col_id]);

    igText(buf);
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
}


void
draw_axis(scene_t* scene) {
    ImDrawList* idl = igGetBackgroundDrawList();
    float s = 10.0;
    vec4 w0 = {   0.0,   0.0,   0.0, 1.0}; 
    vec4 wx = {     s,   0.0,   0.0, 1.0};
    vec4 wy = {   0.0,     s,   0.0, 1.0};
    vec4 wz = {   0.0,   0.0,     s, 1.0};
    
    vec4 p0, px, py, pz;
    w2s(p0, scene->mvp, w0);
    w2s(px, scene->mvp, wx);
    w2s(py, scene->mvp, wy);
    w2s(pz, scene->mvp, wz);

    ImDrawList_AddLine(idl, (ImVec2) {p0[0], p0[1]}, (ImVec2){px[0], px[1]}, 0x7FFF0000,1.0);
    ImDrawList_AddLine(idl, (ImVec2) {p0[0], p0[1]}, (ImVec2){py[0], py[1]}, 0x7F00FF00,1.0);
    ImDrawList_AddLine(idl, (ImVec2) {p0[0], p0[1]}, (ImVec2){pz[0], pz[1]}, 0x7F0000FF,1.0);

    // cursor screen, cursor world
    vec4 cs, cw,   // click screen/world  
         ms, mw,   // mouse screen/world
         rs, rw;   // rotation vector screen/world
         
    cs[0] = click_x;
    cs[1] = click_y;
    ms[0] = mouse_x;
    ms[1] = mouse_y;

    s2w(cw, scene->mvp, cs);
    w2s(cs, scene->mvp, cw);
    s2w(mw, scene->mvp, ms);
    w2s(ms, scene->mvp, mw);

    vec4_mul_cross(rw, mw, cw);
    vec4_scale(rw, rw, 10.0);
    w2s(rs, scene->mvp, rw);
    
    ImDrawList_AddLine(idl, 
        (ImVec2) {ms[0], ms[1]}, 
        (ImVec2) {cs[0], cs[1]}, 
        0x7F7F7F7F, 1.0);
    
    ImDrawList_AddLine(idl, 
        (ImVec2) {p0[0], p0[1]}, 
        (ImVec2) {cs[0], cs[1]}, 
        0x7F7F7F7F, 1.0);
    
    ImDrawList_AddLine(idl, 
        (ImVec2) {ms[0], ms[1]}, 
        (ImVec2) {p0[0], p0[1]}, 
        0x7F7F7F7F, 1.0);
    
    ImDrawList_AddLine(idl, 
        (ImVec2) {rs[0], rs[1]}, 
        (ImVec2) {p0[0], p0[1]}, 
        0x7F7F7F7F, 1.0);
    
}


// https://www.songho.ca/opengl/gl_transform.html#wincoord
// world to screen
static 
w2s(vec4 r, mat4x4 mvp, vec4 p) {
    mat4x4_mul_vec4(r, mvp, p); 
    r[0]=(r[0]/r[3]+1.0)*width /2.0; 
    r[1]=(1.0-r[1]/r[3])*height/2.0; 
    r[2]/=r[3];
}


// screen to vec
// https://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords
static
s2w(vec4 r, mat4x4 mvp, vec4 p) {
   p[0] = p[0]/width*2.0-1.0;
   p[1] = 1.0-p[1]/height*2.0;
   p[2] = 1.0;
   p[3] = 1.0;
   mat4x4 mvp_i;
   mat4x4_invert(mvp_i, mvp);
   mat4x4_mul_vec4(r, mvp_i, p);
   // r[0]/=r[3];
   // r[1]/=r[3];
   // r[2]*=r[3]; 
}

void
draw_markers(scene_t* scene) {
    char buf[2048]={0}; 
    /* 
    // sprintf(buf, "%04.2f %04.2f -> %04.2f %04.2f\n", p0[0], p0[1], px[0], px[1]);
    mat4x4_print(buf, "p", scene->p);
    mat4x4_print(buf, "v", scene->v);
    mat4x4_print(buf, "mvp", scene->mvp);
    ImDrawList_AddText(idl,(ImVec2){10,10},0xFFFFFFFF,buf,NULL);
    */
 
    ImDrawList* idl = igGetBackgroundDrawList();
    unsigned int row = data->max_id[gui_col_id];
    float* max_row = &data->data[row*data->cols];
    vec4 pos = {max_row[0], max_row[1], max_row[2], 1.0};
    vec4 prj; 
    w2s(prj, scene->mvp, pos);
    ImVec2 c = {prj[0], prj[1]};

    float r = 20.0;
    ImDrawList_AddCircle(idl, c, r, 0x7fFFFFFF,16,1.0);
    
    sprintf(buf, "Максимум: %5.2f", data->max[gui_col_id]);
    ImVec2 ts = igCalcTextSize(buf, NULL, 0, 500.0);
    ImDrawList_AddText(
        idl, (ImVec2) {c.x-ts.x/2.0, c.y+r+5.0}, 
        0xAFFFFFFF, buf, NULL);
    ts = igCalcTextSize(data->header[gui_col_id], NULL, 0, 500.0);
    ImDrawList_AddText(
        idl, (ImVec2) {c.x-ts.x/2.0, c.y-ts.y-r-5.0}, 
        0xAFFFFFFF, data->header[gui_col_id], NULL);
}


static char* 
format_float(const char* format, float f) {
    static char tmp[128];
    char src[128];
    sprintf(src, format, f);
    int l = strlen(src);
    char* s = &src[l];
    char* d = &tmp[127];

    while(1) {
        *d = *s; d--; s--;
        if(d == tmp-1 || s == src-1) return d++;
        if(d[1] == '.') break;
    }

    int i = 0;
    while(1) {
        *d = *s; d--; s--; i++;
        if(s == src-1) break;
        if(i%3==0) {*d=' '; d--;};
        if(d == tmp-1) return d++;
    }
    d++;
    return d;
}

void
gui_render(){
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    // ImGui_ImplOpenGL3_RenderDrawData(idd);
};


void
gui_terminate(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);
};
