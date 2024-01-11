#include <stdio.h>


double compute_sum(char *input_data) {

  // Your code goes here!
   
  return 0.0;
}


int main(int argc, char *argv[])
{

  FILE *inf; // file pointer for input file
  char input_data[100]; // input data, length defined by problem
  double sum;  // variable which represents the sum of the numbers


  // Make sure filename has been supplied
  if (argc < 2) {
    fprintf(stderr, "You need to enter filename to be processed.\n");
    return -1;
  }

  // Open input file
  inf = fopen(argv[1],"rb");
  if (inf == NULL) {
    fprintf(stderr, "Error opening file %s\n", argv[1]);
    return -1;
  }

  // Read in input data
  if (fread(&input_data, 1, 100, inf) < 1) {
    fprintf(stderr, "Error reading.\n");
    return -1;
  }

  sum = compute_sum(input_data);

  printf("Sum is: %f\n", sum);

  return 0; 
}

