void syscall_print_int(int);
void syscall_print_string(char*);

int main(void) {
  int i, *k;

  k = &i;
  *k = 12;
  i = *k;

  syscall_print_string(" i: ");
  syscall_print_int(i);
  
  return 1;
}
