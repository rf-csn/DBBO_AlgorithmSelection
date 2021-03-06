/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_dummy2.c
 * @brief Implementation of the leading_ones_dummy2 function and problem.
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
#include "../../transform/transform_vars_sigma.c"
#include "../../transform/transform_vars_xor.c"
#include "../../transform/transform_vars_dummy.c"
#include "../../transform/transform_obj_scale.c"
 
/**
 * @brief Implements the leading_ones_dummy2 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_dummy2_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    double result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        if (x[i] == 1.0) {
            result = (double)(i + 1);
        } else {
            break;
        }
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_leading_ones_dummy2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_dummy2_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_dummy2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy2_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_dummy2 function",
                                                                               f_leading_ones_dummy2_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_dummy2", number_of_variables);

    /* Compute best solution */
    f_leading_ones_dummy2_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_dummy2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy2_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int *z, *sigma,*dummy;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;

    dummy = IOHprofiler_allocate_int_vector(1);
    problem = f_leading_ones_dummy2_allocate(dimension);
    if(instance == 1){
        dummy[0] = dimension * 0.9;
        problem = transform_vars_dummy(problem, dummy, 0);   
    }
    else if(instance > 1 && instance <= 50){
        dummy[0] = dimension * 0.9;
        
        z = IOHprofiler_allocate_int_vector(dummy[0]);
        IOHprofiler_compute_xopt(z,rseed,dummy[0]);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_dummy_xor(problem,dummy,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {

        dummy[0] = dimension * 0.9;
        sigma = IOHprofiler_allocate_int_vector(dummy[0]);
        xins = IOHprofiler_allocate_vector(dummy[0]);
        IOHprofiler_compute_xopt_double(xins,rseed,dummy[0]);
        for(i = 0; i < dummy[0]; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dummy[0]; i++){
            t = (int)(xins[i] * (double)dummy[0]);
            assert(t >= 0 && t < dummy[0]);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp; 
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_dummy_sigma(problem, dummy, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        dummy[0] = dimension * 0.9;
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(dummy);
    
    return problem;
}
