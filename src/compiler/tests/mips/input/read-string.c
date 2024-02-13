int syscall_read_string(char *buffer, int length);
int syscall_print_string(char *buffer);

int main(void) {
  char buffer[256];

  syscall_read_string(buffer, 256);

  syscall_print_string(buffer);

  return 0;
}
