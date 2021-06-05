#include <string.h>
#include <stdio.h>

int main(){
    FILE* fp;
    char str[100];
    char name[100]
    double size,total,free,buffers,cached,slab;
    fp = fopen("/proc/meminfo","r");
    if (!fp){
        printf("Cannot open meminfo");
        return 1;
    }
    while(fgets(str,sizeof(str),fp)){
        sscanf(str,"%s %lf",name,&size);
        if(strstr(name,"MemTotal:")==0){
            total = size;
        }
        if(strstr(name,"MemFree:")==0){
            free = size;
        }
        if(strstr(name,"Buffers:")==0){
            buffers = size;
        }
        if(strstr(name,"Cached:")==0){
            cached = size;
        }
        if(strstr(name,"Slab:")==0){
            slab = size;
        }
    }
    fclose(fp);
    double memutil = (((total - free - buffers - cached - slab)/total)*100);
    printf("MEMORY UTILIZATION: %.2lf%% \n",memutil);
    return 0;
}
