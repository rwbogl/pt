/* ptf.c - primality test finder, optionally in 128 bits */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "cfinite.h"
#include "utils.h"
#include <argp.h>

#define DEFAULT_MAX_COEFF 9
#define DEFAULT_SEARCH_LIMIT 100000000
#define DEFAULT_REPORT_LIMIT 100000
#define STR(A) _STR(A)
#define _STR(A) #A

struct arguments {
    unsigned int order;
    T *coeffs;
    T iterations;
    T max_coeff;
    T search_limit;
    T report_limit;
};

struct argp_option options[] =
{
{"search-limit", 's', "limit", 0, "Search for pseudoprimes up to search limit. Defaults to " STR(DEFAULT_SEARCH_LIMIT) "."},
{"report-limit", 'r', "limit", 0, "Report pseudoprimes greater than report limit. Defaults to " STR(DEFAULT_REPORT_LIMIT) "."},
{"max-coeff", 'm', "limit", 0, "Largest possible coefficient. Defaults to " STR(DEFAULT_MAX_COEFF) "."},
    {NULL}
};

const char *argp_program_version = "ptf 0.1";
const char *argp_program_bug_address = "<robert.w.bliss@gmail.com>";
const char *args_doc = "iterations [coefficients]";
const char *doc =
"Find good Perrin-style primality tests by brute-force searching over coefficients. "
"The search begins with the initial list and continues through lists of the same length "
"where all entries are between 0 and " STR(DEFAULT_MAX_COEFF) " by default.\n\n"
"Try:\n"
"\tptf 1000 0 0 0\n"
"\tptf 1000000 0 0 0 0 0 -r100000\n";

T **RESULT;
T **TEMP;

T first_pseudoprime(recurrence *rec, mpz_t num, T limit)
{
    T **mat = rec->mat;
    int order = rec->order;
    T expected = mat[0][0];

    T **result = init_matrix(order);
    T i = 0;

    for (i = rec->order + 1; i <= limit; ++i) {
        T term = rec_term(rec, i, RESULT, TEMP, i);
        if (term == expected % i) {
            mpz_set_ui(num, i);

            if (mpz_probab_prime_p(num, 25) == 0) {
                free_matrix(result, order);
                return i;
            }
        }
    }

    free_matrix(result, order);
    return i;
}

void run_search(struct arguments *args)
{
    printf("ptf: checking %lu recurrences starting from ", args->iterations);
    print_coeffs(args->coeffs, args->order);
    printf("MAX_COEFF is %lu\n", args->max_coeff);
    printf("SEARCH_LIMIT is %lu\n", args->search_limit);
    printf("REPORT_LIMIT is %lu\n", args->report_limit);

    unsigned int order = args->order;
    T iterations = args->iterations;
    T *coeffs = args->coeffs;

    TEMP = init_matrix(order);
    RESULT = init_matrix(order);

    recurrence *rec = init_rec(order);
    mpz_t num;
    mpz_init(num);

    int quit = 0;
    for (uint64_t count = 0; !quit && count < iterations; ++count) {
        if (count % 1000 == 0) {
            printf("checking recurrence %lu\n", count);
        }

        set_rec(rec, coeffs);
        T pseudo = first_pseudoprime(rec, num, args->search_limit);
        if (pseudo > args->report_limit) {
            printf("report: %lu ", pseudo);
            print_coeffs(coeffs, order);
        }

        coeffs[order - 1]++;

        int carry_count = order - 1;
        while (coeffs[carry_count] > args->max_coeff) {
            // Can't carry beyond the most significant digit.
            if (carry_count == 0) {
                printf("hit order barrier at ");
                print_coeffs(coeffs, order);
                quit = 1;
                break;
            }
            coeffs[carry_count] = 0;
            carry_count--;
            coeffs[carry_count]++;
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
                args->iterations = strtoull(arg, NULL, 10);
            } else {
                args->order++;
                args->coeffs = realloc(args->coeffs, sizeof(args->coeffs) * args->order);
                args->coeffs[args->order - 1] = strtoull(arg, NULL, 10);
            }
            break;
        case 's':
            args->search_limit = strtoull(arg, NULL, 10);
            break;
        case 'r':
            args->report_limit = strtoull(arg, NULL, 10);
            break;
        case 'm':
            args->max_coeff = strtoull(arg, NULL, 10);
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
    struct argp argp = {options, parse_opt, args_doc, doc};

    struct arguments args;
    args.max_coeff = DEFAULT_MAX_COEFF;
    args.search_limit = DEFAULT_SEARCH_LIMIT;
    args.report_limit = DEFAULT_REPORT_LIMIT;
    args.order = 0;
    args.coeffs = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &args);

    print_time();
    run_search(&args);
    free(args.coeffs);
    print_time();

    return 0;
}
