/**
 * @file transform_vars_epistasis.c
 * @brief Implementation of epistasis all decision values by an offset.
 */
#ifndef EPISTASIS
#define EPISTASIS

#include <assert.h>

#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"

#include "../suite/WModel/WModel_struct.h"


void base_epistasis(const int * xIn, const int start, const int nu, int * xOut) {

    const int end = (start + nu) - 1;
    const int flip = xIn[start];
    int i,j;
    int result;
    int skip = start;
    for (i = end ; i >= start; --i) {
      result = flip;
      for (j = end; j > start; --j) {
        if (j != skip) {
          result ^= (xIn[j]);
        }
      }
      xOut[i] = result;
      if ((--skip) < start) {
        skip = end;
      }
    }
  }

void epistasis(const int * xIn, const int nu, int* xOut,int length) {
    const int end = length - nu;
    int i ;
    for (i=0; i <= end; i += nu) {
      base_epistasis(xIn, i, nu, xOut);
    }
    if (i < length) {
      base_epistasis(xIn, i, length - i, xOut);
    }
  }
static void layer_epistasis_compute(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
epistasis(x,block_size,epistasis_x,dimension);
}




static void epistasis_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  epistasis_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (epistasis_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  layer_epistasis_compute(x, data->epistasis_x, data->offset[0], problem->number_of_variables);

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void epistasis_free(void *thing) {
  epistasis_data_t *data = (epistasis_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_epistasis(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int epistasis_bounds) {
    
  if(*offset==0){return inner_problem;}
  epistasis_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  if (epistasis_bounds)
    IOHprofiler_error("epistasis_bounds not implemented.");

 
  data = (epistasis_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);

  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, epistasis_free, "epistasis");
  problem->evaluate_function = epistasis_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
#endif
