#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POINT_COUNT 2000000

// simple float loader
int csv_load(float** result, char* filename, int cols) {

    FILE* fp= fopen(filename, "r");
    if (!fp) return -1;
    
    *result = malloc(POINT_COUNT * cols * sizeof(float));

    char line[1024];
    // skip header
    fgets(line, 1024, fp);
    int i = 0;
    while(fgets(line, 1024, fp)) {
        char* tmp = line;
        // printf("%s\n", line);
        int j;
        char* t;
        for(j=0, t=strtok(tmp,",");
            t && *t; 
            j++, t = strtok(NULL, ",\n")) {
            float f = atof(t);
            (*result)[i*cols+j] = f;
	    }
        i++;
        if(i>=POINT_COUNT) break;
    }
    fclose(fp);
    return i;
}
