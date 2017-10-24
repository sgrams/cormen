/*
 * AL2.4, Stanisław Juliusz Grams (251000 UG MFI), 20171022
 * 
 * 1. USING
 *
 * Compile with command `gcc -o heapsort heapsort.c -Wextra -Wall -pedantic -std=gnu99`
 * Syntax `./heapsort [-l (for iterative heapify function!)] -i inputFile -o outputFile`
 *
 * Input file is required. If output file is not set, defaults will be forced (out.txt).
 * There is possibility to use the program with streams,
 * eg. `cat test1.txt | ./heapsort` will save out.txt file,
 * `cat test1.txt | ./heapsort > out_test.txt` will save out_test.txt file.
 * It is also possible to combine streams and output file as a parameter and vice versa.
 *
 * Checked for memory leaks with
 * `valgrind --leak-check=full -v <various combinations of commands>`
 *
 * Heapsort definition has to differ from the pseudocode
 * I had to change Heapsort function definition as well as other because
 * I didn't want to use the global variable (heapsize) and I wanted to pass the heapify type, too.
 * In my implementation I started counting in the array from 0, unlike the pseudocode which counts from 1.
 * That forces my to decrement array length (numbersTableLength--;) to avoid errors.
 *
 * Tested under kernel 4.13.5-1-ARCH and gcc 7.2.0 
 *
 * 2. LICENSE
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define defaultOutputFileName "out.txt"

void heapsort  (int *A, int numbersTableLength, int type);
void buildHeap (int *A, int heapSize);
void heapify   (int *A, int i, int heapSize);
void heapifyIt (int *A, int i, int heapSize);
void exchange  (int *A, int a, int b);

int main (int argc, char **argv) {
  FILE *inputFile;
  FILE *outputFile;

  char *inputFileName;
  char *outputFileName;
  char c;

  int  *numbersTable=NULL;
  int  i, type=0, inputFileFlag=0, outputFileFlag=0;
  int  numbersTableSize, inputFileLineCounter=0;
  
  // Getopt settings;
  // l stands for loop (iterative version of heapify)
  // i stands for input, o stands for output
  while ((c=getopt(argc, argv, "lio")) != -1)
    switch (c) {
      case 'i': // Allocates memory and sets input filename (or path)
        // Checks if not file and stream at once
        if (!isatty(fileno(stdin))) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m input cannot be a file and a stream at once\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        // Checks if input file is specified
        if (argv[optind] == NULL) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file not specified\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        inputFileName = malloc((strlen(argv[optind])+1)*sizeof(char));
        strncpy(inputFileName, argv[optind], strlen(argv[optind])+1);
        inputFileFlag=1;
        break;
      case 'o': // Allocates memory and sets output filename (or path)
        // Checks if not file and stream at once
        if (!isatty(fileno(stdout))) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m output cannot be a file and a stream at once\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        // Checks if output file is specified
        if (argv[optind] == NULL) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m output file not specified\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        outputFileName = malloc((strlen(argv[optind])+1)*sizeof(char));
        strncpy(outputFileName, argv[optind], strlen(argv[optind])+1);
        outputFileFlag=1;
        break;
      case 'l':
        type=1;
        break;
      case '?':
        fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i inputFile -o outputFile\n");
        return EXIT_FAILURE;
      default:
        break;
    }

  // There has to be a pipe/file on stdin, or inputFileFlag has to be set, to run the program
  if (!isatty(fileno(stdin))) {
    inputFile = stdin;
  }
  else {
    if (!inputFileFlag) {
      fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i inputFile -o outputFile\n");
      if (outputFileFlag) {
        free(outputFileName);
      }
      return EXIT_FAILURE;
    }
    // Opening the input file
    inputFile = fopen((const char *) inputFileName, "r");
  }
  if (!inputFile) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to open file %s\n"
            "sorting terminated\n", argv[0], inputFileName);
    free(inputFileName);
    if (isatty(fileno(stdout)))
      free(outputFileName);
    return EXIT_FAILURE;
  }

  // Check if there is a pipe or a terminal on the stdout
  if (!isatty(fileno(stdout))) {
      outputFile = stdout;
  }
  else {
    // Checking if the file names are not the same
    if (outputFileFlag && !strcmp(inputFileName, outputFileName)) {
      fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file '%s' is as the same as output file\n"
          "sorting terminated\n", argv[0], inputFileName);
      free(inputFileName);
      free(outputFileName);
      return EXIT_FAILURE;
    }
    // If the outputFileFlag is not set, then the defaultOutputFileName will become outputFileName
    if (!outputFileFlag) {
      outputFileName = malloc((strlen(defaultOutputFileName)+1)*sizeof(char));
      strncpy(outputFileName, defaultOutputFileName, strlen(defaultOutputFileName)+1);
      outputFileFlag=1;
    }
    // Opening output file
    outputFile = fopen((const char *) outputFileName, "w");
 }
 
  // If the outputFile is not writable, return failure...
  if (!outputFile) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to write to file %s\n", argv[0], outputFileName);
    if (inputFileFlag) {
      free(inputFileName);
      fclose(inputFile);
    }
    if (outputFileFlag) {
      free(outputFileName);
      fclose(outputFile);
    }
    return EXIT_FAILURE;
  }

  // Reading input and counting numbers...
  // Reserving memory for array at the beginning reduces usage of realloc function
  // what makes the program faster
  for (inputFileLineCounter=0, numbersTableSize=128,
      numbersTable=malloc(numbersTableSize*sizeof(int)), c=0; c != EOF; inputFileLineCounter++) 
  {
    // Checks if file is bigger than size and realloc for more memory
    if (inputFileLineCounter >= numbersTableSize) {
      // Shifts to the left one bit, makes more memory
      // 128 -> 256 -> 512 -> 1024...
      numbersTableSize<<=1;
      numbersTable=realloc(numbersTable, numbersTableSize*sizeof(int));
    }
    c=fscanf(inputFile, "%i", (numbersTable+inputFileLineCounter));
  }
  // For loop goes one line too far to check if EOF
  // Decrements and deallocates unused memory
  numbersTable=realloc(numbersTable, (--inputFileLineCounter)*sizeof(int));
  
  // Runs heapsort on the array
  heapsort(numbersTable, inputFileLineCounter, type);

  // Writes sorted array to outputFile
  for (i=0; i<inputFileLineCounter; i++)
    fprintf(outputFile, "%i\n", *(numbersTable+i));

  // Closes opened files if they are not streams
  if (isatty(fileno(stdin)))
    fclose(inputFile);
  if (isatty(fileno(stdout)))
    fclose(outputFile);

  // Deallocates memory
  if (isatty(fileno(stdin)))
    free(inputFileName);
  if (isatty(fileno(stdout)))
    free(outputFileName); 
  free(numbersTable);

  return 0;
}

void heapsort (int *A, int numbersTableLength, int type) {
    int i;
    // We have to decrement because we started counting from 0,
    // unlike in the pseudocode from 1
    int heapSize = numbersTableLength-1;

    buildHeap(A, heapSize);
    for (i=heapSize; i>=1; i--)
    {
      exchange(A, 0, i);
      heapSize--;
      if (!type)
        heapify(A, 0, heapSize);
      else
        heapifyIt(A, 0, heapSize);
    }
}

void buildHeap (int *A, int heapSize) {
    for (int i=heapSize/2; i>=0; i--)
      heapify(A, i, heapSize);
}

// Recursive heapify version
void heapify (int *A, int i, int heapSize) {
  int largest;

  int le = (2*i);
  int ri = (2*i)+1;

  if (le <= heapSize && *(A+le) > *(A+i))
    largest = le;
  else
    largest = i;

  if (ri <= heapSize && *(A+ri) > *(A+largest))
    largest = ri;

  if (largest != i) {
    exchange(A, i, largest);
    heapify(A, largest, heapSize);
  }
}

// Iterative heapify version
void heapifyIt (int *A, int i, int heapSize) {
  int largest, le, ri;

  while (i<=heapSize)
  {
    le = (2*i);
    ri = (2*i)+1;
  
    if (le <= heapSize && *(A+le) > *(A+i))
      largest = le;
    else
      largest = i;
  
    if (ri <= heapSize && *(A+ri) > *(A+largest))
      largest = ri;
  
    if (largest != i) {
      exchange(A, i, largest);
      i = largest;
    }
    else
      break;
  }
}

// Exchange function that uses the XOR style swap
void exchange (int *A, int a, int b) {
  // If *(A+a) would equal *(A+b), the XOR operation would zero both of variables
  if (*(A+a) != *(A+b)) {
    *(A+a) ^= *(A+b);
    *(A+b) ^= *(A+a);
    *(A+a) ^= *(A+b);
  }
}
