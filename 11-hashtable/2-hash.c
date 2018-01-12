/* AL6.3, Stanisław Juliusz Grams (251000 UG MFI), 20180102
 * S+OK
 * H(k,i)=(h(k)+ (A*i) + (b*i*i)) mod m; (quadratic probing)
 * h(k)  = k mod m;
 *
 * 1. USAGE
 *
 *   Compile with command "gcc -o 2-hash 2-hash.c
 *   `pkg-config --cflags glib-2.0 --libs glib-2.0` -lm"
 *
 *   Syntax: "./2-hash -i keys.txt -v (for verbose output) -b bigM -s smallM [-h (0=djb2, 1=xor, 2=sum) strtoi style]"
 *
 *   Tested under kernel 4.14.8-1-ARCH and gcc 7.2.1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <math.h>

#define defaultHashFuncConstA 3
#define defaultHashFuncConstB 11
#define defaultStringBuffer   1024
#define defaultSyntax         "./2-hash -i keys.txt -v (for verbose output) -b bigM -s smallM [-h (0=djb2, 1=xor, 2=sum) strtoi style]\n"
#define defaultErrorSyntax    "%s: \033[31mfatal error:\033[0m "

#define defaultSmallM         11
#define defaultBigM           19403

typedef struct {
  gint pop;
  gchar *name;
} name_entry;

gulong hash_func          (gchar *key, gint i, gint m, gint strtoi_flag);

glong hash_arr_insert    (name_entry *hash_arr, name_entry *pos, gint m, gint strtoi_flag, gint test_flag);
glong hash_arr_search    (name_entry *hash_arr, gchar *key, gint m, gint strtoi_flag, gint test_flag);

gulong hash_strtoi_djb2   (gchar *str);
gulong hash_strtoi_xor    (gchar *str);
gulong hash_strtoi_sum    (gchar *str);


int main (gint argc, gchar **argv)
{
  FILE *input_file = NULL;
  name_entry *names_array = NULL;
  name_entry *hashtab_arr = NULL;
  
  glong res;

  gint input_file_lc;
  gint names_array_size;
  gint i, j, ch;

  gint strtoi_flag = 0;
  gint verbose_flag = 0;

  gint big_m = defaultBigM;
  gint small_m = defaultSmallM;

  gchar *input_filepath   = NULL;
  gchar *buf_str  = NULL;
  gchar *buf_nam  = NULL;
  gchar *buf_tok  = NULL;
  gchar  c;

  while ((ch = getopt(argc, argv, "i:vb:s:h:")) != -1)
  {
    switch (ch) {
      case 'i':
        if (!optarg) {
        fprintf(stderr, defaultErrorSyntax
            "input file not specified\nprogram terminated\n", argv[0]);
        return EXIT_FAILURE;
        }
        input_filepath = g_strdup(optarg);
        input_file = fopen ((const gchar *) input_filepath, "r");
        if (!input_file) {
          fprintf(stderr, defaultErrorSyntax
              "input file not specified\nprogram terminated\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 'v':
        verbose_flag = 1;
        break;
      case 'b':
        big_m = atoi(optarg);
        if (big_m == 0)
        {
          fprintf(stderr, "NaN!\n");
          big_m = defaultBigM;
        }
        break;
      case 's':
        small_m = atoi(optarg);
        if (small_m == 0) {
          fprintf(stderr, "NaN!\n");
          small_m = defaultSmallM;
        }
        break;
      case 'h':
        strtoi_flag = atoi(optarg);
        if (strtoi_flag < 0 && strtoi_flag > 2)
          strtoi_flag = 0;
        break;
      break;
      case '?':
        return EXIT_FAILURE;
      default:
        break;
    }
   }

  if (!input_file) {
    printf(defaultSyntax);
    return EXIT_SUCCESS;
  }
  /*
   * Loading data into temporary array
   */
  buf_str = g_malloc(defaultStringBuffer * sizeof (gchar));
  for (input_file_lc=0, names_array_size=2048, names_array=g_malloc0(names_array_size*sizeof(name_entry)),
      c=0; c!=EOF; input_file_lc++)
  {
    if (input_file_lc>=names_array_size)
    {
      names_array_size<<=1;
      names_array = g_realloc(names_array, names_array_size*sizeof(name_entry));
    }
    c = fscanf(input_file, "%[^\n]\n", buf_str);
    if (c == EOF)
      break;
    
    buf_nam = buf_str;
    buf_tok = strsep(&buf_nam, " ");
    names_array[input_file_lc].name = g_strdup(buf_nam);
    names_array[input_file_lc].pop = strtol(buf_tok, NULL, 10);
  }
  names_array = realloc(names_array, input_file_lc*sizeof(name_entry));

  if (big_m > (int)(float)roundf(input_file_lc*0.8))
    big_m=defaultBigM;
  if (small_m > (int)(float)roundf(input_file_lc*0.8))
    small_m=defaultSmallM;


  if (strtoi_flag == 1)
    printf("Using xor style string to int conversion...\n");
  else if (strtoi_flag == 2)
    printf("Using sum style string to int conversion...\n");
  else
    printf("Using djb2 string to int conversion...\n");
  /*
   * Main body of program, performing tests, etc.
   */

  // Verbose check-print of loaded file
  if (verbose_flag)
  {
    fprintf(stdout, "Loading input_file into an array...\n");
    for (i=0; i<input_file_lc; i++)
      printf("T[%i].name = %s\n", i, names_array[i].name);
  }

  // Loading and calculating hashes of first 9 names into hash table...
  printf("Loading first %i elements (80%% of %i)...\n", (int)(float)roundf(0.8*small_m), small_m);

  hashtab_arr = g_malloc0(small_m*sizeof(name_entry));
  for (i=0; i<(int)(float)roundf((0.8*small_m)); i++)
  {
    res = hash_arr_insert(hashtab_arr, &names_array[i], small_m, strtoi_flag, 1);
    if (res == -1)
      printf("Could not insert A[%i] (%s), not enough space.\n", i, names_array[i].name);
    else
      printf("Inserted A[%i] (%s) into H[%lu].\n", i, names_array[i].name, res);
  }

  printf("\n");
  printf("Searching for last three elements...\n");
  for (j=i=(int)(float)roundf((0.8*small_m))-1; i>j-3; i--)
  {
    res = hash_arr_search(hashtab_arr, names_array[i].name, small_m, strtoi_flag, 1);
    if (res == -1)
      printf("Could not find %s in hashtable.\n", names_array[i].name);
    else
      printf("Found %s (A[%i]) under H[%lu].\n", names_array[i].name, i, res);
  }
  g_free(hashtab_arr);
 
  printf("\n");
  printf("Press any key to continue...\n");
  getchar();
  
  // Performing main test...
  // Inserting 80% of given M
  hashtab_arr = g_malloc0(big_m*sizeof(name_entry));
  
  printf("Loading first %i elements (80%% of %i)...\n", (int)(float)roundf(0.8*big_m), big_m);
  for (i=0; i<(int)(float)roundf((0.8*big_m)); i++)
  {
    if (verbose_flag)
    {
      res = hash_arr_insert(hashtab_arr, &names_array[i], big_m, strtoi_flag, 1);
      if (res == -1)
        printf("Could not insert A[%i] (%s), not enough space.\n", i, names_array[i].name);
      else
        printf("Inserted A[%i] (%s) into H[%lu].\n", i, names_array[i].name, res);
    }
    else
      hash_arr_insert(hashtab_arr, &names_array[i], big_m, strtoi_flag, 0);
  }

  printf("\n");
  printf("Searching for last 20 elements...\n");

  for (j=i=(int)(float)roundf((0.8*big_m))-1; i>j-20; i--)
  {
    res = hash_arr_search(hashtab_arr, names_array[i].name, big_m, strtoi_flag, 1);
    if (res == -1)
      printf("Could not find %s in hashtable.\n", names_array[i].name);
    else
      printf("Found %s (A[%i]) under H[%lu].\n", names_array[i].name, i, res);
  }

  /*
   * Freeing memory and closing file...
   */
  for (i=0; i<input_file_lc; i++)
    g_free(names_array[i].name);
  g_free(names_array);

  g_free(hashtab_arr);
  g_free(buf_str);
  g_free(input_filepath);

  fclose(input_file);
  return EXIT_SUCCESS;
}

gulong hash_func (gchar *key, gint i, gint m, gint strtoi_flag) {

  if (strtoi_flag == 1) {
    return (hash_strtoi_xor(key)+defaultHashFuncConstA*i + defaultHashFuncConstB*i*i)%m;
  }

  else if (strtoi_flag == 2){
    return (hash_strtoi_sum(key)+defaultHashFuncConstA*i + defaultHashFuncConstB*i*i)%m;
  }

  return (hash_strtoi_djb2(key)+defaultHashFuncConstA*i + defaultHashFuncConstB*i*i)%m;
}

glong hash_arr_insert (name_entry *hash_arr, name_entry *pos, gint m, gint strtoi_flag, gint test_flag) {
  gint cnt=1;
  gint i;
  gulong hash;

  for (i=0; i<m; i++, cnt++)
  {
    hash = hash_func(pos->name, i, m, strtoi_flag);
    if(!hash_arr[hash].name)
    {
      if (test_flag) {
        if (cnt == 1)
          printf("(1 attempt) ");
        else
          printf("(%i attempts) ", cnt);
      }
      *(hash_arr+hash)=*pos;
      return hash;
    }
  }
  printf("(%i attempts) ", cnt);
  return -1;
}
glong hash_arr_search (name_entry *hash_arr, gchar *key, gint m, gint strtoi_flag, gint test_flag) {
  gint cnt=1;
  gint i;
  gulong hash;

  for (i=0; i<m; i++, ++cnt)
  {
    hash = hash_func(key, i, m, strtoi_flag);
    if (!g_strcmp0(hash_arr[hash].name, key))
    {
      if (test_flag)
      {
        if (cnt == 1)
          printf("(1 attempt) ");
        else
          printf("(%i attempts) ", cnt);
      }

      return hash;
    }
    if (&hash_arr[hash] == NULL)
      break;
  }

  return -1;
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
