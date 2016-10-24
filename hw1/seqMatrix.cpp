#include <cstdio>

int main(void) {
  int x, y;
  scanf("%d,%d\n", &x, &y);

  int* mat = new int[x * y];
  int* vec = new int[x];
  int* res = new int[x];

  printf("matrix %d x %d\n", x, y);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      scanf("%d%*c", &mat[i * x + j]);
      printf("%d ", mat[i * x + j]);
    }
    printf("\n");
  }

  printf("vec %d x 1\n", x);
  for (int i = 0; i < x; i++) {
    scanf("%d%*c", &vec[i]);
    printf("%d\n", vec[i]);
  }

  printf("res %d x 1\n", x);
  for (int r = 0; r < x; r++) {
    res[r] = 0;
    for (int i = 0; i < x; i++) {
      res[r] += mat[r * x + i] * vec[i];
    }
    printf("%d\n", res[r]);
  }
}