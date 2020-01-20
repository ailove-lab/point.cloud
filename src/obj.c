#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl3w.h>

#include "obj.h"

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
    
    #define POINT_COUNT 2000000
    #define POINT_SIZE 4
    float* pos_clust = malloc(POINT_COUNT * POINT_SIZE * sizeof(float));
    obj->size = POINT_COUNT;

    FILE* fp= fopen("points.csv", "r");
    char line[1024];
    // skip header
    fgets(line, 1024, fp);
    int i = 0;
    while(fgets(line, 1024, fp)) {
        char* tmp = line;
	    int j = 0;
	    char* tok;
        //printf("%s\n", line);
        for(tok = strtok(tmp, ","); 
            tok && *tok; 
            j++, tok = strtok(NULL, ",\n")) {
            float f = atof(tok);
            //f = f/(1+abs(f));
            pos_clust[i*4+j] = f;
            //printf("%f ", f);
	    }
        // printf("\n");
        // if(pos_clust[i*4+3]<0.0) 
        i++;
        if(i>=POINT_COUNT) break;
    }
    fclose(fp);
    printf("Number of points: %d\n", i);

    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);
    {
        // allocate buffer objects for coordinates, colors and uvs
        glGenBuffers(1, &obj->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glBufferData(GL_ARRAY_BUFFER, i*POINT_SIZE*sizeof(float), pos_clust, GL_STATIC_DRAW);       
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, POINT_SIZE*sizeof(float), 0); 
        glEnableVertexAttribArray(0);
    }
    // unbind vbo & vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(pos_clust);

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
