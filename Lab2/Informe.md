# Orga del Computador

## Nombre : Alexis Daciuk
## Padron : 97630

### Versiones
**GCC** : 9.2.0  
**make** : 4.2.1  
**gdb** : 8.3.1  
**binutils** : 2.33.1 (objdump, nm)  
**coreutils** : 8.31 (od)



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

```
(gdb) stepi
5               push $msg
(gdb) stepi
6               push $1
(gdb) stepi
9               call write
[1] (gdb) x/4x $esp
0xffffd120:     0x00000001      0x0804c020      0x0000000f      0xf7dc5fa9
(gdb) si
0x08049060 in write@plt ()
[2] (gdb)  x/4x $esp
0xffffd11c:     0x08049197      0x00000001      0x0804c020      0x0000000f


```

En **[1]** vemos 4 valores del stack, siendo los primeros 3 los correspondientes a las lineas **push   $0xf** , **push   $0x804c020** y **$0x1** del codigo assembly del archivo **libc_hello.S**, en orden inverso por ser un stack y el ultimo valor **0xf7dc5fa9** apunta al rango de memoria donde estan cargadas las funciones de libc(1).

En **[2]** vemos 4 valores del stack, siendo el primero **0x08049197** la posicion de memoria de **push   $0x7**, que es donde tiene que volver la ejeucion del programa despues de todo lo que tiene que hacer la syscall **write**

(1) Obs: Esto se puede constatar viendo los maps asociados al PID mediante **cat /proc/\<pid\>/maps**

```
alexarch:~/ $ sudo cat /proc/6324/maps                            
08048000-0804b000 r-xp 00000000 fe:01 502744      /home/alexarch/Source_Code/OrgaDelComputador/Lab2/libc_hello
0804b000-0804c000 r-xp 00002000 fe:01 502744      /home/alexarch/Source_Code/OrgaDelComputador/Lab2/libc_hello
0804c000-0804d000 rwxp 00003000 fe:01 502744      /home/alexarch/Source_Code/OrgaDelComputador/Lab2/libc_hello
f7da7000-f7f88000 r-xp 00000000 fe:00 773098      /usr/lib32/libc-2.30.so
f7f88000-f7f8a000 r-xp 001e0000 fe:00 773098      /usr/lib32/libc-2.30.so
f7f8a000-f7f8c000 rwxp 001e2000 fe:00 773098      /usr/lib32/libc-2.30.so
f7f8c000-f7f90000 rwxp 00000000 00:00 0
f7fce000-f7fd1000 r--p 00000000 00:00 0           [vvar]
f7fd1000-f7fd3000 r-xp 00000000 00:00 0           [vdso]
f7fd3000-f7ffc000 r-xp 00000000 fe:00 773087      /usr/lib32/ld-2.30.so
f7ffc000-f7ffd000 r-xp 00028000 fe:00 773087      /usr/lib32/ld-2.30.so
f7ffd000-f7ffe000 rwxp 00029000 fe:00 773087      /usr/lib32/ld-2.30.so
fffdd000-ffffe000 rwxp 00000000 00:00 0           [stack]
```


### x86-lib

1) Compilar y ejecutar el archivo completo int80_hi.S. Mostrar la salida de nm --undefined para este nuevo binario.

```
alexarch:Lab2/ (master*) $ nm --undefined int80_hi                                                  
         w __gmon_start__
         w _ITM_deregisterTMCloneTable
         w _ITM_registerTMCloneTable
         U __libc_start_main@@GLIBC_2.0
```

2) Escribir una versión modificada llamada sys_strlen.S en la que, eliminando la directiva .set len, se calcule la longitud del mensaje (tercer parámetro para write) usando directamente strlen(3) (el código será muy parecido al de ejercicios anteriores).

```
#include <sys/syscall.h>  // SYS_write, SYS_exit

//
// See: <https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux>.
//

.globl main
main:
        push $msg
        call strlen
        mov %eax, %edx         // %edx == third argument (count)
        mov $SYS_write, %eax   // %eax == syscall number
        mov $1, %ebx           // %ebx == first argument (fd)
        mov $msg, %ecx         // %ecx == second argument (buf)
        int $0x80

        mov $SYS_exit, %eax
        mov $7, %ebx
        int $0x80

.data
msg:
        .ascii "Hello, world!\n"
```

3) En la convención de llamadas de GCC, ciertos registros son caller-saved (por ejemplo %ecx) y ciertos otros callee-saved (por ejemplo %ebx). Responder:

a) ¿Qué significa que un registro sea callee-saved en lugar de caller-saved?

Un registro calee-saved es un registro que contiene datos que tienen que ser conservados, por ende, una funcion en caso de querer utilizar esos registros tiene que guardar esos valores y luego restaurar su valor antes de llamar a `ret`

Por otra parte, un registro caller-saved son registros que pueden ser usados sin ninguna condicion de conservacion de valores.


b)  En x86 ¿de qué tipo, caller-saved o callee-saved, es cada registro según la convención de llamadas de GCC?

Caller-saved : eax, ecx, edx

Callee-saved: ebx, esi, edi, ebp, esp


4) Leer en Guide to Bare Metal Programming with GCC la sección: Linker options for default libraries and start files. En el archivo sys_strlen.S del punto anterior, renombrar la función main a \_start, y realizar las siguientes cuatro pruebas de compilación:

  *  int80_hi.S con -nodefaultlibs, luego con -nostartfiles
  *  sys_strlen.S con -nodefaultlibs, luego con -nostartfiles

Mostrar el resultado (compila o no) en una tabla 2 × 2, así como los errores de compilación.

int80_hi : Compila con **-nostartfiles** [1] y no compila con **-nodefaultlibs** [3]

sys_strlen : Compila con **-nostartfiles** y no compila con **-nodefaultlibs** [2]

**[1]** : Compila con el siguiente warning :
```
/usr/bin/ld: warning: cannot find entry symbol _start; defaulting to 0000000008049020
```
**[2]** : No compila con el error:
```
/usr/bin/ld: /usr/lib/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib32/crt1.o: in function `_start':
(.text+0x1c): undefined reference to `__libc_csu_fini'
/usr/bin/ld: (.text+0x23): undefined reference to `__libc_csu_init'
/usr/bin/ld: (.text+0x2c): undefined reference to `main'
/usr/bin/ld: (.text+0x32): undefined reference to `__libc_start_main'
/usr/bin/ld: /tmp/ccljrVQS.o: in function `_start':
/home/alexarch/Source_Code/OrgaDelComputador/Lab2/sys_strlen.S:10: undefined reference to `strlen'
collect2: error: ld returned 1 exit status
make: *** [<builtin>: sys_strlen] Error 1
```

**[3]** : No compila con el error :

```
/usr/bin/ld: /usr/lib/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib32/crt1.o: in function `_start':
(.text+0x1c): undefined reference to `__libc_csu_fini'
/usr/bin/ld: (.text+0x23): undefined reference to `__libc_csu_init'
/usr/bin/ld: (.text+0x32): undefined reference to `__libc_start_main'
collect2: error: ld returned 1 exit status
make: *** [<builtin>: int80_hi] Error 1
```

**Responder:** ¿alguno de los dos archivos compila con -nostdlib?

Si **int80_hi** compila con el mismo warning que tenia con **-nostartfiles** **[1]** y **sys_strlen** no compila con el error :

```
/usr/bin/ld: warning: cannot find entry symbol _start; defaulting to 0000000008049000
/usr/bin/ld: /tmp/ccS5WvAS.o: in function `_start':
/home/alexarch/Source_Code/OrgaDelComputador/Lab2/sys_strlen.S:10: undefined reference to `strlen'
collect2: error: ld returned 1 exit status
make: *** [<builtin>: sys_strlen] Error 1
```

5) Añadir al archivo Makefile una regla que permita compilar sys_strlen.S sin errores, así como cualquier otro archivo cuyo nombre empiece por sys:

```
sys_%: sys_%.S
	$(CC) $(ASFLAGS) $(CPPFLAGS) -nostartfiles $< -o $@
```

6) Mostrar la salida de nm --undefined para el binario **sys_strlen**, y explicar las diferencias respecto a **int80_hi**.

```
alexarch:Lab2/ (master*) $ nm --undefined-only sys_strlen
         U strlen@@GLIBC_2.0
alexarch:Lab2/ (master*) $ nm --undefined-only int80_hi
         w __gmon_start__
         w _ITM_deregisterTMCloneTable
         w _ITM_registerTMCloneTable
         U __libc_start_main@@GLIBC_2.0

```

El ouput de nm se puede entender de la siguiente manera

**U strlen@@GLIBC_2.0**

Con la **U** nos indica que el simbolo no esta definido, que cosa no esta definida? Todo lo que esta antes del **@@**, en este caso **strlen** y lo que esta despues del **@@** es donde posiblemente se pueda encontrar ese simbolo, en este caso **GLIBC_2.0**

Con la **w** nos indica que esta "debilmente" definido, esto significa que no es necesario que ese simbolo se resuelva a momento de linkeado, se deja la resolucion a momento de ejecucion.


### x86-ret

Se pide ahora modificar int80_hi.S para que, en lugar de invocar a a \_exit(), la ejecución finalice sencillamente con una instrucción ret. ¿Cómo se pasa en este caso el valor de retorno?

El valor de retorno cuando se usa `ret` es el valor que esta en el registro `eax`


Se pide también escribir un nuevo programa, libc_puts.S, que use una instrucción ret en lugar de una llamada a \_exit. Al contrario que int80_hi.S, este programa sí modifica la pila. Para simplificar la tarea, libc_puts.S puede usar puts(3) en lugar de write(2)

```
.globl main
main:
        push  $msg
        call  puts
        pop   %ecx
        mov   $0, %eax

        ret

msg:
        .ascii "Hello, world!\n\0"
```

### x86-ebp

- ¿Qué valor sobreescribió GCC cuando usó `mov $7, (%esp)` en lugar de `push $7` para la llamada a \_exit?

Sobreescribió el primer elemento del stack, a diferencia de push, que agrega el valor inmediato "7"

- La versión C no restaura el valor original de los registros %esp y %ebp. Cambiar la llamada a \_exit(7) por return 7, y mostrar en qué cambia el código generado. ¿Se restaura ahora el valor original de %ebp?


  GCC genera el siguiente codigo para la linea `return 7` de C

  ```
  8         return 7;
  (1) 0x08049195 <+31>:    mov    $0x7,%eax
  (2) 0x0804919a <+36>:    mov    -0x4(%ebp),%ecx
  (3) 0x0804919d <+39>:    leave  
  (4) 0x0804919e <+40>:    lea    -0x4(%ecx),%esp
  (5) 0x080491a1 <+43>:    ret
  ```

En (1) nada mas copia el valor 7 a `%eax` que es el valor de retorno

En (3) ejecuta la instruccion `leave` que segun la documentacion libera el stack frame generado para ese procedimiento, basicamente copia la direccion que esta en `%ebp` a `%esp` esto permite que con un simple `pop %ebp` pueda restaurarse el stack frame del procedimiento padre.

En este caso si, se restaura el valor original de `%ebp`


- Crear un archivo llamado lib/exit.c, ¿Qué ocurre con %ebp?

(Pasa lo mismo que en el punto anterior, no se si tendria que dar distinto)

- En hello.c, cambiar la declaración de my_exit y verificar qué ocurre con %ebp, relacionándolo con el significado del atributo noreturn.

TODO

### x86-argv
