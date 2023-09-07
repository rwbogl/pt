#ifndef CFINITE_H
#define CFINITE_H

#include <inttypes.h>

typedef unsigned __int128 uint128_t;

#ifdef USE_128
typedef uint128_t T;
#else
typedef uint64_t T;
#endif

typedef struct {
    T **mat;
    T *inits;
    int order;
} recurrence;

T **init_matrix(int n);
void free_matrix(T **mat, int n);

void matmul_n(T **result, T **a, T **b, int n, T modulus);
void pow_n(T **result, T **mat, T exp, T **temp, int n, T modulus);
void print_n(T **mat, int n);
void copy_mat(T **dest, T **src, int n);

recurrence *init_rec(int order);
void set_rec(recurrence *rec, T *coeffs);
void free_rec(recurrence *rec);
void print_coeffs(T *coeffs, int order);
T rec_term(recurrence *rec, T index, T **result, T **temp, T modulus);

#endif
