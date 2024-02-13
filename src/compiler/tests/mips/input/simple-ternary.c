int syscall_print_int(int i);

int main(void) {
  int i;

  i = 4 < 5 ? 11 + 3 : 22;

  syscall_print_int(i);

  return 0;
}
