#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define defaultOutputFileName "out.txt"

void heapsort  (int *A, int heapsize);
void buildHeap (int *A, int heapsize);
void heapify   (int *A, int i, int heapsize);
void exchange  (int *A, int a, int b);

int main (int argc, char **argv) {
  FILE *inputFile;
  FILE *outputFile;

  char *inputFileName;
  char *outputFileName;

  int c, inputFileFlag = -1, outputFileFlag = -1;

  //getopt setting i stands for input, o stands for output
  while ((c = getopt(argc, argv, "io")) != -1)
    switch (c) {
      case 'i': //allocs memory and sets input filename (or path)
        inputFileName = malloc((strlen(argv[optind])+1)*sizeof(char));
        strncpy(inputFileName, argv[optind], strlen(argv[optind]));
        inputFileFlag=0;
        break;
      case 'o': //allocs memory and sets output filename (or path)
        outputFileName = malloc((strlen(argv[optind])+1)*sizeof(char));
        strncpy(outputFileName, argv[optind], strlen(argv[optind]));
        outputFileFlag=0;
        break;
      case '?':
        fprintf(stderr, "Syntax: heapsort -i inputFile -o outputFile\n");
        return EXIT_FAILURE;
      default:
        break;
    }

  // inputFileFlag has to be set, to run the program
  if (inputFileFlag) {
    fprintf(stderr, "Syntax: heapsort -i inputFile -o outputFile\n");
    return EXIT_FAILURE;
  }

  // if the outputFileFlag is not set, then the defaultOutputFileName will become outputFileName
  if (outputFileFlag) {
    outputFileName = malloc((strlen(defaultOutputFileName)+1)*sizeof(char));
    strncpy(outputFileName, defaultOutputFileName, strlen(defaultOutputFileName));
  }

  // opening files to read and write
  inputFile  = fopen((const char *) inputFileName, "r");
  outputFile = fopen((const char *) outputFileName, "w");

  


  fclose(inputFile);
  fclose(outputFile);

  free(inputFileName);
  free(outputFileName);
  return 0;
}

void heapsort (int *A, int heapsize) {
    int i;
    buildHeap(A, heapsize);
    for (i=heapsize; i>0; i--)
    {
      exchange(A, 0, i);
      heapsize--;
      heapify(A, 0, heapsize);
    }
}

void buildHeap (int *A, int heapsize) {
    for (int i=heapsize/2; i>=0; i--)
      heapify(A, i, heapsize);
}

void heapify (int *A, int i, int heapsize) {
  int largest;

  int le = (2*i);
  int ri = (2*i)+1;

  if (le <= heapsize && A[le] > A[i])
    largest = le;
  else
    largest = i;

  if (ri <= heapsize && A[ri] > A[largest])
    largest = ri;

  if (largest != i) {
    exchange(A, i, largest);
    heapify(A, largest, heapsize);
  }
}

void exchange (int *A, int a, int b)
{
  if (*(A+a) != *(A+b)) {
    *(A+a) ^= *(A+b);
    *(A+b) ^= *(A+a);
    *(A+a) ^= *(A+b);
  }
}
