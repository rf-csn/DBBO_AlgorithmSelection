#ifndef MODULAR_PROBLEM
#define MODULAR_PROBLEM

#include <assert.h>
#include <stdio.h>

#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"
#include "../suite/WModel/suite_WModel_legacy_code.c"
#include "../suite/WModel/utils.c"

/*Load layers of WModel*/
#include "../layer/ruggedness.c"
#include "../layer/dummy_random.c"
#include "../layer/epistasis.c"
#include "../layer/neutrality.c"

/*Load basic transformation of problems (scale/shift/xor/sigma*/
#include "../transform/transform_obj_scale.c"
#include "../transform/transform_obj_shift.c"
#include "../transform/transform_vars_shift.c"
#include "../transform/transform_vars_sigma.c"
#include "../transform/transform_vars_xor.c"

/*Load all raw problems*/
#include "raw_functions/one_max.c"

enum IOHProblem {one_max=1};
enum WModel_layer {Dummy=0,Neutrality=1, Epistasis=2,Ruggedness=3};
static int id_problem=1;


static double f_raw(const int *x, const size_t number_of_variables){
  double res;
  if (id_problem==1){
    res=(double) one_max_raw(x, number_of_variables);
    return res;
  }
  else {return (double) 0;}
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_raw(x, problem->number_of_variables);
/*    if(y[0] > problem->best_value[0]){
     int i;
     for(i=0;i<problem->number_of_variables;i++){fprintf(stderr,"%d ",x[i]);}
      fprintf(stderr,"nb_var:%d best :%lf get :%lf\n",problem->number_of_variables,problem->best_value[0],y[0]);
    }*/
    assert(y[0] <= problem->best_value[0]);
}
static void inner_problem_free_function(IOHprofiler_problem_t *inner_problem){
  if(inner_problem->data !=NULL){
    IOHprofiler_free_memory(inner_problem->data);
    IOHprofiler_free_memory(inner_problem);
  }
}
static IOHprofiler_problem_t *raw_problem_allocate(const size_t number_of_variables,int * parameters,int number_of_layer) {
    char*name;
    name=get_name_problem2(id_problem,parameters);
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars(name,
                                                                               f_evaluate, NULL, number_of_variables, 0, 1, 1);
    IOHprofiler_problem_set_id(problem, "%s_d%02lu", name, number_of_variables);
/*    problem->data=IOHprofiler_duplicate_int_vector(parameters,number_of_layer);*/
    free(name);
    /* Compute best solution */
    f_evaluate(problem, problem->best_parameter, problem->best_value);

    return problem;
}


static void global_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  WModel_data_t *data;
  IOHprofiler_problem_t *inner_problem;
  int temp_dim,new_dim;
  int * tempX;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (WModel_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  temp_dim=data->old_dim;

  tempX=IOHprofiler_duplicate_int_vector(x,temp_dim);

/*for(i=0;i<temp_dim;i++){fprintf(stderr,"%d ",tempX[i]);}fprintf(stderr,"\n");*/
  if(data->parameters[Dummy]>0){
    temp_dim = temp_dim- (int)(data->parameters[Dummy]);
    for (i = 0; i < temp_dim; ++i) {
      data->data1->reduncted_x[i] = tempX[data->data1->postion_match[i]];
    }
    IOHprofiler_free_memory(tempX);
    tempX=IOHprofiler_duplicate_int_vector(data->data1->reduncted_x,temp_dim);
/*for(i=0;i<temp_dim;i++){fprintf(stderr,"%d ",tempX[i]);}fprintf(stderr,"\n");*/
  }if(data->parameters[Neutrality]>1){
    new_dim = temp_dim/ (data->parameters[Neutrality]);
/*    for (i = 0; i < temp_dim; ++i) {
      data->data2->neutrality_x[i] = layer_neutrality_compute(tempX,i*(data->data2->offset[0]), data->data2->offset[0]);
    }*/
    layer_neutrality_compute(tempX, data->data2->neutrality_x, data->data2->offset[0],temp_dim,new_dim);
    temp_dim=new_dim;
    IOHprofiler_free_memory(tempX);
    tempX=IOHprofiler_duplicate_int_vector(data->data2->neutrality_x,temp_dim);
/*for(i=0;i<temp_dim;i++){fprintf(stderr,"%d ",tempX[i]);}fprintf(stderr,"\n");*/
  }if(data->parameters[Epistasis]>1){
    layer_epistasis_compute(tempX, data->data3->epistasis_x, data->data3->offset[0], temp_dim);
    IOHprofiler_free_memory(tempX);
    tempX=IOHprofiler_duplicate_int_vector(data->data3->epistasis_x,temp_dim);
/*for(i=0;i<temp_dim;i++){fprintf(stderr,"%d ",tempX[i]);}fprintf(stderr,"\n");*/
  }
  /*f_evaluate(inner_problem, tempX, y);*/
  IOHprofiler_evaluate_function(inner_problem, tempX, y);

  if(data->parameters[Ruggedness]>0){
    y[0] = layer_compute_ruggedness(y[0],temp_dim,data->parameters[Ruggedness]);
  }
  /*for(i=0;i<10;i++){fprintf(stderr,"%d ",x[i]);}fprintf(stderr,"   -> %f\n",y[0]);*/
  assert(y[0] <= problem->best_value[0]);
  IOHprofiler_free_memory(tempX);
  problem->raw_fitness[0] = y[0];

}

static void global_free(void *thing) {
  WModel_data_t *data = (WModel_data_t *) thing;
  IOHprofiler_free_memory(data->parameters);
  if(data->data1!=NULL){dummy_random_free(data->data1);IOHprofiler_free_memory(data->data1);}
  if(data->data2!=NULL){neutrality_free(data->data2);IOHprofiler_free_memory(data->data2);}
  if(data->data3!=NULL){epistasis_free(data->data3);IOHprofiler_free_memory(data->data3);}
  if(data->data4!=NULL){IOHprofiler_free_memory(data->data4);}
}

static IOHprofiler_problem_t *global_transform(IOHprofiler_problem_t *inner_problem, const int *parameters,int old_dim,int number_of_layer) {
  size_t new_dim,temp_dim;
  int i;
  IOHprofiler_problem_t *problem;
  WModel_data_t *Data;
  dummy_random_data_t *data1;
  neutrality_data_t *data2;
  epistasis_data_t *data3;
  ruggedness_data_t *data4;

  Data = (WModel_data_t *) IOHprofiler_allocate_memory(sizeof(*Data));
  Data->parameters=IOHprofiler_duplicate_int_vector(parameters,number_of_layer);
  Data->number_of_layer=number_of_layer;
  Data->old_dim=old_dim;
  new_dim=old_dim;
  Data->data1=NULL;
  Data->data2=NULL;
  Data->data3=NULL;
  Data->data4=NULL;

  if(*(parameters+Dummy)>0){
    temp_dim = new_dim- *(parameters+Dummy);
    assert(temp_dim > 0);
    data1 = (dummy_random_data_t *) IOHprofiler_allocate_memory(sizeof(*data1));
    data1->offset = IOHprofiler_duplicate_int_vector((parameters+Dummy), 1);
    data1->reduncted_x = IOHprofiler_allocate_int_vector(temp_dim);
    data1->postion_match = IOHprofiler_allocate_int_vector(temp_dim);
    compute_dummy_random_match(data1->postion_match,new_dim,temp_dim);
    Data->data1=data1;
    new_dim=temp_dim;

  }if(*(parameters+Neutrality)>1){
    temp_dim = new_dim/ *(parameters+Neutrality);
    assert(temp_dim > 0);
    data2 = (neutrality_data_t *) IOHprofiler_allocate_memory(sizeof(*data2));
    data2->offset = IOHprofiler_duplicate_int_vector(parameters+Neutrality, 1);
    data2->neutrality_x = IOHprofiler_allocate_int_vector(temp_dim);
    Data->data2=data2;
    new_dim=temp_dim;

  }if(*(parameters+Epistasis)>1){
    data3 = (epistasis_data_t *) IOHprofiler_allocate_memory(sizeof(*data3));
    data3->offset = IOHprofiler_duplicate_int_vector(parameters+Epistasis, 1);
    data3->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
    Data->data3=data3;

  }if(*(parameters+Ruggedness)!=0){
    data4 = (ruggedness_data_t *) IOHprofiler_allocate_memory(sizeof(*data4));
    Data->data4=data4;
  }

  problem = IOHprofiler_problem_transformed_allocate(inner_problem,(void *) Data, global_free, "WModel");

  problem->number_of_variables = new_dim;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = global_evaluate;

/*fprintf(stderr,"para %d %d %d %d \n",*(parameters+Dummy),*(parameters+Neutrality),*(parameters+Epistasis),*(parameters+Ruggedness));*/
  f_evaluate(inner_problem,inner_problem->best_parameter, (problem->best_value));
  inner_problem->best_observed_fvalue[0]=DBL_MIN_EXP;
  inner_problem->best_observed_evaluation[0] = 0;
/*  fprintf(stderr,"best %f\n",*(problem->best_value));*/
  return problem;
}
/**
 * @brief Creates the IOHprofiler one_max_neutrality problem.
 */
static IOHprofiler_problem_t *f_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template,int * parameters,int number_of_layer) {

    size_t *z, *sigma;
    int temp,t,*new_dim;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem,*inner_problem;
    inner_problem = raw_problem_allocate(dimension,parameters,number_of_layer);
/*fprintf(stderr,"%d  ",parameters[1]);
fprintf(stderr,"%d  ",parameters[2]);
fprintf(stderr,"%d  \n",parameters[3]);*/
    problem = global_transform(inner_problem,parameters,dimension,number_of_layer);
    /*
    if(instance == 1){
       
    }
    else if(instance > 1 && instance <= 50){
        z = IOHprofiler_allocate_int_vector(problem->number_of_variables+1);

        IOHprofiler_compute_xopt(z,rseed,problem->number_of_variables);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {
        sigma = IOHprofiler_allocate_int_vector(problem->number_of_variables+1);
        xins = IOHprofiler_allocate_vector(problem->number_of_variables+1);
        
        IOHprofiler_compute_xopt_double(xins,rseed,problem->number_of_variables);
        for(i = 0; i < problem->number_of_variables; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < new_dim; i++){
            t = (int)(xins[i] * (double)problem->number_of_variables);
            assert(t >= 0 && t < problem->number_of_variables);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp; 
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        assert(a <= 5.0 && a >= 0.2);

        problem = transform_vars_sigma(problem,sigma, 0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);

        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    }*/
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");
    IOHprofiler_free_memory(problem_id_template);
    IOHprofiler_free_memory(parameters);


    return problem;
}
#endif
