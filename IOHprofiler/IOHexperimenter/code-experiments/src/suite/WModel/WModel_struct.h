#ifndef WMODEL_STRUCT
#define WMODEL_STRUCT

/**
 * @brief Data type for transform_vars_dummy.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  int *postion_match;
  IOHprofiler_problem_free_function_t old_free_problem;
} dummy_random_data_t;

/**
 * @brief Data type for transform_vars_epistasis.
 */
typedef struct {
  int *offset;
  int *epistasis_x;/*result of the epistasis transformation */
  IOHprofiler_problem_free_function_t old_free_problem;
} epistasis_data_t;

/**
 * @brief Data type for transform_vars_neutrality.
 */
typedef struct {
  int *offset;
  int *neutrality_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} neutrality_data_t;


typedef struct {
  double offset;
} ruggedness_data_t;

typedef struct {
  dummy_random_data_t *data1;
  neutrality_data_t *data2;
  epistasis_data_t *data3;
  ruggedness_data_t *data4;
  int * parameters;
  int number_of_layer;
  int old_dim;
} WModel_data_t;

#endif
