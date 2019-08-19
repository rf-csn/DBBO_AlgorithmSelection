/**
 * @file transform_vars_dummy.c
 * @brief Implementation of dummyd all decision values by an offset.
 */
#ifndef DUMMY_RANDOM
#define DUMMY_RANDOM

#include <assert.h>

#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"
#include "../profiler/IOHprofiler_random.c"
#include "../suite/WModel/WModel_struct.h"




static void compute_dummy_random_match(int * postion_match, const int old_dimension, const int new_dimension){
  size_t i,j,l;
  int temp;
  double *randN = IOHprofiler_allocate_vector(1);
  int flag;
  int dummyins = 10000;
  IOHprofiler_random_state_t *random_generator=IOHprofiler_random_new(dummyins);
  int * flip;

  l = new_dimension;
  flip = IOHprofiler_allocate_int_vector(l);
  for(i = 0; i < l; ++i){
    while(1){
      flag = 0;
      temp = (int)(IOHprofiler_random_uniform(random_generator) * old_dimension);
      for(j = 0; j < i; ++j){
        if(temp == postion_match[j]){
          flag = 1;
          break;
        }
      }
      if(flag == 0){break;}
    }
    postion_match[i] = temp;
  }
  IOHprofiler_free_memory(flip);
  IOHprofiler_free_memory(randN);
  IOHprofiler_random_free(random_generator);
}

static void dummy_random_evaluate(IOHprofiler_problem_t *problem, int *x, double *y) {
  size_t i;
  dummy_random_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (dummy_random_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
/*fprintf(stderr,"inner problem name %s, dim %d\n",inner_problem->problem_name,inner_problem->number_of_variables);
fprintf(stderr,"len %d\n",problem->number_of_variables);*/
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[data->postion_match[i]];
/*fprintf(stderr,"%d ",x[data->postion_match[i]]);*/
  }
/*fprintf(stderr,"\n");*/
  IOHprofiler_evaluate_function(inner_problem, data->reduncted_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void dummy_random_free(void *thing) {
  dummy_random_data_t *data = (dummy_random_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->postion_match);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_dummy_random(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int dummy_bounds) {
  if(*offset<2){return inner_problem;}
  dummy_random_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (dummy_bounds)
    IOHprofiler_error("dummy_bounds not implemented.");
 
  
  new_dimension = offset[0];
  assert(new_dimension <= inner_problem->number_of_variables);
  data = (dummy_random_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->postion_match = IOHprofiler_allocate_int_vector(new_dimension);
  compute_dummy_random_match(data->postion_match,inner_problem->number_of_variables,new_dimension);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, dummy_random_free, "dummy_random");
  problem->number_of_variables = new_dimension;
  /*inner_problem->number_of_variables = problem->number_of_variables;*/

  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    /*inner_problem->number_of_variables = problem->number_of_variables;*/
  }

  problem->evaluate_function = dummy_random_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = problem->best_parameter[data->postion_match[i]];
  }
  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  return problem;
}
#endif
