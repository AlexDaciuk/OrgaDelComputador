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
#define DEF_PERM S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

int main(int argc, char const *argv[]) {
				/*
				   Chequeo que hayan pasado al menos 2 argumentos, primer argumento siempre es el comando
				   de ejecucion del programa, el segundo es el source y el tercero el destination.
				 */
				if (argc != 3) {
								fprintf(stderr,"Cantidad de argumentos incorrecta, se necesitan 2 argumentos para utilizar este programa.\n");
								return EX_USAGE; // Codigo 64
				}

				int r_fd, w_fd;
				unsigned char buffer[BUFF_SIZE];
				int buff_read;

				while ( (r_fd = open(argv[1], O_RDONLY)) < 0 ) {
								if (errno == 21) {
												fprintf(stderr, "El archivo es un directorio.\n");
												return EX_NOINPUT; // Codigo 66
								} else if (errno == 4) { // EINTR
												fprintf(stderr,"No se pudo abrir fichero de origen, probando de nuevo");

								} else {
												perror("Error: ");
												return EX_NOINPUT; // Codigo 66
								}
				}

				// Abro el archivo de destino, si no existe, lo creo con permisos 0666
				// si existe, lo trunco
				if ( (w_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, DEF_PERM)) < 0 ) {
								fprintf(stderr,"No se pudo abrir fichero de destino, error: %m\n");
								return EX_CANTCREAT; // Codigo 73
				}

				while( (buff_read = read(r_fd, buffer, BUFF_SIZE)) )   {
								if (errno == 0) {
												printf("Lei  %i bytes.\n", buff_read);

												int total_wrote = 0;

												while ((buff_read - total_wrote) > 0) {
																int buff_write = write(w_fd, buffer, buff_read);

																if (buff_write > 0) {
																				total_wrote += buff_write;

																				printf("Escribi %i bytes.\n", buff_write);
																}
												}
								} else if (errno == 4) { // EINTR
												fprintf(stderr,"No se pudo abrir fichero de origen, probando de nuevo");
								} else if (errno == 21) {
												fprintf(stderr, "El archivo es un directorio.\n");
												return EX_NOINPUT; // Codigo 66
								} else {
												perror("Error: ");
								}

				}

				if (errno != 0)
								printf("Errno : %i\n", errno );

				return EX_OK;
}
