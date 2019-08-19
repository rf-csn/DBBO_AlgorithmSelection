/**
 * @file transform_obj_ruggedness1.c
 * @brief Implementation of ruggedness1ing the objective value by the given offset.
 */
#ifndef RUGGEDNESS
#define RUGGEDNESS

#include <assert.h>
#include <math.h>
#include "../profiler/IOHprofiler.h"
#include "../profiler/IOHprofiler_problem.c"
#include "../suite/WModel/WModel_struct.h"


int max_gamma(int q) {
    return (int)((q * (q - 1))>>1);
}

int * ruggedness_raw(int gamma, int q){
   int i,j,k,start,max;
   int upper;
   int t;
   int * r;
/*fprintf(stderr,"gamma : %d\n",gamma);
fprintf(stderr,"q : %d\n",q);*/
   r=IOHprofiler_allocate_int_vector(q+1);
   r[0]=0;
   max = max_gamma(q);
    if (gamma <= 0){
      start = 0;
    } else {
      start = q - 1 - (int) (0.5 + sqrt(0.25 + ((max - gamma)<<1)));
    }
/*
fprintf(stderr,"part1 : %d\n",((int)(0.25 + ((max - gamma)<<1))));
fprintf(stderr,"part2 : %f\n",(sqrt((int)(0.25 + ((max - gamma)<<1)))));
fprintf(stderr,"part3 : %d\n",(int)(0.5 + sqrt((int)(0.25 + ((max - gamma)<<1)))));*/
/*fprintf(stderr,"start %d\n",start);*/

/*for(i=0;i<q;i++){fprintf(stderr,"%d.",r[i]);}fprintf(stderr,"\n");*/
    k = 0;
    for (j = 1; j <= start; j++) {
      if ((j & 1) != 0){ r[j] = (q - k);}
      else{k=k+1; r[j] = k;}
    }
/*for(i=0;i<q;i++){fprintf(stderr,"%d.",r[i]);}fprintf(stderr,"\n");*/
    for (; j <= q; j++) {
      k=k+1;
      if((start & 1) != 0){r[j] = (q - k);}
      else{r[j]=k;}
    }
    upper = ((gamma - max) +(((q - start - 1) * (q - start)) >> 1));
    j--;
/*for(i=0;i<q;i++){fprintf(stderr,"%d.",r[i]);}fprintf(stderr,"\n");*/
/*fprintf(stderr,"upper %d\n",upper);*/
    for (i = 1; i <= upper; i++) {
      j=j-1;

      if(j>0){
      t = r[j];
      r[j] = r[q];
      r[q] = t;}
    }

/*fprintf(stderr,"r\n");
for(i=0;i<=q;i++){fprintf(stderr,"%d %d\n",i, r[i]);}*/
int * r2;
   r2=malloc(sizeof(int)*(q+1));
 for(i=0;i<=q;i++){r2[i]=q-r[q-i];}
 free(r);
/*fprintf(stderr,"r2\n");
for(i=0;i<=q;i++){fprintf(stderr,"%d %d\n",i, r2[i]);}*/
    return r2;
  }

int ruggedness_translate(int gamma, int q) {
    int j, k, max, g, lastUpper;

    if (gamma <= 0) {
      return 0;
    }
    g = gamma;
    max = max_gamma(q);
    lastUpper = ((q >> 1) * ((q + 1) >> 1));
    if (g <= lastUpper) {
      j = abs(((q + 2) * 0.5)-sqrt((((q * q) * 0.25) + 1) - g));

      k = ((g - ((q + 2) * j)) + (j * j) + q);
      return ((k + 1 + ((((q + 2) * j) - (j * j) - q - 1) << 1))- (j - 1));
    }

    j = abs((((q % 2) + 1) * 0.5)
        + sqrt((((1 - (q % 2)) * 0.25) + g) - 1 - lastUpper));

    k = g - (((j - (q % 2)) * (j - 1)) + 1 + lastUpper);

    return (max - k - ((2 * j * j) - j) - ((q % 2) * ((-2 * j) + 1)));
  }



double layer_compute_ruggedness(const double y, size_t dimension,int gamma){
  double result;
  int * r,i;
  r=ruggedness_raw(ruggedness_translate(gamma, dimension), dimension);
  result=r[ (int)y ];

/*for(i=0;i<dimension+1;i++){fprintf(stderr,"%d ",r[i]);}fprintf(stderr,"\n");

fprintf(stderr,"dim %d val %f, result %f\n",dimension,y,result);*/
  assert(result <= (double) dimension);
  IOHprofiler_free_memory(r);
  return result;
}

/**
 * @brief Evaluates the transformation.
 */
static void ruggedness_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y,int gamma) {
  ruggedness_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
  	IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
  	return;
  }

  data = (ruggedness_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = layer_compute_ruggedness(y[i],problem->number_of_variables,gamma);
      problem->raw_fitness[i] = y[i];
      
  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_ruggedness(IOHprofiler_problem_t *inner_problem,const int *offset, double *y) {
  /*if(*offset==1){return inner_problem;}*/
  if(*offset>=0){return inner_problem;}
  IOHprofiler_problem_t *problem;
  ruggedness_data_t *data;
  size_t i;
  data = (ruggedness_data_t *) IOHprofiler_allocate_memory(sizeof(*data));

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "ruggedness");
  problem->evaluate_function = ruggedness_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = layer_compute_ruggedness(problem->best_value[i],problem->number_of_variables, &offset); 
  }
  return problem;
}
#endif
