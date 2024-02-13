void syscall_print_int(int i);
void syscall_print_string(char *);

int g(int a) {
  return a;
}

int f(int a, int b) {
  return a + b;
} 

int main(void) {
  int i;
  
  i = f(5, g(4));

  syscall_print_string("Result of: i = f(5, g(4)) = ");
  syscall_print_int(i);
  syscall_print_string("\n");

  syscall_print_string("Result of: i++ = ");
  syscall_print_int(i+1);
  syscall_print_string("\n");
  return 0;
}
