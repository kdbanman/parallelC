/* 
 * Kirby Banman
 * ECE 420 Lab 1
 * 15 Jan 2015
 *
 * File:  
 *    matr.c
 *
 * Purpose:
 *    Using the desired number of threads, compute the product of two matrices
 *    
 * Input:
 *    "data_input" file, 2 command line parameters (see Usage below)
 * Output:
 *    "data_output" file
 *
 * Compile:  gcc -g -Wall -Wextra -o matr matr.c -lpthread -lm
 * Usage:    ./matr <thread_count> <matrix_size>
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <math.h>

const int MAX_THREADS = 4096;
const int MAX_SIZE = 100000;

/* Global variables:  accessible to all threads */

int thread_count;  
int *A, *B, *C; /* Input and output matrices. */
int size; /* Matrix size. */
int part_size; /* Partition square size for each thread. */

/* Method declarations. */

void partition_mult(int *A, int *B, int *C, int i, int j, int p_size);
int dot_product(int *A, int *B, int row, int col);
int coord(int i, int j);
int is_perf_square(int x);
void *thread_func(void* rank);
int load_input(int *A, int *B, int *n);
int save_output(int *C, int *n);
void usage(char* prog_name);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  long       thread;
  pthread_t* thread_handles; 
  int        part_size;

  /* Get number of threads and matrix size from command line */
  if (argc != 3) usage(argv[0]);

  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) usage(argv[0]);

  size = atoi(argv[2]);
  if (size <= 1 || size > MAX_THREADS) usage(argv[0]);

  /* Ensure number of threads evenly divides the of matrix size.
   * Number of threads must also be a perfect square. */
  if (!is_perf_square(thread_count) || (size * size) % thread_count != 0)
    usage(argv[0]);

  /* Calculate partition size for each thread. */
  part_size = size / (int) sqrt(thread_count);

  /* Initialize matrices */
  A = malloc(size * size * sizeof(int));
  B = malloc(size * size * sizeof(int));
  C = malloc(size * size * sizeof(int));

  /* Load contents of input file into input matrices. */
  load_input(A, B, &size);

  /* Initialize threads */
  thread_handles = malloc (thread_count*sizeof(pthread_t)); 

  /* Send work to threads. */
  for (thread = 0; thread < thread_count; thread++)  
    pthread_create(&thread_handles[thread], NULL, thread_func, (void*) thread);  

  printf("All threads dispatched.\n");

  /* Synchronize threads. */
  for (thread = 0; thread < thread_count; thread++) 
    pthread_join(thread_handles[thread], NULL); 

  /* DEBUG */
  partition_mult(A, B, C, 0, 0, size);

  /* Save output */
  save_output(C, &size);

  /* Clean up memory. */
  free(thread_handles);
  free(A);
  free(B);
  free(C);

  return 0;
}  /* main */

/*-------------------------------------------------------------------*/
void *thread_func(void* rank)
{
  long my_rank = (long) rank;  /* Use long in case of 64-bit system */ 

  /* TODO Calculate coordinate from rank, matrix size, and thread count. */

  /* TODO Call partition multiplier using global matrices and partition size. */

  printf("Hello from thread %ld of %d\n", my_rank, thread_count);

  return NULL;
}  /* thread_func */

/*-------------------------------------------------------------------*/
void partition_mult(int *A, int *B, int *C, int i, int j, int p_size)
{
  int a, b;

  for (a = i; a < i + p_size; a++) {
    for (b = j; b < j + p_size; b++) {
      C[coord(a,b)] = dot_product(A, B, a, b);
    }
  }
}

/*-------------------------------------------------------------------*/
int dot_product(int *A, int *B, int row, int col)
{
  int i, sum;

  sum = 0;
  for (i = 0; i < size; i++) {
    sum += A[coord(row, i)] * B[coord(i, col)];
  }

  return sum;
}

/*-------------------------------------------------------------------*/
int coord(int i, int j)
{
  return i * size + j;
}

/*-------------------------------------------------------------------*/
int is_perf_square(int x) {
  int root = (int) sqrt((double) x);
  return root * root == x;
}

/*-------------------------------------------------------------------*/
int load_input(int *A, int *B, int *n)
{
  /*
    The function to load the data generated by matrixgen.c

    The input are the pointer to the two matrices and the pointer 
    to the variable storing the matrix size.
    MAKE SURE that you declared two array with the exact SAME 
    size of the matrix you generated with matrixgen.c
    
    Note that the input is of type int *. If you declared arrays 
    with type int *[], you need to CONVERT to type int*. For example
    if you declared some array like
    int A_[16][16],B_[16][16];
    int n_;
    To call the function, you can use
    load_input(*A_, *B_, &n_);

    Also note that the third argument is a pointer!
  */

        FILE* ip;
        int i,j;
        if ((ip=fopen("data_input","r"))==NULL)
        {
                printf("error opening the input data.\n");
                return 1;
        }
        fscanf(ip,"%d\n",n);
        for (i=0;i<*n;i++)
                for (j=0;j<*n;j++)
                        fscanf(ip,"%d\t",A+*n*i+j);     
        for (i=0;i<*n;i++)
                for (j=0;j<*n;j++)
                        fscanf(ip,"%d\t",B+*n*i+j);
  fclose(ip);     
        return 0;
}

/*-------------------------------------------------------------------*/
int save_output(int *C, int *n)
{  
  /*
    The function to save the result matrix to a file.

    The input are the pointer to the result matrix and the pointer 
    to the variable storing the matrix size.
    MAKE SURE that you declared the result array with the exact SAME 
    size of the matrix you generated with matrixgen.c
    
    Note that the input is of type int *. If you declared arrays 
    with type int *[], you need to CONVERT to type int*. For example
    if you declared some array like
    int C_[16][16];
    int n_;
    To call the function, you can use
    load_input(*C_, &n_);

    Also note that the second argument is a pointer!

    The output will be stored at the file 
    ./data_output
  */
  FILE* op;
  int i,j;
  if ((op=fopen("data_output","w"))==NULL)
  {
    printf("Error opening the output file.\n");
    return 1;
  }
  fprintf(op,"%d\n\n",*n);
  for (i=0;i<*n;i++)
  {
    for (j=0;j<*n;j++)
      fprintf(op,"%d\t",C[*n*i+j]);
    fprintf(op,"\n");
  }
  fclose(op);
  return 0;  
}

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <number of threads> <matrix size>\n", prog_name);
   fprintf(stderr, "  0 < number of threads <= %d\n", MAX_THREADS);
   fprintf(stderr, "  0 < matrix size <= %d\n", MAX_SIZE);
   fprintf(stderr, "  number of threads must be a perfect square\n");
   fprintf(stderr, "  number of threads must evenly divide square of size\n");
   exit(0);
}  /* usage */

