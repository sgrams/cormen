/*
 * AL4.7, Stanisław Juliusz Grams (251000 UG MFI), 20171126
 * 
 * 1. USAGE
 *
 * Compile with command `gcc -o radix radix.c `pkg-config --cflags glib-2.0 --libs glib-2.0`
 * Syntax: ./radixsort [-q (for quick_sort sorting) -t (for test mode)] -i inputFile -o outputFile
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <glib.h>
#include <locale.h>

#define defaultOutputFileName "out.txt"
#define defaultTestMode       0
#define defaultSortMode       0
#define defaultStrBuffer      1024
#define defaultNameBuffer     256

typedef struct {
    gsize     len;
    gint      pop;
    gchar  *name;
} name_entry;

void exchange        (name_entry *A, gint a, gint b);
void quick_sort      (name_entry *A, gint p, gint r);
void radix_sort      (name_entry *A, gint n);
void str_count       (name_entry *A, gint n, gint b);
void ne_cpy          (name_entry *B, name_entry A);
gint char_to_index   (gchar c);

gint partition       (name_entry *A, gint p, gint r);
guint find_max_length (name_entry *A, gint n);

gint verbose_flag = 0;

gint main (gint argc, gchar **argv) {
  FILE *inputFile = NULL;
  FILE *outputFile = NULL;

  gchar *inputFileName = NULL;
  gchar *outputFileName = NULL;
  gchar *inToken = NULL, *nameBuf = NULL;
  gchar c;

  name_entry *strTable   = NULL;
  name_entry *strTable_1 = NULL;
  gchar strBuf[defaultStrBuffer];

  gint  ch, i; // iterators 
  gint  sortMode=defaultSortMode, testMode=defaultTestMode;
  gint  strTableSize, inputFileLineCounter=0;

  time_t start, end;
  srand(time(NULL));

  setlocale (LC_ALL, "pl_PL.UTF-8");
  // Getopt settings;
  // i stands for input, o stands for output
  while ((ch=getopt(argc, argv, "vioqt")) != -1)
    switch (ch) {
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
        inputFileName = g_malloc((strlen(argv[optind])+1)*sizeof(gchar));
        strncpy(inputFileName, argv[optind], strlen(argv[optind])+1);
        inputFile = fopen((const gchar *) inputFileName, "r");
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
        outputFileName = g_malloc((strlen(argv[optind])+1)*sizeof(gchar));
        strncpy(outputFileName, argv[optind], strlen(argv[optind])+1);
        outputFile = fopen((const gchar *) outputFileName, "w");
        break;
      case '?':
        fprintf(stderr, "Syntax: ./radixsort [-q (for quick_sort sorting) -t (for test mode)] -i inputFile -o outputFile\n");
        return EXIT_FAILURE;
      case 'q':
        sortMode=1;
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

  // There has to be a pipe/file on stdin, or inputFile has to be set, to run the program
  if (!isatty(fileno(stdin))) {
    inputFile = stdin;
  }
  else {
    if (inputFile == NULL) {
      fprintf(stderr, "Syntax: ./radixsort [-q (for quick_sort sorting) -t (for test mode)] -i inputFile -o outputFile\n");
      if (outputFile != NULL) {
        g_free(outputFileName);
      }
      return EXIT_FAILURE;
    }
  }
  if (inputFile == NULL) {
    fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to open file %s\n"
            "sorting terminated\n", argv[0], inputFileName);
    g_free(inputFileName);
    if (isatty(fileno(stdout)))
      g_free(outputFileName);
    return EXIT_FAILURE;
  }

  // Check if there is a pipe or a terminal on the stdout
  if (!isatty(fileno(stdout))) {
      outputFile = stdout;
  }
  else {
    // Checking if the file names are not the same
    if ((outputFile != NULL) && !strcmp(inputFileName, outputFileName)) {
      fprintf(stderr, "%s: \033[31mfatal error:\033[0m input file '%s' is as the same as output file\n"
          "sorting terminated\n", argv[0], inputFileName);
      g_free(inputFileName);
      g_free(outputFileName);
      return EXIT_FAILURE;
    }
    // If the outputFile is not set, then the defaultOutputFileName will become outputFileName
    if (outputFile == NULL) {
      outputFileName = g_malloc((strlen(defaultOutputFileName)+1)*sizeof(gchar));
      strncpy(outputFileName, defaultOutputFileName, strlen(defaultOutputFileName)+1);
      outputFile = fopen((const gchar *) outputFileName, "w");
    }
 
    // If the outputFile is not writable, return failure...
    if (outputFile == NULL) {
      fprintf(stderr, "%s: \033[31mfatal error:\033[0m unable to write to file %s\n", argv[0], outputFileName);
      if (inputFile != NULL) {
        g_free(inputFileName);
        fclose(inputFile);
      }
      if (outputFile != NULL) {
        g_free(outputFileName);
        fclose(outputFile);
      }
      return EXIT_FAILURE;
    }

    /*
       Reading input to a structure
       Reserving memory for array at the beginning reduces usage of realloc function
       what makes the program faster
    */
    
    for (inputFileLineCounter=0, strTableSize=1024000,
       strTable=g_malloc(strTableSize*sizeof(name_entry)), c=0; c != EOF; inputFileLineCounter++) 
    {
      /* Checks if file is bigger than size and reallocates for more memory */
      if (inputFileLineCounter >= strTableSize) {
        strTableSize<<=1;
        strTable=realloc(strTable, strTableSize*sizeof(name_entry));
      }

      c = fscanf(inputFile, "%[^\n]\n", strBuf);
      if (c == EOF) // check if buffer is not empty...
        break;

      nameBuf = strBuf;
      inToken = strsep(&nameBuf, " ");
      strTable[inputFileLineCounter].name = g_strdup(nameBuf);
      strTable[inputFileLineCounter].len = strlen(nameBuf);
      strTable[inputFileLineCounter].pop = strtol(inToken, NULL, 10);
    }
    
    /* For loop goes one line too far to check if EOF
     * Decrements and deallocates unused memory
     */
    strTable=realloc(strTable, inputFileLineCounter*sizeof(name_entry));
    
    if (testMode) {
      // duplicating struct array
      strTable_1 = (name_entry *)g_malloc (inputFileLineCounter * sizeof(name_entry));
      for (i=0; i<inputFileLineCounter; i++)
        ne_cpy(&strTable_1[i], strTable[i]);

      printf("Times of sorting %i char * array.\n", inputFileLineCounter);

      // radix sort
      start = clock();
      radix_sort(strTable, inputFileLineCounter);
      end = clock();
      printf("  radix_sort: %4.3lfs\n", (gdouble) difftime(end, start) / CLOCKS_PER_SEC);

      // quick sort
      start = clock();
      quick_sort(strTable_1, 0, inputFileLineCounter-1);
      end = clock();
      printf("  quick_sort: %4.3lfs\n", (gdouble) difftime(end, start) / CLOCKS_PER_SEC);

      for (i=0; i<inputFileLineCounter; i++)
        fprintf(outputFile, "%s\n", strTable[i].name);
      
      if (isatty(fileno(stdin)))
        fclose(inputFile);
      if (isatty(fileno(stdout)))
        fclose(outputFile);

      for (i=0; i<inputFileLineCounter; i++)
      {
        g_free((strTable+i)->name);
        g_free((strTable_1+i)->name);
      }
      g_free(strTable);
      g_free(strTable_1);

      if (isatty(fileno(stdin)))
        g_free(inputFileName);
      if (isatty(fileno(stdout)))
        g_free(outputFileName);

      return EXIT_SUCCESS;
    }

    if (sortMode)
      quick_sort(strTable, 0, inputFileLineCounter-1);
    else
      radix_sort(strTable, inputFileLineCounter);

    // Writes sorted array to outputFile
    for (i=0; i<inputFileLineCounter; i++)
      fprintf(outputFile, "%s\n", strTable[i].name);
    
    // Closes opened files if they are not streams
    //if (isatty(fileno(stdin)))
      fclose(inputFile);
    //if (isatty(fileno(stdout)))
      fclose(outputFile);

    // Deallocates memory
    for (i=0; i<inputFileLineCounter; i++)
    g_free((strTable+i)->name);
    g_free(strTable);

    if (isatty(fileno(stdin)))
      g_free(inputFileName);
    if (isatty(fileno(stdout)))
      g_free(outputFileName); 

  }
  return 0;
}

guint find_max_length (name_entry *A, gint n) {
  gint i, max=A[0].len;
  for (i=1; i<n; i++)
    if ((guint)A[i].len>(guint)max)
      max=A[i].len;
  return (guint) max;
}
void radix_sort (name_entry *A, gint n) {
  gint max = (gint) find_max_length(A, n);
  gint b;

  // string sorting
  for (b=max-1; b >= 0; b--)
    str_count(A, n, b);
}

void str_count (name_entry *A, gint n, gint b) {
  gint i, k;
  gint c[28] = {0};
  name_entry *B = g_malloc (n*sizeof(name_entry)); // output array

  // counting frequencies
  for (i=0; i<n; i++)
  {
    if (A[i].len >= (guint)b)
      c[char_to_index(A[i].name[b])]++;
    else
      c[0]++;
  }

  // cumulating (also works)
  for (k=1; k<28; k++)
    c[k]+=c[k-1];

  for (i=n-1; i>=0; i--)
  {
    if (A[i].len >= (guint)b)
    {
      ne_cpy(&B[c[char_to_index(A[i].name[b])]-1], A[i]);
      c[char_to_index(A[i].name[b])]--;
    }
    else
    {
      ne_cpy(&B[c[0]-1], A[i]);
      c[0]--;
    }
  }

  for (i=0; i<n; i++)
  {
    g_free((A+i)->name);
    ne_cpy((A+i), *(B+i));
    g_free((B+i)->name);
  }

  g_free(B);
}
gint char_to_index (gchar c) {
  gint b = (gint) c;
  if (b == 32) // space
    return 0;
  else if (b >= 65 && b <= 90) // big letters
    return (gint) b - 64;
  else if (b >= 97 && b <= 122) // small letters
    return (gint) b - 96;
  else if (b == 45) // dash
    return 27;
  else return 0;
}

void ne_cpy (name_entry *B, name_entry A) {
  B->name = g_strdup(A.name);
  B->len = (gint) A.len;
  B->pop = (gint) A.pop;
}

void quick_sort (name_entry *A, gint p, gint r) {
  gint q;
  if (p<r) {
    q = partition(A, p, r);
    quick_sort(A, p, q);
    quick_sort(A, q+1, r);
  }
}

gint partition (name_entry *A, gint p, gint r) {
  gchar *x = g_utf8_casefold(A[r].name, A[r].len); // pivot
  gchar *y = NULL;
  gint i = p-1, j;

  for (j=p; j<=r; j++)
  {
    y = g_utf8_casefold(A[j].name, A[j].len);
    if (g_strcmp0(y, x) <= 0) {
      ++i;
      exchange (A, i, j);
    }
    g_free(y);
  }
  g_free(x);
  if (i<r) return i;
  else     return i-1;
}

void exchange (name_entry *A, gint a, gint b) {
  name_entry tmp = *(A+a);
  *(A+a) = *(A+b);
  *(A+b) = tmp;
}
