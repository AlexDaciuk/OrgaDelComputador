#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>



int main(int argc, char const *argv[]) {
				/* Aprovecho PATH_MAX que esta en limits.h */
				char cwd[PATH_MAX];

				/* Chequeo cantidad de argumentos */
				switch ( argc ) {
				case 1:
								getcwd(cwd, sizeof(cwd));
								break;
				case 2:
								strcpy(cwd,argv[1]);
								break;
				default:
								return EX_USAGE;
				}

				printf("El directorio es %s \n\n", cwd);

				struct dirent *dp;
				int dir_fd;
				struct stat fileStat;
				DIR *dir = opendir(cwd);
				dir_fd = dirfd(dir);

				while ( (dp = readdir(dir)) ) {
								if (fstatat(dir_fd, dp->d_name, &fileStat, 0) != -1 &&
								    (dp->d_name[0] != '.')) {
												if (fileStat.st_mode & S_IFDIR) {
																printf("%s\n", dp->d_name);
												} else if (fileStat.st_mode & S_IFREG) {
																printf("%s \t %li\n", dp->d_name, fileStat.st_size);
												}
								}
				}

				return EX_OK;
}
