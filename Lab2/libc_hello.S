.globl main
main:
        push $msg
        call strlen
        mov %eax, len

        // Call convention: arguments on the stack (reverse order).
        push $len
        push $msg
        push $1
        push $volver
        // No declaration needed; asm assumes symbols always exist.
        jmp write

volver:
        push $7
        call _exit

.data
msg:
        .ascii "Hello, world!\n"
len:
        .long 0
