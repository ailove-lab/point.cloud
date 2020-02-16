#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

#define MAX_COLS 1024
#define LINE_SIZE 1024*1024

static void parse_header(data_t* csv, char* header);

void data_free(data_t* csv) {
    // cleanup header
    if(csv) {    
        for(size_t i = 0; i<csv->cols; i++) free(csv->header[i]);
        free(csv->header);
        free(csv->data);
        free(csv->min);
        free(csv->max);
        free(csv->min_id);
        free(csv->max_id);
        free(csv);
    }
}

data_t* data_load(char* filename) {

    printf("Loading %s\n", filename);
    
    FILE* fp= fopen(filename, "r");
    if (!fp) return NULL;
    
    // allocate structure
    data_t* csv = malloc(sizeof(data_t));
    *csv = (data_t){0};
    // count lines
    while(!feof(fp)) if(fgetc(fp) == '\n') csv->rows++;
    printf("Counted: %ld rows\n", csv->rows);
    rewind(fp);

    //parse header
    char line[LINE_SIZE];
    fgets(line, LINE_SIZE, fp);
    parse_header(csv, line);
    
    csv->data   = calloc((csv->rows+1) * csv->cols, sizeof(float));
    csv->min    = calloc(csv->cols, sizeof(float));
    csv->max    = calloc(csv->cols, sizeof(float));
    csv->min_id = calloc(csv->cols, sizeof(unsigned int));
    csv->max_id = calloc(csv->cols, sizeof(unsigned int));
    
    size_t row = 0;
    while(fgets(line, LINE_SIZE, fp)) {
        char* tmp = line;
        int col;
        char* t;
        for(col=0, t=strtok(tmp,",");
            t && *t; 
            col++, t = strtok(NULL, ",\n")) {
            float f = atof(t);
            csv->data[row * csv->cols + col] = f;
            if (csv->min[col] > f) {
                csv->min[col] = f;
                csv->min_id[col] = row;
            }
            if (csv->max[col] < f) {
                csv->max[col] = f;
                csv->max_id[col] = row;
            }
            // Break if data greater than header
            if(col>=csv->cols) break;
	    }
        row++;
    }
    
    printf("loaded: %ld rows\n", row);
    printf("csv->cols: %ld\n", csv->cols);
    printf("csv->rows: %ld\n", csv->rows);

    fclose(fp);
    
    return csv;
}

static void parse_header(data_t* csv, char* header) {
    char* s = strdup(header);
    char* t; // token
    char* n; // tail

    // names cache
    char* names[MAX_COLS];
    // Check if first name is quoted
    t = (s[0]=='"') ? strtok_r(s, "\"", &n) : strtok_r(s, ",\n", &n);
    while(t && *t) {
        // trim comma, spaces
        while(t[0]==' ') t++;
        names[csv->cols] = strdup(t);
        // printf("%d\t%s\n", csv->cols, t);
        csv->cols++;
        if(csv->cols >= MAX_COLS) break;

        while(n[0]==' '|| n[0]==',') n++;
        if(n[0]==0) break;
        // check if name is quoted
        t = (n[0]=='"') ? strtok_r(NULL, "\"", &n) : strtok_r(NULL, ",\n", &n);
    }
    size_t l = sizeof(char*) * csv->cols;
    csv->header = malloc(l);
    memcpy(csv->header, names, l);

    free(s);
}

#ifdef TEST_CSV
int main() {
    data_t* csv = data_load("data.basket.csv");
    
    printf("cols: %zu\n", csv->cols);
    printf("rows: %zu\n", csv->rows);

    for(int i=0; i< csv->cols; i++) {
        printf("%s min: %0.0f %0.0f\n", csv->header[i], csv->min[i], csv->max[i]);
    }
    /*
    for(int j=0; j< csv->rows; j++) {
        for(int i=0; i< csv->cols; i++) {
            if(csv->data[csv->cols*j+i] >0.0)
            printf("%s: %0.0f ", csv->header[i], csv->data[csv->cols*j+i]);
        }
        printf("\n");
    }
    */
    data_free(csv); 
    return 0;
}
#endif