void syscall_print_string(char*);
void syscall_print_int(int);

int main(void) {
	  int x, a, b, c, d, e, f;

  x = 2;
  a = 3;
  b = 4;
  c = 5;
  d = 6;
  e = 7;
  f = 8;

  x = a+b*c/d%e>f;


	syscall_print_int(x);

	return 0;
}
