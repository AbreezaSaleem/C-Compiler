int syscall_print_int(int i);
int syscall_print_string(char *s);

int main(void) {
  int i;
  i = 100;

  {
    int j;
    j = 200;
    syscall_print_string("Block 1: ");
    syscall_print_int(i + j);
    syscall_print_string("\n");
  }
  
  {
    int k;
    k = 300;
    syscall_print_string("Block 2: ");
    syscall_print_int(i + k);
    syscall_print_string("\n");
  }
  return 0;
}
