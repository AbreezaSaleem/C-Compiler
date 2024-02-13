int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  int n;
  n = 0;

  do {
    syscall_print_string("n: ");
    syscall_print_int(n);
    syscall_print_string("\n");
    n++;
  } while (n < 10);

  syscall_print_string("n: ");
  syscall_print_int(n);
  syscall_print_string("\n");
  
  return 0;
}
