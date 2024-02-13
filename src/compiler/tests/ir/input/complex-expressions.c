
int main(void) {
  int i, j, k;

  i = j && (++k);
  i = j && (k++);
  i = j || (k++);
  return 0;
}
