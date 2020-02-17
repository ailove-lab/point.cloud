#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl3w.h>

#include "obj.h"
#include "data.h"
#include "gui.h"

obj_p
obj_ctor() {
    obj_p obj = calloc(1, sizeof(obj_t));
    mat4x4_identity(obj->m);
	return obj;
}

void
obj_dtor(obj_p obj) {
    glDeleteBuffers(1, &obj->vbo);
    glDeleteVertexArrays(1, &obj->vao);
	free(obj);
}


static void 
init_vao(obj_p obj, int col) {
    // bind vao & vbo
    glBindVertexArray(obj->vao);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    // map coordinates
    glEnableVertexAttribArray(0);        
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, obj->cols * sizeof(float), (GLvoid*) (sizeof(float)*0));
    // map data col
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, obj->cols * sizeof(float), (GLvoid*) (sizeof(float)*col));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

data_p data;

obj_p 
obj_cloud() {
	
    obj_p obj = obj_ctor();

    obj->cols = data->cols;
    obj->rows = data->rows;

    glGenBuffers(1, &obj->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, obj->rows * obj->cols * sizeof(float), data->data, GL_STATIC_DRAW);       
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &obj->vao);
    // init_vao(obj, gui_col_id);
        
    return obj;
}


void 
obj_render(obj_p obj) {
    static int col_id = -1;
    if(gui_col_id!=col_id) {
        col_id = gui_col_id;
        init_vao(obj, col_id);
    }
    
    glBindVertexArray(obj->vao);
    glDrawArrays(GL_POINTS, 0, obj->rows);
    glBindVertexArray(0);
}
