void syscall_print_int(int);

short f(short i) {
  i = i * 2;
  return i;
}

int main(void) {
  short i;
  i = 1;
  i = f(i); 
  syscall_print_int(i);
  return 0;
}
