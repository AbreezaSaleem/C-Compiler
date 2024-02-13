

int main(void) {
  int i , j;
  i = 99;
  func(i);
  return 555;
}

int func(int x) {
  return 0;
}

// both not working :(

/*

int func(int x);

int main(void) {
  int i , j;
  i = 99;
  func(i);
  return 555;
}

int func(int x) {
  return 0;
}
int syscall_print_int(int x);
int syscall_print_string(char * c);
int syscall_read_int();
int syscall_read_string();

*/
