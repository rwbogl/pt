/* ptc.c - primality test checker, optionally in 128 bits */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <argp.h>
#include "utils.h"
#include "cfinite.h"

T **RESULT;
T **TEMP;

struct arguments {
    T iterations;
    T start;
    unsigned int order;
    T *coeffs;
};

const char *argp_program_version = "ptc 0.1";
const char *argp_program_bug_address = "<robert.w.bliss@gmail.com>";
const char *args_doc = "start iterations [coefficients]";
const char *doc = "Compute the pseudoprimes of a Perrin-style primality test.\n\n"
"Try:\n\tptc 1 500000 0 1 1\n\tptc 1 1600000 1 1 0 1 0 0 4";

void run_search(struct arguments *args)
{
    T start = args->start;
    T iterations = args->iterations;
    unsigned int order = args->order;
    T *coeffs = args->coeffs;

    printf("ptc: checking terms %lu to %lu\n", start, start + iterations);

    TEMP = init_matrix(order);
    RESULT = init_matrix(order);

    recurrence *rec = init_rec(order);
    set_rec(rec, coeffs);
    mpz_t num;
    mpz_init(num);

    T expected = coeffs[0];

    T index;

    if (start == 0) {
        index = 1;
    } else {
        index = start;
    }

    for (; index < start + iterations; ++index) {
        if (index % 100000 == 0) {
            printf("checking term %lu\n", index);
        }

        T term = rec_term(rec, index, RESULT, TEMP, index);

        if (term == expected % index) {
            mpz_set_ui(num, index);
            if (mpz_probab_prime_p(num, 25) == 0) {
                printf("counterexample: %lu\n", index);
            }
        }
    }

    free_matrix(TEMP, order);
    free_matrix(RESULT, order);
    free_rec(rec);
    mpz_clear(num);
}

static int parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *args = state->input;

    switch (key) {
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                args->start = strtoull(arg, NULL, 10);
            } else if (state->arg_num == 1) {
                args->iterations = strtoull(arg, NULL, 10);
            } else {
                args->order++;
                args->coeffs = realloc(args->coeffs, sizeof(args->coeffs) * args->order);
                args->coeffs[args->order - 1] = strtoull(arg, NULL, 10);
            }
            break;
        case ARGP_KEY_END:
            if (args->order == 0) {
                argp_usage(state);
            }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct argp argp = {NULL, parse_opt, args_doc, doc};
    struct arguments args;

    args.order = 0;
    args.coeffs = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &args);

    print_time();
    run_search(&args);
    print_time();

    free(args.coeffs);
}
