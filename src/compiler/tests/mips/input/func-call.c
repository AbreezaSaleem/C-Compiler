void syscall_print_int(int);

int f(int i, int j) {
  i = i * 2 * j;
  return i;
}

int main(void) {
  int i, j;
  i = 1;
  j = 4;
  i = f(i, j); 
  syscall_print_int(i);
  return 0;
}
