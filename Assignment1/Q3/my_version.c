#include <string.h>
#include <stdio.h>

int main(){
    FILE* fp;
    char str1[100], str2[100];
    char os[100], kern[100];
    fp = fopen("/etc/os-release","r");
    if (!fp){
        printf("Cannot open os-release");
        return 1;
    }
    while(fscanf(fp," %255[^=]=%255[^\n]",str1,str2)!=EOF){
        if(strstr(str1,"PRETTY_NAME")!=NULL){
            char *token = strtok(str2,"\"");
            while (token != NULL ){
                strcpy(os,token);
                token = strtok(NULL,"\"");
            }
        }
    }
    fclose(fp);
    printf("OS: %s\n",os);
    fp = fopen("/proc/version","r");
    if (!fp){
        printf("Cannot open kernel version");
        return 1;
    }
    fgets(str,sizeof(kern),fp)
    char *token = strtok(kern," ");
    while (token != NULL ){
        printf("%s ",token);
        if(strstr(token,"generic")!=NULL)
            break;
        token = strtok(NULL," ");
    }
    fclose(fp);
    return 0;
}
