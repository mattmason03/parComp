#include <cstdio>
#include <stdlib.h>

int main(int argc, char** argv) {
  int size = atoi(argv[1]);

  printf("%d,%d\n", size, size);

  for (int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      int num = rand() % 10;
      if (y + 1 == size)
        printf("%d\n", num);
      else 
        printf("%d,", num);
    }
  }

  for (int x = 0; x < size; x++) {
    printf("%d\n", rand() % 10);
  }
}