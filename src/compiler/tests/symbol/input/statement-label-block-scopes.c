char *az;
int main2(void) {
  int c;

  {
    int g;
    c = 44;
    az = "rizwana";
    {
      int j;
      az = "saleem";
    }
  }

  {
    int h;
    az = "saleem";
  }
  {
    int k;
  }

  label: {
    int l;
  }

  az = "abreeza";

}
