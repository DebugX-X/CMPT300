#include<linux/kernel/h>
#include<ctype.h>

asmlinkage long theme(char *colour)
{
  char *allowedColours[] = { "black", "red", "green", "yellow", "blue", "purple", "cyan", "white"};
  for(int i = 0; i < 8; i++ ){
    if(strcmp(colour,allowedColours[i])==0){
      clr = i + 1;
    }
  }
  switch(clr){
    case 1:
      printk("\033[1;30m");
      return 0;
    case 2:
      printk("\033[1;31m");
      return 0;
    case 3:
      printk("\033[1;32m");
      return 0;
    case 4:
      printk("\033[1;33m");
      return 0;
    case 5:
      printk("\033[1;34m");
      return 0;
    case 6:
      printk("\033[1;35m");
      return 0;
    case 7:
      printk("\033[1;36m");
      return 0;
    case 8:
      printk("\033[1;37m");
      return 0;
    default:
      printk("Invalid Theme colour, please choose the theme colour from the following :\n");
      for(int i = 0; i < 8; i++ ){
        printk("\t%s\n",allowedColours[i]);
      }
    }
  return 0;
}
