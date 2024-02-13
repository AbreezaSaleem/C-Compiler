int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  char *name;
  name = "Hello world!\n";
  syscall_print_string(name);
  return 0;
}
