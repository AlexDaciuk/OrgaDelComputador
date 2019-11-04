# Orga del Computador

## Nombre : Alexis Daciuk
## Padron : 97630



### x86-write

- ¿Por qué se le resta 1 al resultado de sizeof?

C por convencion, los strings terminan en **\0** para indicar fin de cadena, en este caso, se le resta 1 para que **write** no lea ese ultimo caracter, ya que no es un caracter que queremos que se imprima.

- ¿Funcionaría el programa si se declarase msg como **const char *msg = "...";**? ¿Por qué?

Si, ya que el string seria el mismo, en C, los strings son arrays de caracteres y la variable asignada no es mas que un puntero que apunta al primer caracter, para la funcion write, las 2 definiciones son un puntero que apunta a un pedazo de memoria de n bytes definidos por el tercer argumento.


Explicar el propósito de cada instrucción, y cómo se corresponde con el código C original.

```C
.globl main
main:
        // Call convention: arguments on the stack (reverse order).
        push $len
        push $msg
        push $1

        // No declaration needed; asm assumes symbols always exist.
        call write

        push $7
        call _exit

.data
msg:
        .ascii "Hello, world!\n"

.set len, . - msg
```
