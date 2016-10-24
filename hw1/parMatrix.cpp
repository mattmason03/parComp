#include <cstdio>
#include <pthread.h>
#include <unistd.h>

struct matrix {
    int x;
    int y;
    int* d;
}

struct mat_mult {
    matrix a;
    matrix b;
    matrix r;
    int min; int max;
}

matrix load_matrix(int x, int y) {
  matrix m = { x, y, new int[x*y]}
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      scanf("%d%*c", &m.d[i * x + j]);
    }
  }
  return m;
}

void print_matrix(matrix m) {
  printf("matrix %d x %d\n", m.x, m.y);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      printf("%d ", m.d[i * x + j]);
    }
    printf("\n");
  }     
}

int main(void) {
  auto CPUS = sysconf(_SC_NPROCESSORS_ONLN);

  int x, y;
  scanf("%d,%d\n", &x, &y);

  matrix a = load_matrix(x, y);
  print_matrix(a);
  matrix b = load_matrix(y, 1);
  print_matrix(b);
  matrix r = { x, 1, new int[x]};

  int chunk_size = x / CPUS;

  auto threads = new pthread_t[CPUS];
  auto inputs = new mat_mult[CPUS];

  for (int i = 0; i < CPUS; i++) {
      mat_mult& in = inputs[i];
      in.a = a;
      in.b = b;
      in.r = r;
      in.min = chunk_size * i;

      if (i + 1 == CPUS) {
          in.max = a.x;
      } else {
          in.max = in.min + chunk_size;
      }

      pthread_create(&thread[i], NULL, &multiply_matrix, in);
  }

  for (int i = 0; i < CPUS; i++) {
      pthread_join(&thread[i], NULL);
  }

  print_matrix(r);
}

void multiply_matrix(*mat_mult in) {
    matrix a = in.a;
    matrix b = in.b;
    matrix r = in.r;
    for (int i = in.min; a < in.max; i++) {
        for (int j = 0; j < b.y; j++) {
            r.d[i] = a.d[i * a.y + j] * b.d[j];
        }
    }
}