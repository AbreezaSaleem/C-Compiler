int syscall_print_int(int i);

short fib(short n) {
  return (n < 2 ? 1 : fib(n-1) + fib(n-2));
}

int main(void) {
  int i;
  
  i = fib(5);
  syscall_print_int(i);
  return 0;
}
