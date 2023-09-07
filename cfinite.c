/* cfinite.c - compute terms of C finite sequences, optionally in 128 bits */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfinite.h"

// Multiply two nxn matrices, storing the result in `result`.
void matmul_n(T **result, T **a, T **b, int n, T modulus)
{
    int i, j, k;

    for (int i = 0; i < n; ++i) {
        memset(result[i], 0, sizeof(*result[i]) * n);
    }

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            for (k = 0; k < n; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }

            result[i][j] = result[i][j] % modulus;
        }
    }
}

unsigned int leading_bit_pos(unsigned int n)
{
    unsigned int count = 0;
    while (n > 0) {
        n >>= 1;
        count++;
    }

    return count - 1;
}

T **init_matrix(int n)
{
    T **mat = calloc(n, sizeof(*mat));

    for (int i = 0; i < n; ++i) {
        mat[i] = calloc(n, sizeof(*mat[i]));
    }

    return mat;
}

void free_matrix(T **mat, int n)
{
    for (int i = 0; i < n; ++i) {
        free(mat[i]);
    }

    free(mat);
}

void copy_mat(T **dest, T **src, int n)
{
    for (int i = 0; i < n; ++i) {
        memcpy(dest[i], src[i], sizeof(*dest[i]) * n);
    }
}

// Raise `mat` to the power `exp`, storing the result in `result`.
// Everything is nxn.
void pow_n(T **result, T **mat, T exp, T **temp, int n, T modulus)
{
    for (int i = 0; i < n; ++i) {
        memcpy(result[i], mat[i], sizeof(*result[i]) * n);
    }

    if (exp == 1) {
        return;
    }

    // printf("raising matrix to power %lu\n", exp);
    // printf("leading bit pos: %d\n", leading_bit_pos(exp));
 
    // Start at the second-most-significant bit.
    for (int bit = leading_bit_pos(exp) - 1; bit >= 0; --bit) {
        // printf("bit %d: %lu\n", bit, (exp >> bit) & 1);
        matmul_n(temp, result, result, n, modulus);
        if ((exp >> bit) & 1) {
            matmul_n(result, temp, mat, n, modulus);
        } else {
            copy_mat(result, temp, n);
        }
    }

    // print_n(result, n);
}

void print_n(T **mat, int n)
{
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (mat[i][j] > UINT64_MAX) {
                printf("[big] ");
            } else {
                printf("%lu ", (uint64_t) mat[i][j]);
            }
        }
        printf("\n");
    }
}

// Compute a term of the recurrence, using the already allocated result and
// temp matrices for computation.
T rec_term(recurrence *rec, T index, T **result, T **temp, T modulus)
{
    if (index < rec->order) {
        return rec->inits[index];
    }

    pow_n(result, rec->mat, index - rec->order + 1, temp, rec->order, modulus);
    // printf("matrix raised to %lu:\n", index - rec->order);
    // print_n(result, rec->order);
    T term = 0;

    for (int i = 0; i < rec->order; ++i) {
        term += result[0][i] * rec->inits[rec->order - i - 1];
    }

    return term % modulus;
}

recurrence *init_rec(int order)
{
    recurrence *ret = calloc(1, sizeof(*ret));
    ret->order = order;

    ret->mat = calloc(order, sizeof(*ret->mat));

    for (int i = 0; i < ret->order; ++i) {
        ret->mat[i] = calloc(order, sizeof(*ret->mat[i]));
    }

    ret->inits = calloc(order, sizeof(*ret->inits));

    for (int i = 0; i < order - 1; ++i) {
        ret->mat[1 + i][i] = 1;
    }

    return ret;
}

void free_rec(recurrence *rec)
{
    for (int i = 0; i < rec->order; ++i) {
        free(rec->mat[i]);
    }
    free(rec->mat);
    free(rec->inits);
    free(rec);
}

void set_rec(recurrence *rec, T *coeffs)
{
    int i;

    for (i = 0; i < rec->order; ++i) {
        rec->mat[0][i] = coeffs[i];
    }

    // Compute the initial terms using Newton's identities.
    // XXX: We're assuming that the initial conditions will be small, i.e.,
    // call calculations here will fit into a signed 64 bit integer. That's not
    // true in the general case, but very unlikely to happen.
    rec->inits[0] = rec->order;

    for (int k = 1; k < rec->order; ++k) {
        T sum = 0;
        for (int i = 1; i < k; ++i) {
            sum += coeffs[k - i - 1] * rec->inits[i];
        }


        rec->inits[k] = k * coeffs[k - 1] + sum;
    }
}

void print_coeffs(T *coeffs, int order)
{
    printf("[");
    for (int i = 0; i < order - 1; ++i) {
        printf("%lu, ", coeffs[i]);
    }
    printf("%lu]\n", coeffs[order - 1]);
}
