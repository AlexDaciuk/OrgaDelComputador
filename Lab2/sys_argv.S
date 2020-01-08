#include <sys/syscall.h>  // SYS_write, SYS_exit

//
// See: <https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux>.
//

.globl _start
_start:
        mov   %esp,%ebp
        push  8(%ebp)
        call  strlen
        push  %eax
        mov   %eax, %edx         // %edx == third argument (count)
        mov   $SYS_write, %eax   // %eax == syscall number
        mov   $1, %ebx           // %ebx == first argument (fd)
        mov   8(%ebp), %ecx      // %ecx == second argument (buf)

        movl  $('\n'), (%ecx, %edx, 1)  // Agrego el salto de linea al final
        add   $1, %edx

        pop   %edi
        movl  %edi, (%ecx, %edx, 1) // Agrego el tamaño en bytes
        add   $1, %edx

        movl  $('\n'), (%ecx, %edx, 1)  // Agrego otro salto de linea al final
        add   $1, %edx

        int   $0x80

        mov   $SYS_exit, %eax
        mov   $7, %ebx
        int   $0x80
