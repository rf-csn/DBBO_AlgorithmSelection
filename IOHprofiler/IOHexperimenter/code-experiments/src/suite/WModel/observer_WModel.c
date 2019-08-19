/**
 * @file observer_bbob.c
 * @brief Implementation of the PBO observer.
 */
#ifndef OBSERVER_WMODEL
#define OBSERVER_WMODEL

#include "../../profiler/IOHprofiler.h"
#include "../../profiler/IOHprofiler_utilities.c"

static IOHprofiler_problem_t *logger_WModel(IOHprofiler_observer_t *observer, IOHprofiler_problem_t *problem);
static void logger_WModel_free(void *logger);

/**
 * @brief The IOHprofiler observer data type.
 */
typedef struct {
    /* TODO: Can be used to store variables that need to be accessible during one run (i.e. for multiple
   * problems). For example, the following global variables from logger_bbob.c could be stored here: */
    size_t current_dim;
    size_t current_fun_id;
    /* ... and others */
} observer_WModel_data_t;


/**
 * @brief Initializes the bbob observer.
 */
static void WModel_observer_IOHprofiler(IOHprofiler_observer_t *observer, const char *options, IOHprofiler_option_keys_t **option_keys) {
    observer->logger_allocate_function = logger_WModel;
    observer->logger_free_function = logger_WModel_free;
    observer->data_free_function = NULL;
    observer->data = NULL;

    *option_keys = NULL;

    (void)options; /* To silence the compiler */
}

#endif
