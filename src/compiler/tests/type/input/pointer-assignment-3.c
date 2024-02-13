int main(void) {
  short *a, *b;
  int *p;
  int *q;
  int x;

  p = q;

  x = p;
  p = x;

  *p = x;
  x = *p;

  *p = *q;
  return 0;
}
