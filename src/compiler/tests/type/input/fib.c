int main(void) {
  return 0;
}
short fib(short n) {
  return (n < 2 ? 1 : fib(n-1) + fib(n-2));
}
