#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  if (argc > 1) {
    return 22;
  }

  /* Tengo en cuenta el mayor largo posible en ext4 */
  char cwd[4096];
  getcwd(cwd, sizeof(cwd));

  printf("El directorio es %s\n", cwd);


  struct dirent *dp;
  DIR *dir = opendir(cwd);

  while (dp = readdir(dir)){
    printf("%s \n", dp->d_name);
  }








}
