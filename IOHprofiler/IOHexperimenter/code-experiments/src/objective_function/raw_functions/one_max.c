/**
 * @Onemax function
 */

/**
 * @file f_one_max.c
 * @brief Implementation of the one_max function and problem.
 */

#ifndef ONE_MAX
#define ONE_MAX

#include <assert.h>
#include <stdio.h>
#include "../../profiler/IOHprofiler.h"

static double one_max_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    int result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    result = 0;
    for (i = 0; i < number_of_variables; ++i) {
        result += x[i];
    }

    return (double) result;
}

#endif
