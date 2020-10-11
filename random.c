#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("bad arguments\n");
        exit(1);
    }

    int h = atoi(argv[1]);
    int wh = atoi(argv[2]);
    int w = atoi(argv[3]);
    FILE *fp = fopen(argv[4], "w");
    fprintf(fp, "%d %d\n", h, wh);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < wh; ++j) {
            fprintf(fp, "%lf;%lf ", rand()%20 + rand()/((double) RAND_MAX), rand()%20 + rand()/((double) RAND_MAX));
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "%d %d\n", wh, w);
    for (int i = 0; i < wh; ++i) {
        for (int j = 0; j < w; ++j) {
            fprintf(fp, "%lf;%lf ", rand()%20 + rand()/((double) RAND_MAX), rand()%20 + rand()/((double) RAND_MAX));
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    return 0;
}
