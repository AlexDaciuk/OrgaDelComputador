#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/limits.h>

int main(int argc, char const *argv[]) {
  /* Chequeo cantidad de argumentos */
  if (argc > 1) {
    return 22;
  }

  /* Aprovecho PATH_MAX que esta en limits.h */
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));

  printf("El directorio es %s\n \n", cwd);

  struct dirent *dp;
  DIR *dir = opendir(cwd);

  while ( (dp = readdir(dir)) ){
    printf("%s  \n", dp->d_name);
  }

  return 0;
}
