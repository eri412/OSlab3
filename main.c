#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SECOND2NANO 1000000000

typedef struct {
    size_t width;
    size_t height;
    double complex **buff;
} Matrice;

typedef struct {
    Matrice *lhs;
    Matrice *rhs;
    Matrice *result;
    size_t linear_start;
    size_t linear_end;
} _Matrice_params;

void matrice_fill(Matrice *subj) {
    scanf("%zu %zu", &subj->height, &subj->width);
    subj->buff = malloc(subj->height * sizeof(double complex *));
    for (size_t i = 0; i < subj->height; ++i) {
        subj->buff[i] = malloc(subj->width * sizeof(double complex));
        for (size_t j = 0; j < subj->width; ++j) {
            double temp_real;
            double temp_imag;
            scanf("%lf;%lf", &temp_real, &temp_imag);
            subj->buff[i][j] = CMPLX(temp_real, temp_imag);
        }
    }
}

void matrice_print(const Matrice subj) {
    for (size_t i = 0; i < subj.height; ++i) {
        for (size_t j = 0; j < subj.width; ++j) {
            printf("%.2f;%.2f\t", creal(subj.buff[i][j]), cimag(subj.buff[i][j]));
        }
        printf("\n");
    }
}

void matrice_free(Matrice subj) {
    for (size_t i = 0; i < subj.height; ++i) {
        free(subj.buff[i]);
    }
    free(subj.buff);
}

size_t _get_2d_x(const Matrice *matrice, size_t linear_coord) {
    return linear_coord % matrice->width;
}

size_t _get_2d_y(const Matrice *matrice, size_t linear_coord) {
    return linear_coord / matrice->width;
}

void *_matrice_indiv_thread(void *params_void) {
    _Matrice_params *params = (_Matrice_params *) params_void;
    for (size_t i = params->linear_start; i < params->linear_end; ++i) {
        size_t x = _get_2d_x(params->result, i);
        size_t y = _get_2d_y(params->result, i);
        params->result->buff[y][x] = CMPLX(0, 0);
        for (size_t j = 0; j < params->lhs->width; ++j) {
            params->result->buff[y][x] += params->lhs->buff[y][j] * params->rhs->buff[j][x];
        }      
    }
    return NULL;
}

Matrice matrice_mult_threads(Matrice lhs, Matrice rhs, unsigned int threads_limit) {
    if (lhs.width != rhs.height) {
        printf("inappropriate matrices' sizes\n");
        exit(1);
    }

    Matrice result;
    result.height = lhs.height;
    result.width = rhs.width;
    result.buff = malloc(result.height * sizeof(double complex *));
    for (size_t i = 0; i < result.height; ++i) {
        result.buff[i] = malloc(result.width * sizeof(double complex));
    }

    size_t linear_size = result.height * result.width;
    if (threads_limit > linear_size) {
        threads_limit = linear_size;
    }
    size_t quotient = linear_size / threads_limit;
    size_t remainder = linear_size % threads_limit;
    size_t linear_iter = 0;
    pthread_t threads[threads_limit];
    _Matrice_params params[threads_limit];

    for (unsigned int i = 0; i < threads_limit; ++i) {
        params[i].lhs = &lhs;
        params[i].rhs = &rhs;
        params[i].result = &result;
        params[i].linear_start = linear_iter;
        linear_iter += quotient;
        if (remainder > 0) {
            ++linear_iter;
            --remainder;
        }
        params[i].linear_end = linear_iter;
        if (pthread_create(&threads[i], NULL, _matrice_indiv_thread,  &params[i]) != 0) {
            printf("error with thread creating occured\n");
            exit(EXIT_FAILURE);
        }
    }
    for (unsigned int i = 0; i < threads_limit; ++i) {
        pthread_join(threads[i], NULL);
    }
    return result;
}

Matrice matrice_mult_casual(Matrice lhs, Matrice rhs) {
    Matrice result;
    result.height = lhs.height;
    result.width = rhs.width;
    result.buff = malloc(result.height * sizeof(double complex *));
    for (size_t i = 0; i < result.height; ++i) {
        result.buff[i] = malloc(result.width * sizeof(double complex));
    }

    for (size_t y = 0; y < result.height; ++y) {
        for (size_t x = 0; x < result.width; ++x) {
            result.buff[y][x] = 0;
            for (size_t i = 0; i < lhs.width; ++i) {
                result.buff[y][x] += lhs.buff[y][i] * rhs.buff[i][x];
            }
        }
    }

    return result;
}

int main(int argc, char **argv) {
    if (argc != 2 || atoi(argv[1]) == 0) {
        printf("bad arguments\n");
        exit(EXIT_FAILURE);
    }
    unsigned int threads_limit = atoi(argv[1]);
    Matrice lhs;
    matrice_fill(&lhs);
    Matrice rhs;
    matrice_fill(&rhs);
    Matrice result;

    struct timespec casual_start, casual_end;
    timespec_get(&casual_start, TIME_UTC);
    result = matrice_mult_casual(lhs, rhs);
    timespec_get(&casual_end, TIME_UTC);
    matrice_free(result);
    struct timespec thread_start, thread_end;
    timespec_get(&thread_start, TIME_UTC);
    result = matrice_mult_threads(lhs, rhs, threads_limit);
    timespec_get(&thread_end, TIME_UTC);

    fprintf(stderr, "Casual multiplying: %lf\n", ((casual_end.tv_sec * SECOND2NANO + casual_end.tv_nsec) - 
                                                 (casual_start.tv_sec * SECOND2NANO + casual_start.tv_nsec)) / (double) SECOND2NANO);
    fprintf(stderr, "Threading multiplying: %lf\n", ((thread_end.tv_sec * SECOND2NANO + thread_end.tv_nsec) - 
                                                     (thread_start.tv_sec * SECOND2NANO + thread_start.tv_nsec)) / (double) SECOND2NANO);
    matrice_print(result);

    matrice_free(lhs);
    matrice_free(rhs);
    matrice_free(result);

    return 0;
}
