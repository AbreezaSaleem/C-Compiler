int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  int i;
  i = 448;
  syscall_print_int(i);

  return 555;
}
