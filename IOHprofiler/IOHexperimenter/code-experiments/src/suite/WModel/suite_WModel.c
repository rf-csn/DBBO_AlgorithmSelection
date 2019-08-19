/**
 * @file suite_IOHprofiler.c
 * @brief Implementation of the IOHprofiler suite for W_Model
 */
#ifndef SUITE_WMODEL
#define SUITE_WMODEL
#include "../../profiler/IOHprofiler.h"

#include "../../objective_function/modular_problem.c"

#include "utils.c"
#include "WModel_struct.h"

void WModel_struct_free(struct WModel * wm){
  if(wm!=NULL){
  IOHprofiler_free_memory(wm->layer1);
  IOHprofiler_free_memory(wm->layer2);
  IOHprofiler_free_memory(wm->layer3);
  IOHprofiler_free_memory(wm->layer4);
  IOHprofiler_free_memory(wm->counter);
  
  }
}

/**
* filter layers for the WModel
*
*
static void IOHprofiler_suite_filter_layers(size_t *layer, const size_t *layer_numbers) {
    size_t i, j;
    size_t count = IOHprofiler_count_numbers(layer_numbers, IOHprofiler_MAX_INSTANCES, "layers_WModel");
    int found;

    for (i = 0; i < suite->number_of_dimensions; i++) {
        found = 0;
        for (j = 0; j < count; j++) {
            if (layer[i] == dimension_numbers[j])
                found = 1;
        }
        if (!found)
            layer[i] = 0;
    }
}*/
static IOHprofiler_suite_t *IOHprofiler_suite_allocate(const char *suite_name,
                                                       const size_t number_of_functions,
                                                       const size_t number_of_dimensions,
                                                       const size_t *dimensions,
                                                       const char *default_instances);

/**
 * @brief Sets the dimensions and default instances for the IOHprofiler suite.
 */
 int range_to_n(int dim){return dim;}
int range_to_nsquare_div2(int dim){return (dim*(dim-1))/2;}


static IOHprofiler_suite_t *suite_WModel_initialize(int number,char **layer_param,char* dim) {
    /*old*/
/*    init_parameters(number,layer_param);*/
    /*---*/

    IOHprofiler_suite_t *suite;
    size_t i;
    const char *suite_name = "WModel";

    size_t dimension[20000];
    int * temp_dim=parse_range_layer(dim);

    struct WModel * wm = (struct WModel *)IOHprofiler_allocate_memory(sizeof(struct WModel));
    wm->number_of_layers=number;
    wm->layer1=parse_range_layer(layer_param[0]);
    wm->min1=0;
    wm->l_max1=&range_to_n;
    wm->layer2=parse_range_layer(layer_param[1]);
    wm->min2=1;
    wm->l_max2=&range_to_n;
    wm->layer3=parse_range_layer(layer_param[2]);
    wm->min3=1;
    wm->l_max3=&range_to_n;
    wm->layer4=parse_range_layer(layer_param[3]);
    wm->min4=0;
    wm->l_max4=&range_to_nsquare_div2;

    int * counter=IOHprofiler_allocate_int_vector(number);
    for(i=0;i<number;i++){counter[i]=0;}
    counter[number-1]=0;
    wm->counter=counter;

    const size_t number_of_dimension = sizeof(dimension) / sizeof(dimension[0]);
    for (i = 0; i < number_of_dimension; ++i){
        dimension[i] = i + 1;
    }
    const size_t number_of_functions=(size_t) compute_number_of_functions(wm, temp_dim[0]);
    fprintf(stderr,"--------%d------------\n",number_of_functions);
    /* IMPORTANT: Make sure to change the default instance for every new workshop! */
    suite = IOHprofiler_suite_allocate(suite_name, number_of_functions, number_of_dimension, dimension, "year: 2019");
    
    suite->data =wm;
    suite->data_free_function = WModel_struct_free;
    IOHprofiler_free_memory(temp_dim);
    return suite;

}

/**
 * @brief Sets the instances associated with years for the IOHprofiler suite.
 */
static const char *suite_WModel_get_instances_by_year(const int year) {
    if (year == 2018) {
        return "1-100";
    } else {
        IOHprofiler_error("suite_WModel_get_instances_by_year(): year %d not defined for suite_IOHprofiler", year);
        return NULL;
    }
}

/**
 * @brief Creates and returns a IOHprofiler problem without needing the actual IOHprofiler suite.
 *
 * Useful for other suites as well (see for example suite_biobj.c).
 */

static IOHprofiler_problem_t *WModel_get_problem(const size_t function,
                                              const size_t dimension,
                                              const size_t instance,struct WModel* wm) {
    IOHprofiler_problem_t *problem = NULL;
    char *name;
    name =get_name_problem(id_problem,wm);

    const char *problem_id_template;
    problem_id_template=IOHprofiler_allocate_string(strlen(name)+1);
    strcpy(problem_id_template,name);
    free(name);

    const char *problem_name_template = "WModel suite problem f%lu instance %lu in %luD";

    const long rseed = (long)(function + 10000 * instance);

    int * para;
    para=get_real_parameters(dimension,wm);
    problem = f_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                         problem_id_template, problem_name_template,para,wm->number_of_layers);
    if(problem==NULL){
      IOHprofiler_error("get_IOHprofiler_problem(): cannot retrieve problem f%lu instance %lu in %luD",
                          (unsigned long)function, (unsigned long)instance, (unsigned long)dimension);
        return NULL; /* Never reached */
    }
    return problem;
}

/**
 * @brief Returns the problem from the IOHprofiler suite that corresponds to the given parameters.
 *
 * @param suite The IOHprofiler suite.
 * @param function_idx Index of the function (starting from 0).
 * @param dimension_idx Index of the dimension (starting from 0).
 * @param instance_idx Index of the instance (starting from 0).
 * @return The problem that corresponds to the given parameters.
 */
static IOHprofiler_problem_t *suite_WModel_get_problem(IOHprofiler_suite_t *suite,
                                                    const size_t function_idx,
                                                    const size_t dimension_idx,
                                                    const size_t instance_idx) {
    IOHprofiler_problem_t *problem = NULL;

/*delete later
    int *l;
    l=get_layers_parameters();
    fprintf(stderr,"parameter : dummy:%d neutrality:%d epistasis:%d ruggedness:%d\n",*l,*(l+1),*(l+2),*(l+3));
    free(l);
*/


    const size_t function = suite->functions[function_idx];
    const size_t dimension = suite->dimensions[dimension_idx];
    const size_t instance = suite->instances[instance_idx];
    problem = WModel_get_problem(function, dimension, instance,(struct WModel *)suite->data);
   
    problem->suite_dep_function = function;
    problem->suite_dep_instance = instance;
    problem->suite_dep_index = IOHprofiler_suite_encode_problem_index(suite, function_idx, dimension_idx, instance_idx);

    return problem;
}
#endif
