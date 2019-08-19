enum WModel_Layers {dummy=1, neutrality=2, epistasis=3, ruggedness=4};

/* need somehow a way to free problem or already done ?*/
/*void free_WModel_function(size_t f);*/

static IOHprofiler_problem_t *f_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template);
