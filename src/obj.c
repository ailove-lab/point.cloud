#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl3w.h>

#include "obj.h"
#include "csv.h"

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

obj_p 
obj_plane() {
	
    obj_p obj = obj_ctor();

    // Loading data to mem
    const int point_size = 4;
    csv_t* csv = csv_load("clusters.3d.csv");
    
    obj->size = csv->rows;

    // upload data to gpu mem
    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);
    {
        // allocate buffer objects for coordinates, colors and uvs
        glGenBuffers(1, &obj->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);

        // 1 - first xyz column
        glBufferData(GL_ARRAY_BUFFER, csv->rows * csv->cols * sizeof(float), csv->data, GL_STATIC_DRAW);       
        glVertexAttribPointer(0, point_size, GL_FLOAT, GL_FALSE, csv->cols * sizeof(float), (GLvoid*) (sizeof(float)*1)); 
        glEnableVertexAttribArray(0);

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
    }
    // unbind vbo & vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // free mem
    free(csv);

    return obj;
}

void 
obj_render(obj_p obj) {
    glBindVertexArray(obj->vao);
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, obj->size);
    //glDisableVertexAttribArray(1); 
    //glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}
