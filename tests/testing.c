#include <stdio.h>
#include <math.h>

int main (){
    int x = 8;
    char y = (unsigned char *) (&x);
    printf("%c", y);
    return 0;
}