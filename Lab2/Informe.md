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

(A.7)          push $7
(A.8)          call _exit

(A.9)  .data
(A.10) msg:
(A.11)        .ascii "Hello, world!\n"

(A.12) .set len, . - msg
```

**(A.1) :** La directiva **.global** hace que la etiqueta a la cual se refiere sea visible para el linkeador, en este caso, **main**.

**(A.2) :** Se define la etiqueta **main** se corresponde con la linea **(C.3)** (?), esta etiqueta tiene que ser **main** si lo compila GCC, sino, suele ser **_start**.

**(A.3) :** Se carga **len** a la pila, corresponde con linea **(C.4)**.

**(A.4) :** Se carga **msg** a la pila, corresponde con linea **(C.4)**.

**(A.5) :** Se carga **1** a la pila, corresponde con linea **(C.4)**.

**(A.6) :** Llamada a la syscall **write**

**(A.7) :** Imagino que en $7 esta la direccion a donde tiene que volver el programa y es lo que usa exit, pero no estoy seguro por ahora.

**(A.8) :** Llamada a la syscall **exit**.

**(A.9) :** En la seccion **data** se declaran variables y constantes, son datos que no cambian en tiempo de ejecucion.

**(A.10) :** Se declara la etiqueta **msg**, esta etiqueta apunta a la direccion de memoria del primer byte de la cadena.

**(A.11) :** Define que **msg** es un string de tipo ASCII, en ASM un string ASCII no termina con **\0** o Null Byte, se corresponde con la linea de C.

**(A.12) :** En esta linea, se asigna el valor de **. - msg** a la etiqueta **len**, se corresponde con el **sizeof** de la linea **(C.4)**.



4) Examinar, con objdump -S libc_hello, el código máquina generado e indicar el valor de len en la primera instrucción push. Explicar el efecto del operador . en la línea .set len, . - msg

**len** vale **0xE** o 14 en decimal, que es el largo en bytes de la cadena **Hello, world!\n**, con el Null Byte.

El operador punto (**.**) se refiere a la direccion de memoria actual, entonces **. - msg** es la direccion de memoria actual menos el valor del label **msg**.

5) Mostrar un hex dump de la salida del programa en assembler.

```
0000000  48  65  6c  6c  6f  2c  20  77  6f  72  6c  64  21  0a
          H   e   l   l   o   ,       w   o   r   l   d   !  \n
0000016
```

6) Cambiar la directiva .ascii por .asciz y mostrar el hex dump resultante con el nuevo código. ¿Qué está ocurriendo? ¿Qué ocurre con el valor de len?

```
0000000  48  65  6c  6c  6f  2c  20  77  6f  72  6c  64  21  0a  00
          H   e   l   l   o   ,       w   o   r   l   d   !  \n  \0
0000017
```

La diferencia entre **.ascii** y **.asciz** en assembly es que **.asciz** termina los strings con 0, explicitamente, entonces ahora se esta imprimiendo ese 0 final.

**len** ahora vale 15, ya que ahora el string tambien tiene un 0 al final.

### x86-call

1) Mostrar en una sesión de GDB cómo imprimir las mismas instrucciones usando la directiva x $pc y el modificador adecuado.

```
(gdb) x/7iw $pc
=> 0x8049186 <main>:    push   $0xf
   0x804918b <main+5>:  push   $0x804c020
   0x8049190 <main+10>: push   $0x1
   0x8049192 <main+12>: call   0x8049060 <write@plt>
   0x8049197 <main+17>: push   $0x7
   0x8049199 <main+19>: call   0x8049040 <_exit@plt>
   0x804919e <main+24>: xchg   %ax,%ax
```


2)  Después, usar el comando stepi (step instruction) para avanzar la ejecución hasta la llamada a write. En ese momento, mostrar los primeros cuatro valores de la pila justo antes e inmediatamente después de ejecutar la instrucción call, y explicar cada uno de ellos.
