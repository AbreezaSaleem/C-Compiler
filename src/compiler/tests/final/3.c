int func(void) {
  return 1;
}

int main(void) {
  signed int a[10];

  int b;

  a[b];
  a[func()];
  return 1;
}
