#!/bin/bash

# Si se invoca el script sin ningún parámetro:
#
#     $ ./test_recode.sh
#
# se imprime un resumen de todos los tests.
#
# Con el argumento --diff:
#
#     $ ./test_recode.sh --diff
#
# se muestran las diferencias en hexadecimal entre lo obtenido y lo
# esperado.
#
# Notas: para cada codificación, se comprueba también la conversión de
# esa codificación, a la *misma* codificación. Esto sirve para depurar
# cada codificación por separado, cuando no está claro en cuál está el
# error.
#
# Si se añade una regla "recode57" al Makefile, se puede compilar y
# correr en una instrucción con:
#
#     $ make -s recode57 && ./test_recode.sh
#
# Se pueden comentar las llamadas a "test_type" al final del archivo
# para controlar qué categoría de archivos probar.

set -eu

DIFF="${1-}"
ENCODINGS="UTF-8 UTF-16BE UTF-16LE UTF-32BE UTF-32LE"

# Esta variable ignora no incluye UTF-8; descomentar probar solo
# las codificaciones de ancho fijo.
# ENCODINGS="UTF-32LE UTF-32BE UTF-16LE UTF-16BE"

cmp_output() {
    local type="$1"
    local orig_enc="$2"
    local dest_enc="$3"

    ./recode57 "$dest_enc" <"${type}_${orig_enc}.txt" |
        cmp -s "${type}_${dest_enc}.txt" -
}

diff_hex() {
    local type="$1"
    local orig_enc="$2"
    local dest_enc="$3"

    diff -u --label esperado --label obtenido      \
        <(od -An -t x1 <"${type}_${dest_enc}.txt") \
        <(./recode57 "$3" <"${type}_${orig_enc}.txt" | od -An -t x1) || true
}

test_type() {
    local type="$1"
    echo "=== TEST: '$type' ==="

    for src in $ENCODINGS; do
        for dst in $ENCODINGS; do
            # if [[ $src != $dst ]]; then
                echo -n "$src -> $dst: "
                if cmp_output "$type" "$src" "$dst"; then
                    echo -e "\e[92mOK\e[0m"
                else
                    echo -e "\e[91mFAIL\e[0m"
                    if [[ $DIFF == "--diff" ]]; then
                        diff_hex "$type" "$src" "$dst"
                    fi
                fi
            # fi
        done
    done

    echo
}

test_type "simple"
test_type "bmp"
test_type "surrogate"
