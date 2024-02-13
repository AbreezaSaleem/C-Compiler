void func(void) {
  return 0;
}

int func2(int a) {
  return a;
}

int main(void) {
  func(10);
  func();
  func2(func());
  func2();

  return 1;
}
