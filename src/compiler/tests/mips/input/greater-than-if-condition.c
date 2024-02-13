int syscall_read_int(void);
int syscall_print_int(int i);
int syscall_print_string(char *ed);

int main(void) {
  int n;
  n = 3;

  if(n > 8) {
    n = 99;
  } else {
    n = 100;
  }
  syscall_print_int(n);
  
  return 0;
}
