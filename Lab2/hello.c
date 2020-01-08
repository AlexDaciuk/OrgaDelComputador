#include <string.h>
#include <unistd.h>

const char msg[] = "Hello, world!\n";

extern void my_exit(int status);

int main(void) {
  write(1, msg, strlen(msg));
  my_exit(7);
}
