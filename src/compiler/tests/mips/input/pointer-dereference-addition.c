void syscall_print_int(int);
void syscall_print_string(char*);

int main(void) {
  int i, l, *j, *k;

  k = &i;
  j = &l;

  *k = 12;
  *j = 8;

  i = *k + *j;

  syscall_print_int(i);
  
  return 1;
}
