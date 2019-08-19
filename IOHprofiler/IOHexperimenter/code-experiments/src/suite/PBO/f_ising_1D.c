/**
 * @file f_ising_1D.c
 * @brief Implementation of the ising_1D function and problem.
 */

#include <assert.h>
#include <stdio.h>

#ifndef IOHPROFILER
#define IOHPROFILER
#include "../../profiler/IOHprofiler.h"
#endif

#ifndef IOHPROFILER_UTILITIES
#define IOHPROFILER_UTILITIES
#include "../../profiler/IOHprofiler_utilities.c"
#endif

#include "suite_PBO_legacy_code.c"
#include "../../transform/transform_obj_shift.c"
#include "../../transform/transform_vars_sigma.c"
#include "../../transform/transform_vars_xor.c"
#include "../../transform/transform_obj_scale.c"


/**
 * @brief Implements the Ising one dimension function without connections to any IOHprofiler structures.
 */


int modulo_ising_1D(int x,int N){
    return (x % N + N) %N;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static int f_ising_1D_raw(const int *x, const size_t number_of_variables) {
    int result= 0;
    size_t i;
    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    for (i = 0; i < number_of_variables; ++i) {
        int first_neig=x[modulo_ising_1D((i+1), number_of_variables)];
        int second_neig=x[modulo_ising_1D((i -1) , number_of_variables)];

        result += (x[i] *first_neig) + ((1- x[i])*(1- first_neig));
        result += (x[i] *second_neig) + ((1- x[i])*(1- second_neig));
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_ising_1D_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_ising_1D_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic ising_1D problem.
 */
static IOHprofiler_problem_t *f_ising_1D_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("ising_1D function",
                                                                               f_ising_1D_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "ising_1D", number_of_variables);

    /* Compute best solution */
    f_ising_1D_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler ising_1D problem.
 */
static IOHprofiler_problem_t *f_ising_1D_IOHprofiler_problem_allocate(const size_t function,
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
    problem = f_ising_1D_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
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
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        for (i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem, z, 0);
        problem = transform_obj_shift(problem, a);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
