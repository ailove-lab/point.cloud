#pragma once

typedef struct {
    unsigned int cols;    // number of cols
    unsigned int rows;    // number of rows
    char** header;        // csv header
    float* data;          // data
    float* min;           // min of col
    float* max;           // max of col
    unsigned int* min_id; // min row
    unsigned int* max_id; // max row
    float* sum;           // col sum
    float* notzero;       // notzero count
} data_t;
typedef data_t* data_p;

void data_free(data_p csv);
data_p data_load(char* filename);