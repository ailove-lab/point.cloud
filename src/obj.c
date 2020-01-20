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
    float* points;
    const int point_size = 4;
    int points_count = csv_load(&points, "points.csv", point_size);
    if(points_count<0) return obj;

    obj->size = points_count;

    // upload data to gpu mem
    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);
    {
        // allocate buffer objects for coordinates, colors and uvs
        glGenBuffers(1, &obj->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glBufferData(GL_ARRAY_BUFFER, points_count*point_size*sizeof(float), points, GL_STATIC_DRAW);       
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, point_size*sizeof(float), 0); 
        glEnableVertexAttribArray(0);
    }
    // unbind vbo & vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // free mem
    free(points);

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
