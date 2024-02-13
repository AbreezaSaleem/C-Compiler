int main(void) {
  int i, x, a, b, c, d;
  goto one;

  one: {
    int a;
    if (a > 45) {
      return 0;
    }
  }
  two: {}
  three: while(!x) {
    i++;
  }
  four: a > b ? c : d;
  return 0;
}
