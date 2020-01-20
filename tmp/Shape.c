#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Shape.h"

static void InitPlane(Shape*);

static void
Init(Shape* outShape) {
    ADrawable->Init(outShape->drawable);
    InitPlane(outShape);
}
static Shape*
Create() {
    Shape* shape = malloc(sizeof(Shape));
    Init(shape);
    return shape;
}

static void
Release(Shape* shape) {
    ADrawable->Release(shape->drawable);
}

static void
InitPlane(Shape* outShape) {
    Drawable* drawable = outShape->drawable;
    
    #define POINT_COUNT 2000000
    #define POINT_SIZE 4
    float* pos_clust=malloc(POINT_COUNT*POINT_SIZE*sizeof(float));
    drawable->size = POINT_COUNT;

    FILE* fp= fopen("points.csv", "r");
    char line[1024];
    // skip header
    fgets(line, 1024, fp);
    int i = 0;
    while(fgets(line, 1024, fp)) {
        char* tmp = line;
	    int j = 0;
	    const char* tok;
        for(tok = strtok(tmp, ","); 
            tok && *tok; 
            j++, tok = strtok(NULL, ",\n")) {
            float f = atof(tok);
            pos_clust[i*4+j] = f;
            // printf("%f ", f);
	    }
        // printf("\n");
        i++;
        if(i>=POINT_COUNT) break;
    }
    fclose(fp);
    printf("Number of points: %d\n", i);

    glGenVertexArrays(1, &drawable->vao);
    glBindVertexArray(drawable->vao);
    {
        // allocate buffer objects for coordinates, colors and uvs
        glGenBuffers(1, &drawable->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo);
        glBufferData(GL_ARRAY_BUFFER, i*POINT_SIZE*sizeof(float), pos_clust, GL_STATIC_DRAW);       
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, POINT_SIZE*sizeof(float), 0); 
        glEnableVertexAttribArray(0);
    }
    // unbind vbo & vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    free(pos_clust);
}

struct AShape AShape[1] = {{
    Create,
    Init,
    Release,
}};
