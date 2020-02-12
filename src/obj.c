#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl3w.h>

#include "obj.h"
#include "csv.h"
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


obj_p 
obj_cloud() {
	
    obj_p obj = obj_ctor();

    csv_p csv = csv_load("_clusters.4d.csv");
    obj->cols = csv->cols;
    obj->rows = csv->rows;

    glGenBuffers(1, &obj->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, obj->rows * obj->cols * sizeof(float), csv->data, GL_STATIC_DRAW);       
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    csv_free(csv);

    glGenVertexArrays(1, &obj->vao);
    init_vao(obj, 4);

    /*
    // example   
    // interleaved data
    GLfloat vertices[] = {
        -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
    };
    GLuint VBOId;
    glGenVertexArrays(1, &VAOId);
    glBindVertexArray(VAOId);
    glGenBuffers(1, &VBOId);
    glBindBuffer(GL_ARRAY_BUFFER, VBOId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // specify position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // specify color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    // specify texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    */
    
    return obj;
}


void 
obj_render(obj_p obj) {
    static int i=0;
    static int j=0;
    if(++i % 50 == 0) init_vao(obj, ++j%5);
    
    glBindVertexArray(obj->vao);
    glDrawArrays(GL_POINTS, 0, obj->rows);
    glBindVertexArray(0);
}
