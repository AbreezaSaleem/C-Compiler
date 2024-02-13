int funcB(int a, char*b, long arr[45]) {
  return 45;
}

int funcA(void) {
  int a, b, c;
  long arr[45];
  funcB(1, 'a', arr);
}


char funcC(int (*p)[7]) {
  return 'A';
}

int main(void) {
  return 0;
}

char funcC(int (*p)[7]) {
  return 'A';
}
