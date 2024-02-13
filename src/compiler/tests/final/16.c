void syscall_print_int(int);
void syscall_print_string(char *);
  
int inc(int a) {
	return a + 1;
}

int main(void) {
	int i;
	int result;
	result = 0;

	for (i = 0; i < 100000; i++) {
		result = inc(result);
	}

	syscall_print_string("Result after 10000 iterations: ");
	syscall_print_int(result);
	syscall_print_string("\n");

	return 0;
}
