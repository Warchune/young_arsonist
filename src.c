#include <stdio.h>
#include "young_arsonist.h"

#define FILE_IN "./src/f.in"
#define FILE_OUT "./src/f.out"

void load(){
    FILE *f_in;
    f_in = fopen(FILE_IN, "r");
    fscanf(f_in, "%d", &n);
    for (int i = 0; i < n; i++){
        fscanf(f_in, "%d%d%d%d%ld", &match[i].x1, &match[i].y1, &match[i].x2, &match[i].y2, &match[i].time);
    }
    fclose(f_in);
}

void save(){
    FILE *f_out;
    f_out = fopen(FILE_OUT, "w");
    fprintf(f_out, "%d %d\n"
                   "%.2Lf\n", res_x, res_y, res/2);
    fclose(f_out);
}