#include "bbgl.h"
#include "data.h"

data_p data;

int main(int argc, char** argv) {
    data = data_load("_clusters.4d.csv");
    bbgl_init();
    bbgl_loop();
    data_free(data);
}
