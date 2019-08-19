/**
 * @file transform_vars_neutrality.c
 * @brief Implementation of neutrality all decision values by an offset.
 */
#ifndef NEUTRALITY
#define NEUTRALITY

#include <assert.h>

#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"

#include "../suite/WModel/WModel_struct.h"


static int layer_neutrality_compute_old(const int *x, const int index, const int block_size){
  size_t number_of_one, number_of_zero, i;
  number_of_zero = 0;
  number_of_one = 0;
  i = 0;
  while(i < block_size){
    if(x[index + i] == 0){
      number_of_zero++;
    }
    if(x[index + i] == 1){
      number_of_one++;
    }
    ++i;
  }
  return (number_of_zero >= number_of_one ? 0 : 1);
}
static void layer_neutrality_compute(const int *xIn, int *xOut, const int mu,int dim,int temp_dim){
  int thresholdFor1 = (mu >> 1) + (mu & 1);
  int i,j,ones,flush;
  int temp;
  i=0;j=0; ones=0; flush=mu;
  while((i<dim) && (j<temp_dim)){
    if (xIn[i]==1){
      ones+=1;
    }
    i+=1;
    if(i>=flush){
      flush+=mu;
      if (ones >= thresholdFor1){temp=1;}else{temp=0;}
      xOut[j]=temp;
      j+=1;
      ones=0;
    }
  }
}
/*
static void neutrality_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  neutrality_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (neutrality_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = layer_neutrality_compute(x, i*data->offset[0], data->offset[0]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->neutrality_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}*/

/**
 * @brief Frees the data object.
 */
static void neutrality_free(void *thing) {
  neutrality_data_t *data = (neutrality_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
/*
static IOHprofiler_problem_t *transform_neutrality(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int neutrality_bounds) {
    
  if(*offset==0){return inner_problem;}
  neutrality_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (neutrality_bounds)
    IOHprofiler_error("neutrality_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / offset[0];
  }
  else{
    return inner_problem;
  }
  
  assert(new_dimension > 0);
  data = (neutrality_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);

  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, neutrality_free, "neutrality");
  problem->number_of_variables = new_dimension;
  problem->evaluate_function = neutrality_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = layer_neutrality_compute(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  return problem;
}*/
#endif
