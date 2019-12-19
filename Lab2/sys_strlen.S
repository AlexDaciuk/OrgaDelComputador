#include <sys/syscall.h>  // SYS_write, SYS_exit

//
// See: <https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux>.
//

.globl _start
_start:
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
