#include <cstdio>
#include <pthread.h>
#include <unistd.h>

struct matrix
{
  int x;
  int y;
  int *d;
};

struct mat_mult
{
  matrix a, b, r;
};

matrix load_matrix(FILE *in_file, int x, int y)
{
  matrix m = {x, y, new int[x * y]};
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < y; j++)
    {
      fscanf(in_file, "%d%*c", &m.d[i * y + j]);
    }
  }
  return m;
}

void print_matrix(FILE *out_file, matrix m)
{
  int x = m.x;
  int y = m.y;
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < y; j++)
    {
      fprintf(out_file, "%d", m.d[i * y + j]);
    }
    fprintf(out_file, "\n");
  }
}


void *multiply_matrix(void *input)
{
  mat_mult in = *((mat_mult *)input);
  matrix a = in.a;
  matrix b = in.b;
  matrix r = in.r;
  for (int i = 0; i < a.x; i++)
  {
    int temp = 0;
    for (int j = 0; j < a.y; j++)
    {
      temp += a.d[i * a.y + j] * b.d[j];
    }
    r.d[i] = temp;
  }
}

int main(int argc, char** argv) {
  FILE *in_file = fopen(argv[1], "r");
  FILE *out_file = fopen("par.txt", "w+");

  int CPUS = sysconf(_SC_NPROCESSORS_ONLN);

  int x, y;
  fscanf(in_file, "%d,%d\n", &x, &y);

  matrix a = load_matrix(in_file, x, y);
  matrix b = load_matrix(in_file, y, 1);

  int rows_per_thread = x / CPUS;

  pthread_t *threads = new pthread_t[CPUS];
  mat_mult *inputs = new mat_mult[CPUS];

  matrix r = { x, 1, new int[x] };

  int array_step = rows_per_thread * a.y;

  for (int i = 0; i < CPUS; i++)
  {
    int rows = rows_per_thread;
    if (i + 1 == CPUS) {
      rows = a.x - rows_per_thread * i;
    }

    mat_mult &in = inputs[i];
    in.a = { rows, a.y, a.d + array_step * i };
    in.b = b;
    in.r = { rows, 1, r.d + rows_per_thread * i };

    pthread_create(&threads[i], NULL, &multiply_matrix, &in);
  }

  for (int i = 0; i < CPUS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  print_matrix(out_file, r);
}