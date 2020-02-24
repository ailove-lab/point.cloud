#pragma once

typedef struct cluster_stat_t cluster_stat_t;

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
    int clusters_cnt;     // number of cluster, max_id - min_id 
    cluster_stat_t* clusters; // clusters stat
} data_t;
typedef data_t* data_p;

// category statistics
typedef struct cat_stat_t {
    int id;
    float sum;
} cat_stat_t;

typedef struct cluster_stat_t {
    int     id;          // cluster id
    int     cnt;         // members count
    float   sum;         // cluster categorie sum
    cat_stat_t* cat_sum; // per category sum
} cluster_stat_t;

void data_free(data_p csv);
data_p data_load(char* filename);
