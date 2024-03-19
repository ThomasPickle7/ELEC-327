#include <stdio.h>
#include <math.h>


int main() {
    float  XX;
    int aa;
    double FF;
    unsigned char x,y,w,z;
    unsigned char data[32] = {0};
    unsigned char *cptr1,*cptr2;

   
    x=3; 
    y=4;
    w=5;
    z=6;
    FF = 2*asin(1);
    aa = 13 + 12*1024*64;
    XX = 2*asin(1);
    data[0]= 12;
    data[15]=20;
    data[20]=5;
    unsigned char mkdata[64] = {0};
    int *ptr;
    float *fptr;
    double *dptr;

    cptr1 = (unsigned char *)(&cptr2);
    cptr2 = (unsigned char *)(data);
    *dptr = 3.14;
    *dptr = 2*asin(1);
    printf("%d", w);
    return 0;
}