#pragma once

typedef struct {
    size_t cols;   // number of cols
    size_t rows;   // number of rows
    char** header; // csv header
    float* data;  // data
    double* min;   // min of col
    double* max;   // max of col
} csv_t;

void csv_free(csv_t* csv);
csv_t* csv_load(char* filename);