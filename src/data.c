#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "data.h"

#define MAX_COLS 1024
#define LINE_SIZE 1024*1024

static void parse_header(data_t* data, char* header);
static void parse_clusters(data_t* data);

void data_free(data_t* data) {
    // cleanup header
    if(data) {    
        for(size_t i = 0; i<data->cols; i++) free(data->header[i]);
        free(data->header);
        free(data->data);
        free(data->min);
        free(data->max);
        free(data->min_id);
        free(data->max_id);
        free(data->sum);
        free(data->notzero);
        
        for(int i=0; i<data->clusters_cnt; i++) free(data->clusters[i].cat_sum);
        free(data->clusters); 

        free(data);
    }
}

data_t* 
data_load(char* filename) {

    printf("Loading %s\n", filename);
    
    FILE* fp= fopen(filename, "r");
    if (!fp) return NULL;
    
    // allocate structure
    data_t* data = calloc(1, sizeof(data_t));
    // count lines
    while(!feof(fp)) if(fgetc(fp) == '\n') data->rows++;
    printf("Counted: %d rows\n", data->rows);
    rewind(fp);

    //parse header
    char line[LINE_SIZE];
    fgets(line, LINE_SIZE, fp);
    parse_header(data, line);
    
    data->data    = calloc((data->rows+1) * data->cols, sizeof(float));
    data->min     = calloc(data->cols, sizeof(float));
    data->max     = calloc(data->cols, sizeof(float));
    data->sum     = calloc(data->cols, sizeof(float));
    data->notzero = calloc(data->cols, sizeof(float));
    data->min_id  = calloc(data->cols, sizeof(unsigned int));
    data->max_id  = calloc(data->cols, sizeof(unsigned int));
    
    size_t row = 0;
    while(fgets(line, LINE_SIZE, fp)) {
        char* tmp = line;
        int col;
        char* t;
        for(col=0, t=strtok(tmp,",");
            t && *t; 
            col++, t = strtok(NULL, ",\n")) {
            float f = atof(t);
            data->data[row * data->cols + col] = f;
            if (f != 0.0) {
                data->notzero[col]++;
                data->sum[col]+= f;
            }
            if (data->min[col] > f) {
                data->min[col] = f;
                data->min_id[col] = row;
            }
            if (data->max[col] < f) {
                data->max[col] = f;
                data->max_id[col] = row;
            }
            // Break if data greater than header
            if(col>=data->cols) break;
	    }
        row++;
    }
    
    parse_clusters(data);
    
    printf("loaded: %ld rows\n", row);
    printf("data->cols: %d\n", data->cols);
    printf("data->rows: %d\n", data->rows);

    fclose(fp);
    
    return data;
}


static int clusters_comp(const void* a, const void* b) {
    cluster_stat_t* c1 = a;
    cluster_stat_t* c2 = b;
    return c2->sum - c1->sum;
}

static int cat_stat_comp(const void* a, const void* b) {
    cat_stat_t* c1 = a;
    cat_stat_t* c2 = b;
    return c2->sum - c1->sum;
}

static void parse_clusters(data_t* data) {
    // search default cluster column if need
    if (cluster_col<0) {
        for(int col=0; col < data->cols; col++) {
            if(strcmp(data->header[col], "clust"  ) == 0 || 
               strcmp(data->header[col], "кластер") == 0 ) {
                cluster_col = col;
                break;
            }
        }
    }

    // cluster col not set
    if(cluster_col < 0) return;
    printf("cluster_col found: %d\n", cluster_col);
    
    // min / max values of cluster id
    int min = (int)data->min[cluster_col];
    int max = (int)data->max[cluster_col];
    printf("Cluster ids %d - %d\n", min, max);
    
    // init clusters stat sturcts 
    data->clusters_cnt = max - min + 1;
    data->clusters = calloc(data->clusters_cnt, sizeof(cluster_stat_t));
    for(int cls=0; cls<data->clusters_cnt; cls++) { 
        data->clusters[cls].cat_sum = calloc(data->cols, sizeof(cat_stat_t));
        // init col ids
        for(int col=0; col < data->cols; col++) {
            data->clusters[cls].cat_sum[col].id = col;
        }
    }

    // aggregate clusters stats
    for(int row = 0; row < data->rows; row++) {
        int cluster = (int) (data->data[row*data->cols + cluster_col]);
        int cid = cluster - min; // min = -1 by deafult
        // count members of cluster
        data->clusters[cid].cnt++;
        data->clusters[cid].id = cluster; 
        for(int col = 0; col < data->cols; col++) {
            // get category float
            float f = data->data[row*data->cols + col];
            if(f>0.0) {
                // integrate per category sum
                data->clusters[cid].cat_sum[col].sum += f;
                // integrate only categories cols
                if (col >= categories_start)
                    data->clusters[cid].sum += f;
            }
        }
    }

    // sort clusters by total sum
    qsort(data->clusters, 
        data->clusters_cnt, 
        sizeof(cluster_stat_t), 
        clusters_comp);
    // sort cluster stat by category sum
    for(int cls=0; cls<data->clusters_cnt; cls++) { 
        qsort(data->clusters[cls].cat_sum, 
            data->cols, 
            sizeof(cat_stat_t), 
            cat_stat_comp); 
    }
}

static void parse_header(data_t* data, char* header) {
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
        names[data->cols] = strdup(t);
        // printf("%d\t%s\n", data->cols, t);
        data->cols++;
        if(data->cols >= MAX_COLS) break;

        while(n[0]==' '|| n[0]==',') n++;
        if(n[0]==0) break;
        // check if name is quoted
        t = (n[0]=='"') ? strtok_r(NULL, "\"", &n) : strtok_r(NULL, ",\n", &n);
    }
    size_t l = sizeof(char*) * data->cols;
    data->header = malloc(l);
    memcpy(data->header, names, l);

    free(s);
}

#ifdef TEST_CSV
int main() {
    data_t* data = data_load("data.basket.data");
    
    printf("cols: %zu\n", data->cols);
    printf("rows: %zu\n", data->rows);

    for(int i=0; i< data->cols; i++) {
        printf("%s min: %0.0f %0.0f\n", data->header[i], data->min[i], data->max[i]);
    }
    /*
    for(int j=0; j< data->rows; j++) {
        for(int i=0; i< data->cols; i++) {
            if(data->data[data->cols*j+i] >0.0)
            printf("%s: %0.0f ", data->header[i], data->data[data->cols*j+i]);
        }
        printf("\n");
    }
    */
    data_free(data); 
    return 0;
}
#endif
