#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

enum encoding {
				UTF8, UTF16BE, UTF16LE, UTF32BE, UTF32LE,
};

/*
 * Devuelve el encoding correspondiente al argumento.
 */
static enum encoding str_to_encoding(const char *enc) {
				if (strcmp(enc, "UTF-8") == 0)
								return UTF8;
				else if (strcmp(enc, "UTF-16BE") == 0)
								return UTF16BE;
				else if (strcmp(enc, "UTF-16LE") == 0)
								return UTF16LE;
				else if (strcmp(enc, "UTF-32BE") == 0)
								return UTF32BE;
				else if (strcmp(enc, "UTF-32LE") == 0)
								return UTF32LE;
				else
								return -1;
}

/*
 * Devuelve el encoding correspondiente al byte order mark (BOM).
 */
static enum encoding bom_to_encoding(uint8_t *bom) {
				if (bom[0] == 0xFE && bom[1] == 0xFF) {
								return UTF16BE;
				} else if (bom[0] == 0xFF && bom[1] == 0xFE) {
								return UTF16LE;
				} else if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF) {
								return UTF32BE;
				} else if (bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00) {
								return UTF32LE;
				}

				return UTF8;
}

/*
 * Devuelve verdadero si hay un codepoint codificado en buf.
 */
static bool has_codepoint(enum encoding enc, uint8_t *buf, size_t n) {
				switch (enc) {
				case UTF32BE:
				case UTF32LE:
								return n >= 4;
				case UTF16BE:
								// TODO: manejar surrogates.
								return (n >= 4 ||
								        (n >= 2 && 1 /* buf[0] NO es un high surrogate */));
				case UTF16LE:
								// TODO: manejar surrogates.
								return (n >= 4 ||
								        (n >= 2 && 1 /* buf[1] NO es un high surrogate */));
				case UTF8:
								return (n >= 4 ||
								        (n >= 3 && buf[0] <= 0xEF) ||
								        (n >= 2 && buf[0] <= 0xDF) ||
								        (n >= 1 && buf[0] <= 0x7F));
				default:
								return false;
				}
}


/*
 * Transforma una codificación a UCS-4.
 *
 * Argumentos:
 *
 *   - enc: el encoding original.
 *   - buf: los bytes originales.
 *   - nbytes: número de bytes disponibles en buf.
 *   - destbuf: resultado de la conversión.
 *
 * Devuelve: el número de CODEPOINTS obtenidos (número
 *           de elementos escritos en destbuf).
 *
 * Actualiza: nbytes contiene el número de bytes que no se
 *            pudieron consumir (secuencia o surrgate incompleto).
 *
 * Se debe garantiza que "destbuf" puede albergar al menos nbytes
 * elementos (caso enc=UTF-8, buf=ASCII).
 */
int orig_to_ucs4(enum encoding enc, uint8_t *buf, size_t *nbytes, uint32_t *destbuf) {
				int i = 0, b = 0;

				// La función has_codepoint determina que se podrá decodificar
				// un codepoint en el encoding especificado. Así, adentro del
				// ciclo, en cada "case" se cumple la pre-condición que hay el
				// número adecuado de bytes presentes (no hace falta comprobarlo
				// de nuevo).
				while (has_codepoint(enc, &buf[b], *nbytes)) {
								uint32_t cp = 0;
								switch (enc) {
								case UTF32LE:
												cp |= buf[b++];
												// ...
												*nbytes -= 4;
												break;
								case UTF32BE:
												cp |= buf[b++] << 24;
												// ...
												*nbytes -= 4;
												break;
								case UTF8:
												// TODO: Implementar los cuatro casos posibles de UTF-8.
												break;
								case UTF16BE:
												// TODO: Implementar primero rango BMP (0x0000-0xFFFF).
												// TODO: Implementar después comprobando surrogates.
												break;
								case UTF16LE:
												// TODO: Ídem.
												break;
								}

								destbuf[i++] = cp;
				}
				return 0; // TODO: devolver número de codepoints.
}

/*
 * Transforma UCS-4 a la codificación deseada.
 *
 * Argumentos:
 *
 *   - enc: el encoding destino.
 *   - input: los codepoints a codificar.
 *   - npoints: el número de codepoints en input.
 *   - output: resultado de la conversión.
 *
 * Devuelve: el número de BYTES obtenidos (número
 *           de elementos escritos en destbuf).
 *
 * Se debe garantiza que "destbuf" puede albergar al menos npoints*4
 * elementos, o bytes (caso enc=UTF-32).
 */
int ucs4_to_dest(enum encoding enc, uint32_t *input, int npoints, uint8_t *outbuf) {
				// TODO: Implementar.
				for (int i=0, b=0; i < npoints; i++) {
								uint32_t cp = input[i];
								switch (enc) {
								case UTF32LE:
												outbuf[b++] = cp & 0xFF;
												// ...
												break;
								case UTF32BE:
												outbuf[b++] = (cp >> 24) & 0xFF;
												// ...
												break;
								}
				}
				return 0; // TODO: devolver número de bytes escritos.
}


int main(int argc, char *argv[]) {
				enum encoding orig_enc, dest_enc;

				if (argc != 2) {
								fprintf(stderr, "Uso: ./recode57 <encoding>\n");
								return 1;
				}

				if ((dest_enc = str_to_encoding(argv[1])) < 0) {
								fprintf(stderr, "Encoding no válido: %s\n", argv[1]);
								return 1;
				}

				// Detectar codificación origen con byte order mark.
				uint8_t bom[4];

				read(STDIN_FILENO, bom, 4);
				orig_enc = bom_to_encoding(bom);

				// En cada iteración, leer hasta 1024 bytes, convertirlos a UCS-4
				// (equivalente a UTF32-LE con enteros nativos) y convertirlo por
				// salida estándar.
				uint8_t inbuf[1024];
				uint8_t outbuf[1024*4];
				uint32_t ucs4[1024];
				size_t prevbytes = 0;
				ssize_t inbytes;
				int npoints, outbytes;

				// Si orig_enc no fue UTF-32, quedaron 2 o 4 bytes en "bom" que
				// deben ser prefijados en inbuf.
				if (orig_enc == UTF8) {
								memcpy(inbuf, bom, 4);
								prevbytes = 4;
				} else if (orig_enc == UTF16BE || orig_enc == UTF16LE) {
								memcpy(inbuf, bom + 2, 2);
								prevbytes = 2;
				}

				// TODO: Si dest_enc no es UTF-8, hay que escribir un BOM.

				while ((inbytes = read(STDIN_FILENO, inbuf + prevbytes, sizeof(inbuf) - prevbytes)) > 0) {
								prevbytes += inbytes;
								// fprintf(stderr, "Processing: %zu bytes, ", prevbytes);

								npoints = orig_to_ucs4(orig_enc, inbuf, &prevbytes, ucs4);
								outbytes = ucs4_to_dest(dest_enc, ucs4, npoints, outbuf);
								// fprintf(stderr, "codepoints: %d, output: %d bytes, remaining: %zu bytes\n",
								//         npoints, outbytes, prevbytes);

								write(STDOUT_FILENO, outbuf, outbytes);

								if (prevbytes > 0) {
												// TODO: Se deben mover al incio de inbuf los bytes que
												// quedaron sin procesar al final. (Ver memcpy arriba.)
								}
				}
}
