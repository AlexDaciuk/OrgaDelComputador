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
        .asciz "Hello, world!\n"

.set len, . - msg
