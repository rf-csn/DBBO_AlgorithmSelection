/**
 * @file f_MIS.c
 * @brief Implementation of the Maximum Independent Set function and problem.
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
#include "../../transform/transform_obj_scale.c"

/**
 * @brief Implements the Maximum Independent Set function without connections to any IOHprofiler structures.
 */

 static int isEdge(int i, int j, size_t problem_size) {
    if (i!=problem_size/2 && j==i+1){return 1;}
    else if (i<=(problem_size/2)-1 && j==i+(problem_size/2)+1 ){return 1;}
    else if (i<=(problem_size/2) && i>=2 && j==i+(problem_size/2)-1){return 1;}
    else {return 0;}
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static int f_MIS_raw(const int *x, const size_t number_of_variables) {

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    int i, j,index;
    int result= 0;
    int num_of_ones=0;
    int sum_edges_in_the_set=0;
    int number_of_variables_even=number_of_variables;

    if (number_of_variables%2!=0){
        number_of_variables_even=number_of_variables-1;
    }

    int ones_array[number_of_variables_even+1];

    for (index=0; index<number_of_variables_even; index++){
        if (x[index]==1){
            ones_array[num_of_ones] = index;
            num_of_ones+=1;
        }
    }

    for (i=0; i<num_of_ones; i++){
        for (j=i+1; j<num_of_ones; j++){
            if(isEdge(ones_array[i]+1,ones_array[j]+1,number_of_variables_even)==1){
                sum_edges_in_the_set+=1;
            }
        }
    }
    result=num_of_ones - (number_of_variables_even*sum_edges_in_the_set);
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_MIS_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_MIS_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic MIS problem.
 */
static IOHprofiler_problem_t *f_MIS_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("MIS function",
                                                                               f_MIS_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "MIS", number_of_variables);

    /* Compute best solution */
    /*f_MIS_evaluate(problem, problem->best_parameter, problem->best_value);*/
    problem->best_value[0] = number_of_variables / 2 + 1.0;
    return problem;
}

/**
 * @brief Creates the IOHprofiler MIS problem.
 */
static IOHprofiler_problem_t *f_MIS_IOHprofiler_problem_allocate(const size_t function,
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
    problem = f_MIS_allocate(dimension);

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
