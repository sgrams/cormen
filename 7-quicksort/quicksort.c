/*
 * AL3.5, Stanisław Juliusz Grams (251000 UG MFI), 20171110
 * 
 * 1. USING
 *
 * Compile with command `gcc -o quicksort quicksort.c -Wextra -Wall -pedantic -std=gnu99`
 * Syntax: quicksort [-m (for modified quick_sort procedure) -t (for test mode) -c (sets constant value)] -i inputFile -o outputFile
 *
 * Input file is required (if test mode is not set).
 * If output file is not set, defaults will be forced (out.txt).
 * There will be no output file if program is in test mode.
 * There is possibility to use the program with streams,
 * eg. `cat test1.txt | ./quicksort` will save out.txt file,
 * `cat test1.txt | ./quicksort > out_test.txt` will save out_test.txt file.
 * It is also possible to combine streams and output file as a parameter and vice versa.
 *
 * Checked for memory leaks with
 * `valgrind --leak-check=full -v <various combinations of commands>`
 *
 * Tested under kernel 4.13.5-1-ARCH and gcc 7.2.0 
 *
 * Example execution with constant c (sortConst) set to 8192:
 *
 *
 * $ ./quicksort -t -c 8192
 * quick_sort algorithm
 * array size N | random data | unfavorable data
 *         8192       0.00353            0.14619
 *        16384       0.00409            0.60878
 *        32768       0.00861            2.35473
 *        65536       0.01915            9.73219
 * quick_sort_mod (with insert_sort, c=8192) algorithm
 * array size N | random data | unfavorable data
 *         8192       0.02920            0.10042
 *        16384       0.05073            0.84374
 *        32768       0.12965            4.11824
 *        65536       0.27783           17.89473
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define defaultOutputFileName "out.txt"
#define defaultSortConst      1024
#define defaultTestMode       0
#define defaultSortMode       0

#define defaultArraySize      8192


void exchange        (int *A, int a, int b);
void insert_sort     (int *A, int n);
void quick_sort      (int *A, int p, int r);
void quick_sort_mod  (int *A, int p, int r, int c);
int  partition       (int *A, int p, int r);

int verbose_flag = 0;

int main (int argc, char **argv) {
  FILE *inputFile;
  FILE *outputFile;

  char *inputFileName;
  char *outputFileName;
  char *constInputError=NULL;
  char c;

  int  *numbersTable=NULL;
  int  i, j, k, l; // iterators 
  int  testMode=defaultTestMode, sortMode=defaultSortMode, sortConst=defaultSortConst;
  int  inputFileFlag=0, outputFileFlag=0;
  int  numbersTableSize, inputFileLineCounter=0;
  int  testSortType=0;

  int    *testArray[2][2][4];
  double  timeArray[2][2][4];
  
  time_t start, end;
  srand(time(NULL));

  // Getopt settings;
  // i stands for input, o stands for output
  while ((c=getopt(argc, argv, "vciomt")) != -1)
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
      case '?':
      fprintf(stderr, "Syntax: quicksort [-m (for modified quick_sort procedure) -t (for test mode) -c (sets constant value)] -i inputFile -o outputFile\n");
        return EXIT_FAILURE;
      case 'm':
        sortMode=1;
        break;
      case 'c':
        if (sortMode || testMode) {
          if (argv[optind] != NULL) {
            sortConst = strtol(argv[optind], &constInputError, 0);
            if (*(constInputError))
            {
              fprintf(stderr, "\033[31merror:\033[0m unable to set constant value, using defaults\n");
              sortConst=defaultSortConst;
            }
          }
          else {
            fprintf(stderr, "\033[31merror:\033[0m constant not specified, using defaults...\n");
            sortConst = defaultSortConst;
          }
        }
        else {
          fprintf(stderr, "\033[31merror:\033[0m modified quick_sort procedure or test mode has to be set...\n");
        }
        break;
      case 't':
        testMode=1;
        break;
      case 'v':
        verbose_flag=1;
        break;
      default:
        break;
    }

  if (testMode)
  {
    // preparing memory and inserting data
    for (i=0; i<2; i++)
      for (j=0, k=defaultArraySize; j<4; j++, k=k<<1)
      {
        testArray[0][i][j] = (int *)malloc (k*sizeof(int));
        testArray[1][i][j] = (int *)malloc (k*sizeof(int));

        for (l=0; l<k; l++)
        {
          // inserting randomized data
          if (!i) {
            testArray[0][0][j][l] = rand()%k;
            testArray[1][0][j][l] = testArray[0][i][j][l];
          }
          // inserting descending data
          else {
            testArray[0][1][j][l] = k-l;
            testArray[1][1][j][l] = testArray[0][i][j][l];
          }
        }
      }

    // sorting loop...
    
    for (i=0; i<2; i++)
    {
      for(j=0, k=defaultArraySize; j<4; j++, k=k<<1)
      {
        // sorting using normal algorithm...
        start=clock();
        quick_sort(testArray[0][i][j], 0, k-1);
        end=clock();
        timeArray[0][i][j] = (double) difftime(end, start) / CLOCKS_PER_SEC;

        // sorting using modified algorithm...
        start=clock();
        quick_sort_mod(testArray[1][i][j], 0, k-1, sortConst);
        end=clock();
        timeArray[1][i][j] = (double) difftime(end, start) / CLOCKS_PER_SEC;
      }
    }

    // Freeing memory...
    
    for (i=0; i<2; i++)
      for (j=0; j<4; j++)
      {
        free(testArray[0][i][j]);
        free(testArray[1][i][j]);
      }
     
    
    // Printing results
    for (testSortType=0; testSortType<2; testSortType++)
    {
      if (!testSortType)
        printf("quick_sort algorithm\n");
      else
        printf("quick_sort_mod (with insert_sort, c=%i) algorithm\n", sortConst);
      printf("array size N | random data | unfavorable data\n");

      for (j=0, k=defaultArraySize; j<4; k=(k<<1), j++)
      {
        printf("%12i   %11.5lf   ", k, timeArray[testSortType][0][j]);
        printf("%16.5lf\n", timeArray[testSortType][1][j]);
      }
    }
    
    return EXIT_SUCCESS;
  }
  // There has to be a pipe/file on stdin, or inputFileFlag has to be set, to run the program
  if (!isatty(fileno(stdin))) {
    inputFile = stdin;
  }
  else {
    if (!inputFileFlag) {
      fprintf(stderr, "Syntax: quicksort [-m (for modified quick_sort procedure) -t (for test mode) -c (sets constant value)] -i inputFile -o outputFile\n");
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

    // Runs quicksort on the array
    if (sortMode) {
      if (verbose_flag)
        printf("sortConst=%i\n", sortConst);
      quick_sort_mod(numbersTable, 0, inputFileLineCounter-1, sortConst);
    }
    else
      quick_sort(numbersTable, 0, inputFileLineCounter-1);

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
  }
  return 0;
}
void insert_sort (int *A, int r) {
  if (verbose_flag)
  {
    static int verbose_check;
    fprintf(stdout, "insert_sort: I am inserting! %i\n", ++verbose_check);
  }

  int i, j, k;
  for (i=0; i<(r+1); i++)
  {
    k = *(A+i);
    j = i-1;
    while (j >= 0 && *(A+j) > k)
    {
      *(A+(j+1)) = *(A+j);
      j--;
    }
    A[j+1] = k;
  }
}

void quick_sort (int *A, int p, int r) {
  int q;
  if (p<r) {
    q = partition(A, p, r);
    quick_sort(A, p, q);
    quick_sort(A, q+1, r);
  }
}

void quick_sort_mod (int *A, int p, int r, int c) {
  int q;
  if ((r-p+1) < c)
    insert_sort(A, r);
  else {
    if (p<r) {
      q = partition(A, p, r);
      quick_sort_mod(A,p, q, c);
      quick_sort_mod(A,q+1, r, c);
    }
  }
}

int partition (int *A, int p, int r) {
  int x = *(A+r); // pivot
  int i = p-1, j;

  for (j=p; j<=r; j++)
  {
    if (A[j] <= x){
      ++i;
      exchange(A, i, j);
    }
  }
  if (i<r) return i;
  else     return i-1;
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
