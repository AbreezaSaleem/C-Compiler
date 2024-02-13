int a;

int main(void) {
   int b, c;
   a = 34;

   a = 89;

   b= 5;

   c = 45;

   {
      int d;
      d = 56;
      {
         int e;
         e = 67;
      }
   }
}


int main2(void) {
   int f;

   if (f > 45) {
      f = 56;
   }
}
