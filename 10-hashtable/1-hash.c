/* AL6.2, Stanisław Juliusz Grams (251000 UG MFI), 20171229
 *
 * 1. USAGE
 *
 *   Compile with command `gcc -o 1-hash 1-hash.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0`
 *
 *   Syntax: "./1-hash -i 3700.txt [-x (xor style strtoi) -s (sum style strtoi)
 *            -t n (number of tests)]
 *   There are 3744 lines in the "3700.txt" file which contains single words in every single line.
 *
 *   Tested under kernel 4.14.8-1-ARCH and gcc 7.2.1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#define defaultStringBuffer   1024
#define defaultTestQuantity   4
#define defaultTestComposites 1584, 1668, 1724, 1800
#define defaultTestPrimes     1583, 1667, 1721, 1801
#define defaultSyntax         "./1-hash -i 3700.txt [-x (xor style strtoi) -s (sum style strtoi) -t n (n tests)]\n"
#define defaultErrorSyntax    "%s: \033[31mfatal error:\033[0m "

gulong hash_func          (gchar *key, gint m, gint strtoi_flag);
gulong hash_count_zeros   (gint *arr, gint n);
gfloat hash_arr_find_avg  (gint *arr, gint n);
guint  hash_arr_find_max  (gint *arr, gint n);
gulong hash_strtoi_djb2   (gchar *str);
gulong hash_strtoi_xor    (gchar *str);
gulong hash_strtoi_sum    (gchar *str);

gint main (gint argc, gchar *argv[])
{
  FILE *input_file = NULL;

  gchar read_char;
  gchar *string_buffer;
  gchar *input_filepath;

  gint *hash_array;

  gint *test_primes;
  gint *test_composites;
  gint test_primes_default[] = {defaultTestPrimes};
  gint test_composites_default[] = {defaultTestComposites};
  gint i, j, m, ch;
  gint strtoi_flag = 0;
  gint test_quantity = 0;
  gint test_flag = 0;

  input_file = NULL;
  string_buffer = g_malloc(defaultStringBuffer *sizeof(gchar)+1);

  while ((ch = getopt(argc, argv, "i:sxt:")) != -1)
  {
    switch (ch) {
      case 'i':
        if (!optarg){
          fprintf(stderr, defaultErrorSyntax
              "input file not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen((const gchar*) input_filepath, "r");
        if (!input_file) {
          fprintf(stderr, defaultErrorSyntax
              "input file not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 's':
        strtoi_flag=2;
        break;
      case 'x':
        strtoi_flag=1;
        break;
      case 't':
        test_quantity = atoi(optarg);
        if (!test_quantity) {
          fprintf(stderr, defaultErrorSyntax
              "wrong number of tests\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        test_flag = 1;
        /* Allocating memory for tests... */
        test_primes = g_malloc0(test_quantity*sizeof(gint));
        test_composites = g_malloc0(test_quantity*sizeof(gint));
        printf("Please enter pairs of prime and composite numbers for testing...\n");
        for (i=0; i<test_quantity; i++)
        {
          for (j=0; j<2; j++)
          {
            if (j)
              scanf("%i", &test_composites[i]);
            else
              scanf("%i", &test_primes[i]);
          }
        }
        break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
  }

  if (!input_file)
  {
    printf(defaultSyntax);
    return EXIT_SUCCESS;
  }

  if (!test_quantity) {
    test_quantity = defaultTestQuantity;
    test_primes = test_primes_default;
    test_composites = test_composites_default;
  }

  /*
   * Main body of program, performing tests here
   */
  if (strtoi_flag == 1)
    printf("Using xor style string to int conversion...\n");
  else if (strtoi_flag == 2)
    printf("Using sum style string to int conversion...\n");
  else
    printf("Using djb2 string to int conversion...\n");

  printf("       PRIME NUMBERS  |       COMPOSITE NUMBERS\n");
  printf("M:      avg:zero:max  | M:         avg:zero:max\n");
  printf("----------------------|------------------------\n");

  for (i=0; i<test_quantity; i++)
  {
    for (j=0; j<2; j++)
    {
      read_char = 0;
      fseek(input_file, 0L, SEEK_SET);
      m = test_primes[i];
      if (j)
        m = test_composites[i];
      hash_array = g_malloc0(m * sizeof(gint));
      while (read_char != EOF)
      {
        read_char = fscanf(input_file, "%[^\n]\n", string_buffer);
        if (read_char == EOF)
          break;
      hash_array[hash_func(string_buffer, m, strtoi_flag)]++;
      }
      printf("%i: %-4.2f:%-4lu:%-4u", m, hash_arr_find_avg(hash_array, m), hash_count_zeros(hash_array, m), 
          hash_arr_find_max(hash_array, m));
      if (!j) {
        if (strtoi_flag == 1)
          printf(" | ");
        else
          printf("  | ");
      }
      g_free (hash_array);
    }
  printf("\n----------------------|------------------------\n");
  }


  /*
   * Freeing memory
   */
  if (test_flag) {
    g_free(test_composites);
    g_free(test_primes);
  }
  if (input_file) {
    g_free(input_filepath);
  }
  g_free(string_buffer);
  fclose(input_file);
  return 0;
}

/*
 * hash_func calculates an int from string an then
 * is using basic hashing function h(k, m) = k % m
 */
gulong hash_func (gchar *key, gint m, gint strtoi_flag) {
  if (strtoi_flag == 1)
    return hash_strtoi_xor(key)%m;
  if (strtoi_flag == 2)
    return hash_strtoi_sum(key)%m;
  return hash_strtoi_djb2(key)%m;
}

/*
 * hash_strtoi_djb2 is based on djb2 algorithm
 * which is known to be one of the best way of hashing strings
 * 1. Why 5381?
 *    5381 is 709th prime
 *    709  is 127th prime
 *    127  is 31st  prime
 *    31   is 11th  prime
 *    5    is 3rd   prime
 *    3    is 2nd   prime
 *    2    is 1st   prime
 *
 *    5381st prime will exceed the limit of gulong (unsigned long long)
 *    it results in fewer collisions and better avalanching
 * 2. Why 33?
 *    Not really explained, it just simply works better than any of constants.
 *
 */
gulong hash_strtoi_djb2 (gchar *str) {
  gulong res = 5381;
  gint i;

  for (i=0; (guint) i < strlen(str); i++)
  {
    res = ((res << 5) + res) + str[i];
  }
  return res;
}

/*
 * hash_strtoi_xor is a XOR style string to int conversion function
 * it is easy, has its pros and cons
 */
gulong hash_strtoi_xor (gchar *str) {
  gulong res = 0;
  gint i;

  for (i=0; (guint) i+1 < strlen(str); i+=2)
  {
    res ^= (256*(str[i] + str[i+1]));
  }

  return res;
}

/*
 * hash_strtoi_sum is a sum style string to int conversion function
 * it is similar to djb2
 */
gulong hash_strtoi_sum (gchar *str) {
 gulong res = 0;
 gint i;

 for (i=0; (guint) i < strlen(str); i++)
 {
   res = res*33 + str[i];
 }

 return res;
}

/*
 * hash_count_zeros function counts number of 0's in given array
 */
gulong hash_count_zeros (gint *arr, gint n) {
  gint i, res = 0;
  for (i=0; i<n; i++) {
   if (arr[i] == 0)
     res++;
  }
  return res;
}

/*
 * hash_arr_find_avg function finds average from numbers which are not
 * equal to 0 in the given array
 */
gfloat hash_arr_find_avg (gint *arr, gint n) {
  gint i, m;
  gfloat avg = 0;
  for (i=0, m=0; i<n; i++)
  {
    if (arr[i]>0) {
      avg+=arr[i];
      ++m;
    }
  }
  return avg/=m;
}

/*
 * hash_arr_find_max function finds maximum number in given array
 */
guint hash_arr_find_max (gint *arr, gint n) {
  gint i, res;
  for (i=1, res = arr[0]; i<n; i++)
  {
    if (res<arr[i]) {
      res = arr[i];
    }
  }
  return res;
}
