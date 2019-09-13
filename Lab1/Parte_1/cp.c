#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <sysexits.h>
#define BUFF_SIZE 10

int main(int argc, char const *argv[]) {
				/*
				   Chequeo que hayan pasado al menos 2 argumentos, source y destination
				   El primer argumento siempre es el comando de ejecucion del programa
				   El segundo argumento es el source y el tercero el destination
				 */
				if (argc < 3) {
								return EX_USAGE; // Codigo 64
				}

				int r_fd, w_fd;
				unsigned char buffer[BUFF_SIZE];
				int buff_read;

				r_fd = open(argv[1], O_RDONLY);
				if (r_fd < 0) {
								printf("No se pudo abrir fichero de origen.\n");
								printf("Errno : %i \n", errno);
								return EX_NOINPUT; // Codigo 66
				}

				w_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT,
				            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				if (w_fd < 0) {
								printf("No se pudo abrir fichero de destino\n");
								printf("Errno : %i \n", errno);
								return EX_CANTCREAT; // Codigo 73
				}

				while( ( (buff_read = read(r_fd, buffer, BUFF_SIZE)) > 0) )   {
								printf("Lei  %i bytes.\n", buff_read);

								int total_wrote = 0;

								while ((buff_read - total_wrote) > 0) {
												int buff_write = write(w_fd, buffer, buff_read);

												if (buff_write > 0) {
																total_wrote += buff_write;

																printf("Escribi %i bytes.\n", buff_write);
												}
								}
				}

				printf("Errno : %i\n", errno );

				return EX_OK;
}
