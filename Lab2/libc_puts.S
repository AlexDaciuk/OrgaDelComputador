.globl main
main:
        push  $msg
        call  puts
        pop   %ecx
        mov   $0, %eax

        ret

msg:
        .ascii "Hello, world!\n\0"
