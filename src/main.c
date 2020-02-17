#include <locale.h>

#include "bbgl.h"
#include "data.h"

data_p data;
int usage() {
    printf("Uasge:\n");
    printf("\t./point_cloud csv_file.csv\n\n");
    printf("csv file format:\n");
    printf("\t x, y, z, u, [category], [product 1], ..., [product n]\n\n");
    printf("First 4 columns of csv file, x y z u, will we used as point coordinates\n");
    printf("All cols, except the header, must be convertible to float\n");
    return 0;
}

int main(int argc, char** argv) {
    
    if(argc!=2) return usage();

    setlocale(LC_NUMERIC, "");
    struct lconv *loc = localeconv();
    loc->decimal_point = ".";
    loc->thousands_sep = " ";

    data = data_load(argv[1]);

    bbgl_init();
    bbgl_loop();
    data_free(data);
}
