#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>


enum encoding {
				UTF8, UTF16BE, UTF16LE, UTF32BE, UTF32LE, NONE
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
								return NONE;
}

/*
 * Devuelve el encoding correspondiente al byte order mark (BOM).
 */
static enum encoding bom_to_encoding(uint8_t *bom) {
				if (bom[0] == 0xFE && bom[1] == 0xFF)
								return UTF16BE;
				else if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF)
								return UTF32BE;
				else if (bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00)
								return UTF32LE;
				else if (bom[0] == 0xFF && bom[1] == 0xFE)
								return UTF16LE;

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
								return (n >= 4 || ((n >= 2) && ((buf[0] < 0xD8) || (buf[0] > 0xDB))));
				case UTF16LE:
								return (n >= 4 || ((n >= 2) && ((buf[1] < 0xD8) || (buf[1] > 0xDB))));
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
				uint32_t tmp = 0;

				// La función has_codepoint determina que se podrá decodificar
				// un codepoint en el encoding especificado. Así, adentro del
				// ciclo, en cada "case" se cumple la pre-condición que hay el
				// número adecuado de bytes presentes (no hace falta comprobarlo
				// de nuevo).
				while (has_codepoint(enc, &buf[b], *nbytes)) {
								uint32_t cp = 0;
								switch (enc) {
								case UTF32LE:
												cp |= (buf[b++] << 24);
												cp |= (buf[b++] << 16);
												cp |= (buf[b++] << 8);
												cp |= buf[b++];

												*nbytes -= 4;
												break;
								case UTF32BE:
												cp |= buf[b++];
												cp |= (buf[b++] << 8);
												cp |= (buf[b++] << 16);
												cp |= (buf[b++] << 24);

												*nbytes -= 4;
												break;
								case UTF8:
												fprintf(stderr, "buf[%i] vale %#x\n",b, buf[b]);
												fprintf(stderr, "buf[%i] vale %#x\n",b+1, buf[b+1]);

												if ( (buf[0] & 0x80) == 0 ) {
																tmp = (buf[b++] << 24) & 0xFF000000;
																//fprintf(stderr, "Aca1\n");
																//fprintf(stderr, "tmp vale %#x \n", tmp);
																*nbytes -= 1;
												} else if ( ((buf[0] & 0xC0) ==  0xC0) && ((buf[1] & 0x80) == 0x80) ) {
																tmp |= (buf[b++] & 0x3F) << 27;
																tmp |= (buf[b++] & 0x7F) << 21;
																fprintf(stderr, "Aca2\n");
																*nbytes -= 2;
												} else if ( ((buf[0] & 0x70) == 0x70) && ((buf[1] & 0x80) == 0x80) && ((buf[2] & 0x80) == 0x80)) {
																tmp |= (buf[b++] & 0x1F) << 28;
																tmp |= (buf[b++] & 0x7F) << 22;
																tmp |= (buf[b++] & 0x7F) << 16;
																//fprintf(stderr, "Aca3\n");

																*nbytes -= 3;
												} else if ( ((buf[0] & 0xF0) == 0xF0) && ((buf[1] & 0x80) == 0x80) && ((buf[2] & 0x80) == 0x80) && ((buf[3] & 0x80) == 0x80)) {
																tmp |= (buf[b++] & 0x15) << 29;
																tmp |= (buf[b++] & 0x7F) << 23;
																tmp |= (buf[b++] & 0x7F) << 17;
																tmp |= (buf[b++] & 0x7F) << 11;
																//	fprintf(stderr, "Aca3\n");

																*nbytes -= 4;
												}

												// tmp esta el cp en big endian, ahora nada mas tengo que guardar en cp lo mismo
												// pero en little endian
												fprintf(stderr, "tmp vale %#x \n", tmp);

												//cp |= tmp >> 24;
												//cp |= (tmp >> 16) & 0xFF0000;
												//cp |= (tmp >> 8) & 0x00FF;
												//cp |= tmp & 0xFF;

												cp = tmp;

												//fprintf(stderr, "cp vale %#x \n", cp);

												break;
								case UTF16BE:
												if (((0xD8 <= buf[b]) && (buf[b] <= 0xDB) &&  (0xDC <= buf[b+2]) && (buf[b+2] <= 0xDF))) {
																cp |= buf[b++];
																cp |= buf[b++]<< 8;
																cp &= 0xFF03;
																cp <<= 10;
																cp |= buf[b++];
																cp |= buf[b++]<<8;
																cp += 0x08000000;

																*nbytes -= 4;

												} else {
																cp |= buf[b++];
																cp |= buf[b++]<< 8;
																cp <<= 16;
																*nbytes -= 2;
												}

												break;
								case UTF16LE:
												if ( ((0xD8 <= buf[b+1]) && ( buf[b+1] <= 0xDB) &&  (0xDC <= buf[b+3]) && (buf[b+3] <= 0xDF)) ) {
																cp |= buf[b++] << 8;
																cp |= buf[b++];
																cp &= 0xFF03;
																cp <<= 10;
																cp |= buf[b++] << 8;
																cp |= buf[b++];
																cp &= 0xFF03;
																cp += 0x08000000;

																*nbytes -= 4;
												} else {
																cp |= buf[b++] << 8;
																cp |= buf[b++];
																cp <<= 16;
																*nbytes -= 2;

												}


												break;
								default:
												break;

								}

								destbuf[i++] = cp;
				}
				return i;
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
				int written_bytes = 0;
				uint32_t tmp = 0;

				int b = 0;

				// TODO: Si dest_enc no es UTF-8, hay que empezar a escribir 2 o 4 bytes mas adelante del comienzo del buff de salida
				if ((enc == UTF16BE) || (enc == UTF16LE) ) {
								b = 2;
				} else if ((enc == UTF32BE) || (enc == UTF32LE)) {
								b = 4;
				}

				for (int i=0; i < npoints; i++) {
								uint32_t cp = input[i];

								switch (enc) {
								case UTF32LE:
												outbuf[b++] = (cp >> 24) & 0xFF;
												outbuf[b++] = (cp >> 16) & 0xFF;
												outbuf[b++] = (cp >> 8) & 0xFF;
												outbuf[b++] = (cp & 0xFF);

												break;
								case UTF32BE:
												outbuf[b++] = cp & 0xFF;
												outbuf[b++] = (cp >> 8) & 0xFF;
												outbuf[b++] = (cp >> 16) & 0xFF;
												outbuf[b++] = (cp >> 24) & 0xFF;

												break;
								case UTF16BE:
												if (cp <= 0xFFFF0000) {
																outbuf[b++] = (cp >> 16) & 0xFF;
																outbuf[b++] = (cp >> 24) & 0xFF;
												} else {
																cp -= 0x10000;

												}

												break;
								case UTF16LE:
												if (cp <= 0xFFFF0000) {
																outbuf[b++] = (cp >> 24) & 0xFF;
																outbuf[b++] = (cp >> 16) & 0xFF;

												} else {
																cp -= 0x10000;


												}
												break;
								case UTF8:
												fprintf(stderr, "out_cp vale %#x \n", cp);

												//tmp |= (cp >> 24) & 0xFF;
												//tmp |= (cp >> 16) & 0xFF;
												//tmp |= (cp >> 8) & 0xFF;
												//tmp |= cp & 0xFF;


												if ( ((cp >> 24) & 0xFF) <= 0x7F) {
																tmp = (cp >> 24) & 0xFF;
																//fprintf(stderr, "tmp vale %#x\n", tmp);
																outbuf[b++] = tmp;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
												}
												else if (0x07 >= ((cp >> 16) & 0x00FF) && ((cp >> 24) & 0xFF) >= 0x80) {
																outbuf[b++] = ((cp >> 6) & 0x1F) | 0xC0;
																fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = (cp & 0x3F) | 0x80;
																fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
												}
												else if ( ((cp >> 16) & 0xFF) >= 0x08 && ((cp >> 16) & 0xFF) <= 0xFF) {
																outbuf[b++] = ((cp >> 12) & 0x0F) | 0xE0;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = ((cp >> 6) & 0x3F) | 0x80;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = (cp & 0x3F) | 0x80;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
												}
												else if (((cp >> 8) & 0xFF) >= 0x01 && ((cp >> 8) & 0xFF) <= 0x10) {
																outbuf[b++] = ((cp >> 18) & 0x07) | 0xF0;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = ((cp >> 12) & 0x0F) | 0x80;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = ((cp >> 6) & 0x3F) | 0x80;
																//fprintf(stderr, "outbuf[%i] vale %#x\n",b-1, outbuf[b-1]);
																outbuf[b++] = (cp & 0x3F) | 0x80;
												}



												break;

								default:
												break;
								}
								written_bytes = b;
				}
				return written_bytes;
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

				// Si dest_enc no es UTF-8, hay que escribir un BOM.
				if (dest_enc == UTF16BE) {
								outbuf[0] = 0xFE;
								outbuf[1] = 0xFF;
				} else if (dest_enc == UTF16LE) {
								outbuf[0] = 0xFF;
								outbuf[1] = 0xFE;
				} else if (dest_enc == UTF32BE) {
								outbuf[0] = outbuf[1] = 0x00;
								outbuf[2] = 0xFE;
								outbuf[3] = 0xFF;
				} else if (dest_enc == UTF32LE) {
								outbuf[0] = 0xFF;
								outbuf[1] = 0xFE;
								outbuf[2] = outbuf[3] = 0x00;
				}


				while ((inbytes = read(STDIN_FILENO, inbuf + prevbytes, sizeof(inbuf) - prevbytes)) > 0) {
								prevbytes += inbytes;
								// fprintf(stderr, "Processing: %zu bytes, ", prevbytes);

								npoints = orig_to_ucs4(orig_enc, inbuf, &prevbytes, ucs4);
								outbytes = ucs4_to_dest(dest_enc, ucs4, npoints, outbuf);
								//fprintf(stderr, "codepoints: %d, output: %d bytes, remaining: %zu bytes\n", //npoints, outbytes, prevbytes);

								write(STDOUT_FILENO, outbuf, outbytes);

								if (prevbytes > 0) {
												memcpy(inbuf, inbuf + outbytes, prevbytes);
								}
				}
}
