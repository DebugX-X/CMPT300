#include "dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void) {
    DIR *procdr;
    FILE *procfile;
    struct dirent *en;
    procdr = opendir("/proc");
    char name[100];
    if(procdr){
        while ((en = readdir(procdr))){
            char* path = (char*)calloc(1024,sizeof(char));
            if(path) {
                sprintf(path, "/proc/%s/cmdline",en->d_name);
                procfile = fopen(path, "r");
                if (procfile) {
                    size_t size;
                    size = fread(path, sizeof(char), 1024, procfile);
                    if(size>0) {
                        if ('\n' == path[size - 1])
                            path[size - 1] = '\0';
                        char * token = strtok(path,"/");
                        while( token != NULL ) {
                            strcpy(name,token);
                            token = strtok(NULL, "/");
                        }
                        printf("%s\t %s\n", en->d_name, name);
                    }
                    fclose(procfile);
                }
            }
        }
        closedir(procdr);
    }
    return 0;
}
