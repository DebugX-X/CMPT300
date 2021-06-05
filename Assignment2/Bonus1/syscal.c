#include<stdio.h>

int main(void)
{
  char str[] = "Hello this is a test";
  printf("The string is : %s\n",str);
  printf("The string after calling : %s\n",str);
  uppercase(str);
  return 0;
}

