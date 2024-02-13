void syscall_print_int(int);
void syscall_print_string(char *);
  
char *global;

int inc(int a) {
  int temp;
	temp = 5;
  temp = a * 2;
	return a + 2;
}

void foo(void) {
	return;
}

int main(void) {
	int i;
	int result;
	result = 0;

	for (i = 0; i < 32500; i++) {
		int a;

		if (45 > 2) {
			a = 44;
		} else {
			a = 55;
		}

		result = inc(result);
	}

	label: {
		foo();
	}

	syscall_print_string("Result after 32500 iterations: ");
	syscall_print_int(result);
	syscall_print_string("\n");

	return 0;
}
