/**
 * @file transform_vars_sigma.c
 * @brief Implementation of sigmad all decision values by an offset.
 */
#ifndef TRANSFORM_VARS_SIGMA
#define TRANSFORM_VARS_SIGMA

#include <assert.h>

#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_sigma.
 */
typedef struct {
  int *offset;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_sigma_data_t;

/**
 * @brief Evaluates the transformation.
 */
static int sigma_compute(const int *x, const int pos){
  return x[pos];
}

static void transform_vars_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
  	IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
  	return;
  }
  data = (transform_vars_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = sigma_compute(x,data->offset[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];
  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_sigma_free(void *thing) {
  transform_vars_sigma_data_t *data = (transform_vars_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->sigma_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int sigma_bounds) {
    
  transform_vars_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  size_t i;
  if (sigma_bounds)
    IOHprofiler_error("sigma_bounds not implemented.");

  data = (transform_vars_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->sigma_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  temp_best = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_sigma_free, "transform_vars_sigma");
  problem->evaluate_function = transform_vars_sigma_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = sigma_compute(problem->best_parameter,data->offset[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  IOHprofiler_free_memory(temp_best);
  return problem;
}
#endif
