int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  int n;
  n = syscall_read_int();
  syscall_print_int(n);
  return 0;
}
