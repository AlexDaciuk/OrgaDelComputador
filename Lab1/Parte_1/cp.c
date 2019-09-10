#include <stdio.h>
#include <sysexits.h>
#ifndef BUFF_SIZE
#define BUFF_SIZE 10

int main(int argc, char const *argv[]) {
        /*
           Chequeo que hayan pasado al menos 2 argumentos, source y destination
           El primer argumento siempre es el comando de ejecucion del programa
           El segundo argumento es el source y el tercero el destination
         */
        if (argc < 3) {
                return EX_USAGE;
        }

        FILE *sptr, *dptr;
        unsigned char buffer[BUFF_SIZE];

        sptr = fopen(argv[1], "rb");
        if (!sptr) {
                printf("No se pudo abrir fichero de origen.\n");
                return EX_NOINPUT;
        }

        dptr = fopen(argv[2], "wb");
        if (!dptr) {
                printf("No se pudo abrir fichero de destino\n");
                return EX_CANTCREAT;
        }

        while(!feof(sptr)) {
                /* Trato de leer 10 bytes */
                int buff_read = fread(buffer, sizeof(buffer), 1, sptr);
        }
        return EX_OK;
}
