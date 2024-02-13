short func(void) {
  return 0;
}

int func2(int a) {
  return a;
}

int main(void) {
  func2(func());

  return 1;
}
