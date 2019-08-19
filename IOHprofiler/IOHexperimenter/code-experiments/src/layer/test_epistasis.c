#include <stdio.h>
#include <stdlib.h>
void base_epistasis(const int * xIn, const int start, const int nu, int 
* xOut) {

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
    int i = 0;
    for (; i <= end; i += nu) {
      base_epistasis(xIn, i, nu, xOut);
    }
    if (i <length) {
      base_epistasis(xIn, i, length - i, xOut);
    }
  }



void main(void){
int  In[10] = {1,1,1,1,0,0,1,1,1,0};
int  Out[10];
int nu=4;
int l=10;
epistasis(In, nu,Out,l);
int i;
for(i=0;i<l;i++){
  printf("%d ",Out[i]);
}

}
