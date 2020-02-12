#pragma once

typedef struct {
    size_t cols;   // number of cols
    size_t rows;   // number of rows
    char** header; // csv header
    float* data;   // data
    float* min;   // min of col
    float* max;   // max of col
} csv_t;
typedef csv_t* csv_p;

void csv_free(csv_p csv);
csv_p csv_load(char* filename);