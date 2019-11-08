# Orga del Computador

## Nombre : Alexis Daciuk
## Padron : 97630



### x86-write

1) ¿Por qué se le resta 1 al resultado de sizeof?

C por convencion, los strings terminan en **\0** para indicar fin de cadena, en este caso, se le resta 1 para que **write** no lea ese ultimo caracter, ya que no es un caracter que queremos que se imprima.

2) ¿Funcionaría el programa si se declarase msg como **const char *msg = "...";**? ¿Por qué?

Si, ya que el puntero seria el mismo, en C, los strings son arrays de caracteres y la variable asignada no es mas que un puntero que apunta al primer caracter, para la funcion write, las 2 definiciones son un puntero que apunta a un pedazo de memoria de n bytes definidos por el tercer argumento.


3) Explicar el propósito de cada instrucción, y cómo se corresponde con el código C original.

```C
#include <unistd.h> // (C.1)

const char msg[] = "Hello, world!\n"; // (C.2)

int main(void) { // (C.3)
    write(1, msg, sizeof msg - 1); // (C.4)
    _exit(7); // (C.5)
}
```

```
(A.1)  .globl main
(A.2)  main:
             // Call convention: arguments on the stack (reverse order).
(A.3)          push $len
(A.4)          push $msg
(A.5)          push $1

            // No declaration needed; asm assumes symbols always exist.
(A.6)          call write

(A.7)         push $7
(A.8)         call _exit

(A.9)  .data
(A.10) msg:
(A.11)        .ascii "Hello, world!\n"

(A.12) .set len, . - msg
```

**(A.1) :**   

**(A.2) :**

**(A.3) :** Se carga **len** a la pila, corresponde con linea **(C.4)**.

**(A.4) :**

**(A.5) :**  

**(A.9) :** En la seccion **data** se declaran variables y constantes, son datos que no cambian en tiempo de ejecucion.

**(A.10) :** Se declara la etiqueta **msg**

**(A.11) :** Define que **msg** es un string de tipo ASCII, en ASM un string ASCII no termina con **\0** o Null Byte, se corresponde con la linea de C.

**(A.12) :** En esta linea, se asigna el valor de **. - msg** a la etiqueta **len**, se corresponde con el **sizeof** de la linea **(C.4)**.



4) Examinar, con objdump -S libc_hello, el código máquina generado e indicar el valor de len en la primera instrucción push. Explicar el efecto del operador . en la línea .set len, . - msg


El operador punto (**.**) se refiere a la direccion de memoria actual, entonces **. - msg** es la direccion de memoria actual menos el valor del label **msg**.
