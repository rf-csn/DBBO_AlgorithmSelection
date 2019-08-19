/**
 * @Onemax function
 */

/**
 * @file f_one_max_ruggedness3.c
 * @brief Implementation of the one_max_ruggedness3 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#ifndef IOHPROFILER
#define IOHPROFILER
#include "../../profiler/IOHprofiler.h"
#endif
#ifndef IOHPROFILER_PROBLEM
#define IOHPROFILER_PROBLEM
#include "../../profiler/IOHprofiler_problem.c"
#endif
#include "suite_PBO_legacy_code.c"
#include "../../transform/transform_obj_shift.c"
#include "../../transform/transform_obj_scale.c"
#include "../../transform/transform_obj_ruggedness3.c"


/**
 * @brief Implements the one_max_ruggedness3 function without connections to any IOHprofiler structures.
 */
static double f_one_max_ruggedness3_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    double result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        result += (double)x[i];
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_one_max_ruggedness3_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_ruggedness3_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness3_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_ruggedness3 function",
                                                                               f_one_max_ruggedness3_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_ruggedness3", number_of_variables);

    /* Compute best solution */
    f_one_max_ruggedness3_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness3_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_one_max_ruggedness3_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness3(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp; 
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        problem = transform_obj_ruggedness3(problem);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
