#include <stdio.h>
#include <sysexits.h>
#include <string.h>
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

        FILE *sptr, *dptr;
        unsigned char buffer[BUFF_SIZE];
        int buff_read = 0;

        sptr = fopen(argv[1], "rb");
        if (!sptr) {
                printf("No se pudo abrir fichero de origen.\n");
                return EX_NOINPUT; // Codigo 66
        }

        dptr = fopen(argv[2], "wb");
        if (!dptr) {
                printf("No se pudo abrir fichero de destino\n");
                return EX_CANTCREAT; // Codigo 73
        }

        while( (buff_read = fread(buffer, sizeof(char), 10, sptr)) ) {
                printf("Lei %i paquetes de %i bytes del archivo.\n", buff_read, BUFF_SIZE);

                int buff_write = fwrite(buffer, sizeof(char), buff_read, dptr);
                printf("Escribi %i paquetes de %i bytes al archivo.\n", buff_write, BUFF_SIZE );
        }

        return EX_OK;
}
