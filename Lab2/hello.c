#include <unistd.h>

const char msg[] = "Hello, world!\n";

int main(void) {
    write(1, msg, sizeof msg - 1);
    _exit(7);
}
