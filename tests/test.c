#include <stdio.h>
#include <math.h>

struct mike_str {
    char x;
    int y;
    double z;
};

int main() {
    FILE  *fp;
    unsigned char mkdata[64] = {0};
    short *sptr;
    int *iptr;
    long *lptr;
    long long *llptr;
    float *fptr;
    double *dptr;
    struct mike_str *m_sptr;

    sptr = (short *)(mkdata);
    *(sptr) = 0xABCD;
 
    iptr = (int *)(mkdata);
    *(iptr+1) = 0x30004000;

    dptr = (double *)(mkdata);
    *(dptr+1) = 2*asin(1);

 /*    fptr = (float *)(mkdata);
    *(fptr+4) = 2*asin(1); */

/*     lptr = (long *)(mkdata);
    // *(lptr+3) =  13;
    *(lptr+3) =  -1; */

/*     *(lptr + 4) = (long)(mkdata);
    *(lptr + 5) = (long)(fptr+4); */

/*     m_sptr = (struct mike_str *)(mkdata);
    (m_sptr+3)->x = 7;
    (m_sptr+3)->y = -1;
    (m_sptr+3)->z = 2*asin(1); */

    fp = fopen("test.txt","w+");
    unsigned int i;
    for(i=0; i<64; i++) fprintf(fp, "%c",*(mkdata+i));
    fclose(fp);
    return 0;
}