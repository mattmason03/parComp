#include <cstdio>

struct matrix
{
  int x;
  int y;
  int *d;
};

struct mat_mult
{
  matrix a;
  matrix b;
  matrix r;
  int min;
  int max;
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

int main(int argc, char** argv) {
  FILE *in_file = fopen(argv[1], "r");
  FILE *out_file = fopen("seq.txt", "w+");

  int x, y;

  fscanf(in_file, "%d,%d\n", &x, &y);

  matrix a = load_matrix(in_file, x, y);
  matrix b = load_matrix(in_file, y, 1);

  matrix r = { x, 1, new int[x] };

  int* mat = a.d;
  int* vec = b.d;
  int* res = r.d;

  for (int r = 0; r < x; r++) {
    res[r] = 0;
    for (int i = 0; i < x; i++) {
      res[r] += mat[r * x + i] * vec[i];
    }
  }

  print_matrix(out_file, r);
}