void syscall_print_int(int);
void syscall_print_string(char*);

int main(void) {
  int i, a[10];

  for (i = 0; i < 10; i = i + 1) {
    a[i] = i * i;
  }
  
  for (i = 0; i < 10; i = i + 1) {
    syscall_print_int(a[i]);
    syscall_print_string("\n");
  }
  
  return 0;
}
