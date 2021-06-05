#include<linux/kernel/h>
#include<ctype.h>

asmlinkage long uppercase(char *str)
{
        while(*str != '\0')
        {
          *str = toupper(*str);
          str++;
        }
        printk("%s\n", str);
        return 0;
}
