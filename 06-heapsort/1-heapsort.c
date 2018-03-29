/*
 * AL2.4, Stanisław Juliusz Grams (251000 UG MFI), 20171024
 * 
 * 1. USING
 *
 * Compile with command `gcc -o heapsort heapsort.c -Wextra -Wall -pedantic -std=gnu99`
 * Syntax `./heapsort [-l (for iterative heapsort_heapify function!)] -i input_file -o output_file`
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
 * I had to change Heapsort function definition as well as the others because
 * I didn't want to use the global variable (heap_size) and I wanted to pass the heapsort_heapify type, too.
 * In my implementation I started counting in the array from 0, unlike the pseudocode which counts from 1.
 * That forces me to decrement array length (numbersTableLength--;) to avoid errors.
 *
 * Tested under kernel 4.13.5-1-ARCH and gcc 7.2.0 
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#define defaultOutputFileName "out.txt"

void heapsort                   (gint *A, gint n, gint type);
void heapsort_buildheap         (gint *A, gint heap_size, gint type);
void heapsort_heapify           (gint *A, gint i, gint heap_size);
void heapsort_heapify_iterative (gint *A, gint i, gint heap_size);
void exchange                   (gint *A, gint a, gint b);

gint main (gint argc, gchar **argv) {
  FILE *input_file  = NULL;
  FILE *output_file = NULL;

  gchar *input_filepath  = NULL;
  gchar *output_filepath = NULL;
  gchar c;

  gint  *numbers_array = NULL;
  gint  i;
  gint  type = 0;
  gint  numbersTableSize, inputFileLineCounter=0;
  
  // Getopt settings;
  // l stands for loop (iterative version of heapsort_heapify)
  // i stands for input, o stands for output
  while ((c=getopt(argc, argv, "li:o")) != -1)
    switch (c) {
      case 'i': // Allocates memory and sets input filename (or path)
        // Checks if not file and stream at once
        if (!isatty(fileno(stdin))) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m input cannot be a file and a stream at once\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        // Checks if input file is specified
        if (!optarg) {
          fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file not specified\n"
              "sorting terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar *) input_filepath, "r");
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
        output_filepath = g_strdup(optarg);
        output_file = fopen((const gchar *) output_filepath, "w");
        break;
      case 'l':
        type=1;
        break;
      case '?':
        fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i input_file -o output_file\n");
        return EXIT_FAILURE;
      default:
        break;
    }
  // There has to be a pipe/file on stdin, or inputFileFlag has to be set, to run the program
  if (!isatty(fileno(stdin))) {
    input_file = stdin;
  }
  else {
    if (!input_file) {
      fprintf(stderr, "Syntax: heapsort [-l (for iterative version)] -i input_file -o output_file\n");
      if (output_file) {
        g_free(output_filepath);
      }
      return EXIT_FAILURE;
    }
  }
  if (!input_file) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to open file %s\n"
            "sorting terminated\n", argv[0], input_filepath);
    g_free(input_filepath);
    if (isatty(fileno(stdout)))
      g_free(output_filepath);
    return EXIT_FAILURE;
  }

  // Check if there is a pipe or a terminal on the stdout
  if (!isatty(fileno(stdout))) {
      output_file = stdout;
  }
  else {
    // Checking if the file names are not the same
    if (output_file && !strcmp(input_filepath, output_filepath)) {
      fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file '%s' is as the same as output file\n"
          "sorting terminated\n", argv[0], input_filepath);
      g_free(input_filepath);
      g_free(output_filepath);
      return EXIT_FAILURE;
    }
    // If the outputFileFlag is not set, then the defaultOutputFileName will become output_filepath
    if (!output_file) {
      output_filepath = g_malloc((strlen(defaultOutputFileName)+1)*sizeof(gchar));
      strncpy(output_filepath, defaultOutputFileName, strlen(defaultOutputFileName)+1);
    }
    // Opening output file
    output_file = fopen((const gchar *) output_filepath, "w");
 }
 
  // If the output_file is not writable, return failure...
  if (!output_file) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to write to file %s\n", argv[0], output_filepath);
    if (input_file) {
      g_free(input_filepath);
      fclose(input_file);
    }
    if (output_file) {
      g_free(output_filepath);
      fclose(output_file);
    }
    return EXIT_FAILURE;
  }

  // Reading input and counting numbers...
  // Reserving memory for array at the beginning reduces usage of realloc function
  // what makes the program faster
  for (inputFileLineCounter=0, numbersTableSize=128,
      numbers_array=g_malloc(numbersTableSize*sizeof(gint)), c=0; c != EOF; inputFileLineCounter++) 
  {
    // Checks if file is bigger than size and realloc for more memory
    if (inputFileLineCounter >= numbersTableSize) {
      // Shifts to the left one bit, makes more memory
      // 128 -> 256 -> 512 -> 1024...
      numbersTableSize<<=1;
      numbers_array=realloc(numbers_array, numbersTableSize*sizeof(gint));
    }
    c=fscanf(input_file, "%i", (numbers_array+inputFileLineCounter));
  }
  // For loop goes one line too far to check if EOF
  // Decrements and deallocates unused memory
  numbers_array=realloc(numbers_array, (--inputFileLineCounter)*sizeof(gint));
  
  // Runs heapsort on the array
  heapsort(numbers_array, inputFileLineCounter, type);

  // Writes sorted array to output_file
  for (i=0; i<inputFileLineCounter; i++)
    fprintf(output_file, "%i\n", *(numbers_array+i));

  // Closes opened files if they are not streams
  if (isatty(fileno(stdin)))
    fclose(input_file);
  if (isatty(fileno(stdout)))
    fclose(output_file);

  // Deallocates memory
  if (isatty(fileno(stdin)))
    g_free(input_filepath);
  if (isatty(fileno(stdout)))
    g_free(output_filepath); 
  g_free(numbers_array);

  return 0;
}

void heapsort (gint *A, gint n, gint type) {
    gint i;
    gint heap_size = n-1;

    heapsort_buildheap(A, heap_size, type);
    for (i=heap_size; i>=1; i--)
    {
      exchange(A, 0, i);
      heap_size--;
      if (!type)
        heapsort_heapify(A, 0, heap_size);
      else
        heapsort_heapify_iterative(A, 0, heap_size);
    }
}

void heapsort_buildheap (gint *A, gint heap_size, gint type) {
    for (gint i=heap_size/2; i>=0; i--)
      if (type)
        heapsort_heapify_iterative(A, i, heap_size);
      else
        heapsort_heapify(A, i, heap_size);
}

// Recursive heapsort_heapify version
void heapsort_heapify (gint *A, gint i, gint heap_size) {
  gint largest;

  gint le = (2*i);
  gint ri = (2*i)+1;

  if (le <= heap_size && *(A+le) > *(A+i))
    largest = le;
  else
    largest = i;

  if (ri <= heap_size && *(A+ri) > *(A+largest))
    largest = ri;

  if (largest != i) {
    exchange(A, i, largest);
    heapsort_heapify(A, largest, heap_size);
  }
}

// Iterative heapsort_heapify version
void heapsort_heapify_iterative (gint *A, gint i, gint heap_size) {
  gint largest, le, ri;

  while (i<=heap_size)
  {
    le = (2*i)+1;
    ri = (2*i)+2;
  
    if (le <= heap_size && *(A+le) > *(A+i))
      largest = le;
    else
      largest = i;
  
    if (ri <= heap_size && *(A+ri) > *(A+largest))
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
void exchange (gint *A, gint a, gint b) {
  // If *(A+a) would equal *(A+b), the XOR operation would zero both of variables
  if (*(A+a) != *(A+b)) {
    *(A+a) ^= *(A+b);
    *(A+b) ^= *(A+a);
    *(A+a) ^= *(A+b);
  }
}
