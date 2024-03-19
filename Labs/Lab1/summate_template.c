#include <stdio.h>
#include <string.h>
double compute_sum(char *input_data)
{
  int header_end, header, pos_data;
  double sum;
  header_end = 0;
  // increments until the end of the header is found, so we know how many values there are
  while (input_data[header_end] != 'e')
  {
    header_end++;
  }
  header = 0;
  sum = 0;
  // data starts 1 ahead of the header to account for the 'e' charachter
  pos_data = header_end + 1;
  // This loop runs for however many values are found to be in the header
  while (header < header_end)
  {
    // 4 cases that all do basically the same thing but for their respective types
    if (input_data[header] == 'b')
    {
      // creates a char instance
      char b_val;
      // casts a pointer to the current position in the input data, according to the
      // data type indicated by the header
      b_val = *(char *)(&input_data[pos_data]);
      // the val is added to the sum
      sum += b_val;
      // the position in data is incremented according to the data type
      pos_data = pos_data + 1;
    }
    if (input_data[header] == 'h')
    {
      // creates a short instance
      short h_val;
      // casts a pointer to the current position in the input data, according to the
      // data type indicated by the header
      h_val = *(short *)(&input_data[pos_data]);
      // the val is added to the sum
      sum += h_val;
      // the position in data is incremented according to the data type
      pos_data = pos_data + 2;
    }
    if (input_data[header] == 'i')
    {
      // creates a integer instance
      int i_val;
      // casts a pointer to the current position in the input data, according to the
      // data type indicated by the header
      i_val = *(int *)(&input_data[pos_data]);
      // the val is added to the sum
      sum += i_val;
      // the position in data is incremented according to the data type
      pos_data = pos_data + 4;
    }
    if (input_data[header] == 'd')
    {
      // creates a double instance
      double d_val;
      // casts a pointer to the current position in the input data, according to the
      // data type indicated by the header
      d_val = *(double *)(&input_data[pos_data]);
      // the val is added to the sum
      sum += d_val;
      // the position in data is incremented according to the data type
      pos_data = pos_data + 8;
    }
    // once all cases are checked, the header increments to the next entry
    header++;
  }
  return sum;
}
int main(int argc, char *argv[])
{
  FILE *inf;            // file pointer for input file
  char input_data[100]; // input data, length defined by problem
  double sum;           // variable which represents the sum of the numbers
  // Make sure filename has been supplied
  if (argc < 2)
  {
    fprintf(stderr, "You need to enter filename to be processed.\n");
    return -1;
  }
  // Open input file
  inf = fopen(argv[1], "rb");
  if (inf == NULL)
  {
    fprintf(stderr, "Error opening file %s\n", argv[1]);
    return -1;
  }
  // Read in input data
  if (fread(&input_data, 1, 100, inf) < 1)
  {
    fprintf(stderr, "Error reading.\n");
    return -1;
  }
  sum = compute_sum(input_data);
  printf("Sum is: %f\n", sum);
  return 0;
}