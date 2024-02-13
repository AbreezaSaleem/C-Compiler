void syscall_print_int(int);
void syscall_print_string(char*);

int main(void) {
  int i, l, *p;
  int arr[3];

  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  p = arr;

  for (i = 0; i < 3 ; i++) {
    syscall_print_string("Element at index ");
    syscall_print_int(i);
    syscall_print_string(": ");
    syscall_print_int(*(p + i));
    syscall_print_string("\n");
  }
  return 1;
}
