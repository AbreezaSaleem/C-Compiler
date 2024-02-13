int syscall_print_int(int i);

int main(void) {
  int i;
  int j, k;
  j = 0;
  k = 1;

  i = j && k;

  syscall_print_int(i);

  i = j || k;

  syscall_print_int(i);

  return 0;
}
