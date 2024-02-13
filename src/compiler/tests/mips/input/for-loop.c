int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  int n;
  n = 0;

  for(n = 0; n < 12; n++) {
    syscall_print_string("n: ");
    syscall_print_int(n);
    syscall_print_string("\n");
  }

  syscall_print_string("n: ");
  syscall_print_int(n);
  syscall_print_string("\n");

  return 0;
}

