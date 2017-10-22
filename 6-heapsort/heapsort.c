/* AL2.4, Stanisław Juliusz Grams (251000 UG MFI), 20171022
 * Compile with command `gcc -o heapsort heapsort.c -Wall -pedantic -std=gnu99`
 * Syntax `./heapsort [-l (for iterative heapify function!)] -i inputFile -o outputFile`
 * Input file is required. If output file is not set, defaults will be forced (out.txt).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define defaultOutputFileName "out.txt"

void heapsort  (int *A, int heapsize, int type);
void buildHeap (int *A, int heapsize);
void heapify   (int *A, int i, int heapsize);
void heapifyIt (int *A, int i, int heapsize);
void exchange  (int *A, int a, int b);

int main (int argc, char **argv) {
  FILE *inputFile;
  FILE *outputFile;

  char *inputFileName;
  char *outputFileName;

  int  *numbersTable;
  int  i, c, type=0, inputFileFlag=-1, outputFileFlag=-1, inputFileLineCounter=0;

  //getopt setting i stands for input, o stands for output
  while ((c=getopt(argc, argv, "lio")) != -1)
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
      case 'l': //l stands for loop and runs heapify iteratively
        type=1;
        break;
      case '?':
        fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i inputFile -o outputFile\n");
        return EXIT_FAILURE;
      default:
        break;
    }

  // inputFileFlag has to be set, to run the program
  if (inputFileFlag) {
    fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i inputFile -o outputFile\n");
    return EXIT_FAILURE;
  }

  // if the outputFileFlag is not set, then the defaultOutputFileName will become outputFileName
  if (outputFileFlag) {
    outputFileName = malloc((strlen(defaultOutputFileName)+1)*sizeof(char));
    strncpy(outputFileName, defaultOutputFileName, strlen(defaultOutputFileName));
  }

  if (!strcmp(inputFileName, outputFileName)) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file '%s' is as the same as output file\nsorting terminated\n", argv[0], inputFileName);
    return EXIT_FAILURE;
  }
  // opening input file
  inputFile  = fopen((const char *) inputFileName, "r");

  // if the inputFile does not exist nor is readable, return failure...
  if (!inputFile) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to open file %s\n", argv[0], inputFileName);
    return EXIT_FAILURE;
  }

  //opening output file
  outputFile = fopen((const char *) outputFileName, "w");

  // if the outputFile is not writable, return failure...
  if(!outputFile) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to write to file %s\n", argv[0], outputFileName);
    return EXIT_FAILURE;
  }

  // counting lines (and numbers in list, assuming one line = one number) before declaring required memory
  for (c=getc(inputFile); c!=EOF; c=getc(inputFile))
    if (c == '\n')
      inputFileLineCounter++;
  rewind(inputFile); //back to the beginning of the file after counting the lines

  // allocs memory for given array of numbers
  numbersTable = malloc(inputFileLineCounter * sizeof(int));

  // read numbers to the array (I really assume we're having a number in every line...)
  for (i=0; i<inputFileLineCounter; i++)
    fscanf(inputFile, "%i", (numbersTable+i));

  // run heapsort on the array
  heapsort(numbersTable, inputFileLineCounter-1, type);

  for (i=0; i<inputFileLineCounter; i++)
    fprintf(outputFile, "%i\n", *(numbersTable+i));

  //closing opened files
  fclose(inputFile);
  fclose(outputFile);

  //freeing memory
  free(inputFileName);
  free(outputFileName);
  free(numbersTable);

  return 0;
}

void heapsort (int *A, int heapsize, int type) {
    int i;
    buildHeap(A, heapsize);
    for (i=heapsize; i>0; i--)
    {
      exchange(A, 0, i);
      heapsize--;
      if (!type)
        heapify(A, 0, heapsize);
      else
        heapifyIt(A, 0, heapsize);
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

void heapifyIt (int *A, int i, int heapsize) {
  int largest, le, ri;
  while (i<=heapsize)
  {
    le = (2*i);
    ri = (2*i)+1;
  
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
    else
      break;
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
