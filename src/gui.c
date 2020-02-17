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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_GLFW_GL3_MOUSE_GRABBING

#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#include "gui.h"
#include "data.h"

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

float gui_camera_r  = 10.0;
float gui_camera_rx = 30.0;
float gui_camera_ry = 30.0;

int gui_col_id = 0;
float gui_min = 0;
float gui_max = 600;

struct nk_context *ctx;
struct nk_colorf bg;

void gui_init(GLFWwindow* win) {

    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    struct nk_font_config config = nk_font_config(16);
    config.oversample_h = 2;
    config.oversample_v = 2;
    config.range = nk_font_cyrillic_glyph_ranges();

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font* font = nk_font_atlas_add_from_file(atlas, "./Roboto-Regular.ttf", 16, &config);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &font->handle);
    
}

data_p data;
void gui_update() {

    nk_glfw3_new_frame();
    
    /* GUI */
    if (nk_begin(ctx, "Ailove.Lab", nk_rect(50, 50, 250, 600),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_float(ctx, "Radius",     0.01, &gui_camera_r , 100.0, 0.1, 0.2);
        nk_property_float(ctx, "Rotation x", 0.01, &gui_camera_rx, 180.0, 0.1, 0.2);
        nk_property_float(ctx, "Rotation y", 0.00, &gui_camera_ry, 360.0, 0.1, 0.2);
        
        nk_property_float(ctx, "min", data->min[gui_col_id], &gui_min, data->max[gui_col_id], 0.1, 0.2);
        nk_property_float(ctx, "max", data->min[gui_col_id], &gui_max, data->max[gui_col_id], 0.1, 0.2);
        
        nk_layout_row_dynamic(ctx, 300, 1);
        if (nk_group_begin(ctx, "Group_Without_Border", 0)) {
            nk_layout_row_static(ctx, 18, 300, 1);
            for (int i = 0; i<data->cols; ++i) {
                int current = gui_col_id == i;
                if(nk_selectable_label(ctx, data->header[i], NK_TEXT_LEFT, &current)) {
                    gui_col_id = i;
                    gui_min = data->min[gui_col_id];
                    gui_max = data->max[gui_col_id];
                };
            }
            nk_group_end(ctx);
        }

    }
    nk_end(ctx);
}

void gui_render(){

    /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI. */
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
};


void gui_terminate(){
    nk_glfw3_shutdown();
};
