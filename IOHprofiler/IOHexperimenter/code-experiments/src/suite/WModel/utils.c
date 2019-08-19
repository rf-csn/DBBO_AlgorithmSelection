#ifndef UTILS
#define UTILS

#define MAX_PARAMETER 1000
#define MAX_CHAR 5
#include "../../profiler/IOHprofiler_string.c"
#include "../../profiler/IOHprofiler.h"

static int layer_number;
/*static char **layer_para;*/
static char * dimensions;

#define BUFF 50
/*
 * this files contains basic functions about parameters for the layer
*/
static int layer_para[4] ={0,0,0,0};

int * copy_and_realloc(const int *src, const size_t number_of_elements,int new_size);
/*old*/
/*
void init_parameters(int number,char**lp){
  layer_number=number;
  layer_parameters=lp;

}*/
/*old*/
int number_of_op(size_t * layer){
  int i=1,sum=1;
  if (layer==NULL){return 0;}
  while(*(layer+i)!=0){
    sum++;i++;
  }
  return sum;
}
void pr_para(int * p){
fprintf(stderr,"%d  ",p[0]);
fprintf(stderr,"%d  ",p[1]);
fprintf(stderr,"%d  ",p[2]);
fprintf(stderr,"%d \n",p[3]);
}
int compute_problem_dimension(int dim,int du,int ne){
  int output_dim =dim;
  if(du>dim){return 0;}
  output_dim = (int)(output_dim- (int)(du));
  if(ne>0 && ne<output_dim){
    output_dim=(int)(output_dim/ (ne));
  }else if(ne<0 || ne>=output_dim){ return 0;}
/*  if(output_dim<4){fprintf(stderr,"\n dim %d du %d ne %d dim %d\n",dim,du,ne,output_dim);}*/
  return output_dim;
}
int correct_epistasis(int dim,int * ep){
  int sum;
  sum=0;
  int i=0;
  do{
    if (ep[i] !=0 && ep[i]<dim){sum++;}
    i++;
  }while(ep[i]!=0);
  return sum;
}
int correct_ruggedness(int dim,int * ru){
  int sum;
  sum=0;
  int i=0;
  do{
    if (ru[i]<(dim*(dim-1))/2){sum++;}
    i++;
  }while(ru[i]!=0);
  return sum;
}
float dim_proportion(int dim,int dum,int neutr){
  if(neutr==0){return ( 100-dum);}

/*  fprintf(stderr,"%d %d %f  res %d\n",dum,neutr,(100.-dum)/neutr,(100.-dum)/neutr>=5);*/
  return  (100.-dum)/neutr;
}
int is_correct_problem(const int dim, const int dum,const int neutr){
/*  if (dim_proportion(dim,dum,neutr)>=(float) 1/dim){return 1;}*/
  if (dim_proportion(dim,dum,neutr)>=5){return 1;}
  
  else{return 0;}
}
int valid(int dim, int du, int ne, int ep,int ru){
/*  if(ep==5){fprintf(stderr,"ep %d val %d \n",ep,!(ep !=0 && ep<dim));}*/
  if(!(ru<=(dim*(dim-1))/2)){return 0;}
  if(!(ep !=0 && ep<dim)){return 0;}
  return is_correct_problem(dim, du, ne);
}
int nb_elt(int *l){
  int s=0;
  do{s++;
  }while(l[s]!=0);
  return s;
}
int get_real_value(int dim,int min,int_to_int_func f,int ind){
  int max=f(dim);
  int res= (min+((int) (ind*(max-min)/100)));
  return res;
}
typedef struct  WModel WModel;
/*Count total number of problem based on the number of layer*their range*/
int compute_number_of_functions(WModel *wm,int dimension){
    int dim=dimension;
    int i;
    int * dummy=IOHprofiler_allocate_int_vector(nb_elt(wm->layer1));
    for(i=0;i<nb_elt(wm->layer1);i++){dummy[i]=get_real_value(dimension,wm->min1,wm->l_max1,wm->layer1[i]);}
    int * neutr=IOHprofiler_allocate_int_vector(nb_elt(wm->layer2));
    for(i=0;i<nb_elt(wm->layer2);i++){neutr[i]=get_real_value(dimension,wm->min2,wm->l_max2,wm->layer2[i]);}
    int * epis=IOHprofiler_allocate_int_vector(nb_elt(wm->layer3));
    for(i=0;i<nb_elt(wm->layer3);i++){epis[i]=get_real_value(dimension,wm->min3,wm->l_max3,wm->layer3[i]);}
    int * rugg=IOHprofiler_allocate_int_vector(nb_elt(wm->layer4));
    for(i=0;i<nb_elt(wm->layer4);i++){rugg[i]=get_real_value(dimension,wm->min4,wm->l_max4,wm->layer4[i]);}
    int b,c,d,e,sum,temp;
    sum=0;
    c=0;
    b=0;
    temp=0;
    d=nb_elt(wm->layer3);
    e=nb_elt(wm->layer4);
    do{c=0;
      do{
        temp+=is_correct_problem(dim,wm->layer1[b],wm->layer2[c]);
        c++;
      }while(c <nb_elt(wm->layer2));
      b++;
    }while(b<nb_elt(wm->layer1));
    sum+=temp*d*e;
/*    fprintf(stderr,"\n-------- dim %d sum %d ---------\n",dim,sum);*/
/*
for(i=0;i<nb_elt(wm->layer1);i++){fprintf(stderr," %d ",dummy[i]);}
fprintf(stderr,"\n");
for(i=0;i<nb_elt(wm->layer2);i++){fprintf(stderr," %d ",neutr[i]);}
fprintf(stderr,"\n");
for(i=0;i<nb_elt(wm->layer3);i++){fprintf(stderr," %d ",epis[i]);}
fprintf(stderr,"\n");
for(i=0;i<nb_elt(wm->layer4);i++){fprintf(stderr," %d ",rugg[i]);}
fprintf(stderr,"\nsum %d\n",sum);*/
    IOHprofiler_free_memory(dummy);
    IOHprofiler_free_memory(neutr);
    IOHprofiler_free_memory(epis);
    IOHprofiler_free_memory(rugg);
    return sum;
}



int * get_WModel_parameters(WModel* wm){

  int *res=IOHprofiler_allocate_int_vector(wm->number_of_layers);
  res[0]=wm->layer1[wm->counter[0]];
  res[1]=wm->layer2[wm->counter[1]];
  res[2]=wm->layer3[wm->counter[2]];
  if(wm->counter[3] >=0){
    res[3]=wm->layer4[wm->counter[3]];
  }else {res[3]=0;}
  return res;
}
int * get_real_parameters(int dim,WModel * wm){
  int * p=get_WModel_parameters(wm);
  int *res=IOHprofiler_allocate_int_vector(wm->number_of_layers);
  res[0]=get_real_value(dim,wm->min1,wm->l_max1,p[0]);
  res[1]=get_real_value(dim,wm->min2,wm->l_max2,p[1]);
  res[2]=get_real_value(dim,wm->min3,wm->l_max3,p[2]);
  res[3]=get_real_value(dim,wm->min4,wm->l_max4,p[3]);
  IOHprofiler_free_memory(p);
  return res;
}
static int * get_layers_parameters(void);
void increment_layers_param(WModel * wm,int dim){
int * temp1,temp2;
int x=0;


/*-----------------------------------------------*/
  if (wm->counter[3]<nb_elt(wm->layer4)-1){
    wm->counter[3]++;
  }else if(wm->counter[3]==nb_elt(wm->layer4)-1){
    wm->counter[3]=0;
    if (wm->counter[2]<nb_elt(wm->layer3)-1){
      wm->counter[2]++;
    }else if(wm->counter[2]==nb_elt(wm->layer3)-1){
     wm->counter[2]=0;
     if (wm->counter[1]<nb_elt(wm->layer2)-1){
       wm->counter[1]++;
      }else if(wm->counter[1]==nb_elt(wm->layer2)-1){
        wm->counter[1]=0;
        if (wm->counter[0]<nb_elt(wm->layer1)-1){
          wm->counter[0]++;
        }else if(wm->counter[0]==nb_elt(wm->layer1)-1){
          wm->counter[0]=0;
        }
      }
    }
  }
  int * r=get_real_parameters(dim,wm);
  int * r2=get_WModel_parameters(wm);
  if(!valid(dim, r2[0],r2[1],r[2],r[3]))
    {  IOHprofiler_free_memory(r);IOHprofiler_free_memory(r2);
       increment_layers_param(wm, dim);}
  else{IOHprofiler_free_memory(r);IOHprofiler_free_memory(r2);}
  r=get_WModel_parameters(wm);
  int i;
  for(i=0;i<4;i++){layer_para[i]=r[i];}
  IOHprofiler_free_memory(r);

}



char* get_name_problem(int id_problem, struct WModel * wm){
  int * para;
  char *name;
  para=get_WModel_parameters(wm);
  name=malloc(sizeof(char)*100);
    
/*  if(id_problem==1){
/*    sprintf(name, "one_max_dummy=%d_mu=%d_nu=%d_gamma=%d",layer_para[0],layer_para[1],layer_para[2],layer_para[3]);*/
    sprintf(name, "one_max_D%d_N%d_E%d_R%d",para[0],para[1],para[2],para[3]);
/*  }*/
  IOHprofiler_free_memory(para);
  return name;
}
char* get_name_problem2(int id_problem, int * para){
  char *name;
  name=malloc(sizeof(char)*100);
    
/*  if(id_problem==1){
/*    sprintf(name, "one_max_dummy=%d_mu=%d_nu=%d_gamma=%d",layer_para[0],layer_para[1],layer_para[2],layer_para[3]);*/
    sprintf(name, "one_max_D%d_N%d_E%d_R%d",para[0],para[1],para[2],para[3]);
/*  }*/
  return name;
}
/*old*//*
void incr(int ind){
  if(ind>0){
  size_t * p;
  p=IOHprofiler_string_parse_ranges(layer_parameters[layer_number-ind], 0, 0, "layer_parameter", MAX_PARAMETER);
  if (number_of_op(p)> (layer_para[layer_number-ind]+1)){
    layer_para[layer_number-ind]++;
  }else if(number_of_op(p)==(layer_para[layer_number-ind]+1)){
    layer_para[layer_number-ind]=0;
    incr(ind-1);
  }
  IOHprofiler_free_memory(p);
  }
}*/
/*old*//*
void incr_layer_para(void){
  int i;
  i=layer_number;
  incr(i);
}*/

static int * get_layers_parameters(void){
  int n_l=4;
  int * layer_values=IOHprofiler_allocate_int_vector(n_l);
  int i;

  for (i=0;i<n_l;i++){layer_values[i]=layer_para[i];}
  return layer_values;
}
/*
calcule la dimension du problem après les features du W-Model
casse un peu la modularité
*/

/*old*/

int compute_problem_get_dimension(int dim){
  int *p;
  p=get_layers_parameters();
  int output_dim =dim+1;
  if(p[0]>dim){return 0;}
  output_dim = output_dim- (int)(p[0]);
  if(p[1]>0 && p[1]<output_dim){
    output_dim=output_dim/ (p[1]);
/*  printf(".%d  %d %d\n",output_dim,p[1],output_dim/ (p[1]));*/
  }else if(p[1]<0 || p[1]>=output_dim){ return 0;}
  else if(p[2]>output_dim){return 0;}
  else if(p[3]>((output_dim-1)*(output_dim)/2)){return 0;}

/*  printf("%d  %d %d\n",p[0],p[1],output_dim);*/
  return output_dim;
}
/*old*/
/*Return a string of the layers parameters "k=a_mu=b_nu=c_gamma=d"*/
void get_name(int id_problem,char *name){
  int * para;
  para=get_layers_parameters();
  if(id_problem==1){
/*    sprintf(name, "one_max_dummy=%d_mu=%d_nu=%d_gamma=%d",layer_para[0],layer_para[1],layer_para[2],layer_para[3]);*/
    sprintf(name, "one_max_D%d_N%d_E%d_R%d",para[0],para[1],para[2],para[3]);
  }
  IOHprofiler_free_memory(para);
}

/*
 my own parser because at least it will work
*/
char * extract_number(char *l){
  char * res= IOHprofiler_allocate_memory(BUFF * sizeof(char *));
  if (strlen(l)==0 || l == NULL){
    res[0]=0;
  return res;
  }
  int i =0;
  while(isdigit(l[i])){
    res[i]=l[i];
    i+=1;
  }
  res[i]='\0';
  return res;
}
int* parse_range_layer(char * l){
  int i,j;
  int current_free_space=BUFF;
  int realloc=1;
  int nb_elt=0;
  int a,b,c;
  int * res= IOHprofiler_allocate_int_vector(BUFF);
  i=0;
  char * int_temp1;
  char * int_temp2;
  if (strlen(l)==0 || l == NULL){
    res[0]=0;
  return res;
  }
  while(i<=strlen(l) && l[i]!='\0'){
    if(isdigit(l[i])){
      int_temp1=extract_number(l+i);
      i+=strlen(int_temp1);
      if(l[i]=='\0'){
        if (current_free_space<=1){
         realloc+=1;
         res=copy_and_realloc(res,nb_elt,realloc*BUFF);
         current_free_space+=BUFF;
        }
        current_free_space-=1;
        res[nb_elt] = atoi((char*) int_temp1);
        nb_elt+=1;
        IOHprofiler_free_memory(int_temp1);
        i+=2;
      }else if(l[i]==','){
        if (current_free_space<=1){
         realloc+=1;
         res=copy_and_realloc(res,nb_elt,realloc*BUFF);
         current_free_space+=BUFF;
        }
        current_free_space-=1;
        res[nb_elt] = atoi((char*) int_temp1);
        nb_elt+=1;
        IOHprofiler_free_memory(int_temp1);
        i+=1;
      }else if(l[i]=='-'){
        i+=1;
        int_temp2=extract_number(l+i);
        i+=strlen(int_temp2);
        a=atoi(int_temp1);
        b=atoi(int_temp2);
        IOHprofiler_free_memory(int_temp1);
        IOHprofiler_free_memory(int_temp2);
        if(b<=a){IOHprofiler_error("parse layers wrong parameter1");}
        c=b-a;
        while (current_free_space<=c+1){
         realloc+=1;
         res=copy_and_realloc(res,nb_elt,realloc*BUFF);
         current_free_space+=BUFF;
        }
        for(j=0;j<=c;j++){
          res[nb_elt] = a+j;
          nb_elt+=1;
          current_free_space-=1;
        }if(l[i]!=',' && l[i]!='\0'){IOHprofiler_error("parse layers wrong parameter1");}
        i+=1;
      }
    }else{IOHprofiler_error("parse layers wrong parameter2");}
  }
  res[nb_elt]=0;
/*  for(i=0;i<=nb_elt;i++){
  fprintf(stderr,"\%d  ",res[i]);
} fprintf(stderr,"\n");*/
  return res;
}
int * copy_and_realloc(const int *src, const size_t number_of_elements,int new_size) {
    size_t i;
    int *dst;

    assert(src != NULL);
    assert(new_size > 0);

    dst = IOHprofiler_allocate_int_vector(new_size);
    for (i = 0; i < number_of_elements; ++i) {
        dst[i] = src[i];
    }IOHprofiler_free_memory(src);
    return dst;
}

#endif
