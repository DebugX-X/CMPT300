#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

/** defining my structures for log and Environmental variables **/
typedef struct{
    char *name;
    struct tm time;
    int code;
}command;

static command **cmdlog;
static int cmdsize = 0;

typedef struct{
    char *name;
    char *value;
}EnvVar;

EnvVar **envVar;
int varsize = 0;

/** Exit CSHELL*/
void exiter(){
    printf("Bye!");
    exit(0);
}
/** HELP USER*/
void Helpme(){
    char* username = strtok(getenv("USER"),"@");
    printf("hey %s, it appears that you are missing keyword or command, or have permission problem\n",username);
}

/** Helper funcs for log */
void addlog(command *newcmd){

    /* first call */
    if(cmdsize == 0){
        cmdlog = malloc(100* sizeof(command));
        if(cmdlog == NULL){
            fprintf(stderr,"error memory allocation\n");
            exit(0);
        }
    }
    if(cmdsize == 99) {
        printf("Maximum number of log size reached");
        exiter();
    }
    cmdlog[cmdsize++] = newcmd;
}

int logPrint(){
    if(cmdsize == 0) {
        printf("log is empty\n");
        return 1;
    }else{
        int i;
        for (i = 0; i<cmdsize;i++){
            printf("%s",asctime(&cmdlog[i]->time));
            printf(" %s %d\n",cmdlog[i]->name,cmdlog[i]->code);
        }
        return 0;
    }
}

void freelog(){
    for(int i =0; i<cmdsize;i++){
        free(cmdlog[i]);
    }
    cmdsize = 0;
}

/*** Helper funcs for EnvVar ***/
int addEVar(EnvVar *newvar){

    if(varsize == 0){
        envVar = malloc(100* sizeof(EnvVar));
        if(envVar == NULL){
            fprintf(stderr,"error memory allocation\n");
            exiter();
        }
    }
    if(varsize == 99) {
        printf("Maximum number of Environmental variables defined, Cannot define %s", newvar->name);
        exiter();
    }
    envVar[varsize++] = newvar;
    return 0;
}

// Updates EnvVar's value and return 0
int upEVar(EnvVar *var){
    for(int i = 0; i<varsize;i++){
        if(strcmp(envVar[i]->name, var->name)==0){
            envVar[i] = var;
            return 0;
        }
    }
    return 1;
}

// Checks if an EnvVar exists, return 1 if it does
int checkEVar(char *name){
    for(int i = 0 ;i<varsize;i++){
        if(strcmp(envVar[i]->name,name)==0){
            return 1;
        }
    }
    return 0;
}

// Gets EnvVar value
char *getEVar(char* name){
    for(int i =0 ;i<varsize;i++){
        if(strcmp(envVar[i]->name,name)==0){
            return envVar[i]->value;
        }
    }
    return 0;
}

/** welcome prompt for Interactive mode **/
void printPrompt() {
    printf("\n\n\n\n******************************************");
    printf("\n\n\n\t****CSHELL Submission by****");
    printf("\n\n\n\n******************************************");
    char* username = getenv("USER");
    printf("\n\n\nThe current USER is: @%s", username);
    printf("\n");
}

/** gets user input */
char *getCmd(void){

    int len = 0;
    char *buffer = malloc(1024 * sizeof (char));
    int buffsize = 1024;
    char c;
    if(!buffer){
        fprintf(stderr, "Input error\n");
        exiter();
    }
    while(1){
        c = getchar();
        if(c == EOF){
            free(buffer);
            return NULL;
        }else if( c == '\n'){
            buffer[len] = '\0';
            return buffer;
        }
        if(len >= buffsize){
            buffsize += 1024;
            buffer = realloc(buffer,buffsize);
        }
        buffer[len++] = c;
    }
}

/** tokenizes the input using space, tab and newline */
void stringparser(char *parse, char **token){
    int i = 0;
    token[i]= strtok(parse," \n\t");
    while( token[i] != NULL && i < 100) {
        i++;
        token[i] = strtok(NULL, " \n\t");
    }
}

/** checks if input command is a buildin command */
int checkBICmd(char *command){
    char* buildCmds[] = {"exit","log","print","theme"};
    for(int i = 0; i < 4; i++){
        if(strcmp(command,buildCmds[i]) == 0){
            return i + 1;
        }
    }
    return -1;
}

// checks if Argument is in $var format
int isEnvVar(char *cmd){
    if(cmd[0]=='$'){
        return 1;
    }
    return 0;
}

/** print cmd **/
int printer(char **parse){
        int i = 1;
        while (parse[i] != NULL) {
            if(isEnvVar(parse[i])){ //Format check
                if(checkEVar(parse[i])) {//check if arguement is EnvVar
                    printf("%s ", getEVar(parse[i]));
                }
                else{
                    Helpme();
                    exiter();
                }
            }else {
                printf("%s ", parse[i]);
            }
            i++;
        }
        printf("\n");
        return 0;
}

/** Changes the theme of the output */
int changeTheme(char *colour){
    char *allowedColours[] = { "black", "red", "green", "yellow", "blue", "purple", "cyan", "white"};
    if(colour == NULL)
    {
        printf("please enter a theme colour, allowed colours are :\n");
        for(int i = 0; i < 8; i++ ){
            printf("\t%s\n",allowedColours[i]);
        }
        return 1;
    }
    int clr = 0;
    for(int i = 0; i < 8; i++ ){
        if(strcmp(colour,allowedColours[i])==0){
            clr = i + 1;
        }
    }
    switch(clr){
        case 1:
            printf("\033[1;30m");
            return 0;
        case 2:
            printf("\033[1;31m");
            return 0;
        case 3:
            printf("\033[1;32m");
            return 0;
        case 4:
            printf("\033[1;33m");
            return 0;
        case 5:
            printf("\033[1;34m");
            return 0;
        case 6:
            printf("\033[1;35m");
            return 0;
        case 7:
            printf("\033[1;36m");
            return 0;
        case 8:
            printf("\033[1;37m");
            return 0;
        default:
            printf("Unknown theme colour, please choose the theme colour from the following :\n");
            for(int i = 0; i < 8; i++ ){
                printf("\t%s\n",allowedColours[i]);
            }
    }
    return 1;
}

//checks if Agrument is in the $<var>=<value> format
int valEnvVar(char* cmd){
    char c;
    for(int i = 0;;i++){
        c = cmd[i];
        if(c == '='){
            return 1;
        }else if(c =='\0'){
            return 0;
        }
    }
}

/** execute none build-in commands */
int nonBuildCmd(char** parse){
    int pipein[2];
    int pipeout[2];
    if(pipe(pipein)<0){
        printf("Unable to initialize input pipe, Please try again\n");
        return 1;
    }
    if (pipe(pipeout)<0) {
        printf("Unable to initialize output pipe, Please try again\n");
        return 1;
    }
    close(pipein[0]);
    if(parse[1]!=NULL) {
        int i =1;
        while (parse[i] != NULL) {
            if (checkEVar(parse[i])) {
                write(pipein[1], getEVar(parse[i]), strlen(parse[i]));
            } else {
                write(pipein[1], parse[i], strlen(parse[i]));
            }
            i++;
        }
    }
    close(pipein[1]);
    pid_t p1 = fork();
    if(p1 == -1) {
        printf("Unable to fork a child, Please try again\n");
        return 1;
    }else if(p1 == 0) {
        close(pipein[1]);
        dup2(pipein[0],STDIN_FILENO);
        close(pipein[0]);
        int fail = 0;
        if (execvp(parse[0], parse) < 0) {
            fail = 1;
        }
        close(pipeout[0]);
        write(pipeout[1], &fail, sizeof(fail));
        close(pipeout[1]);
        exit(0);
    }else {
        wait(NULL);
        int ret;
        close(pipeout[1]);
        read(pipeout[0], &ret, sizeof(ret));
        close(pipeout[0]);
        if(ret == 1) {
            return 1;
        }else{
            return 0;
        }
    }
}

int main( int argc, char *argv[]) {
    time_t rtime;
    int sw;
    if(argc < 2) {
        printPrompt();
        while (1) {
            char *cmdline,*args[100];
            printf("cshell$ ");
            cmdline = getCmd();
            command *cmd = malloc(sizeof (command));
            time(&rtime);
            cmd->time = *localtime(&rtime);
            stringparser(cmdline, args);
            cmd->name = strdup(args[0]);
            if ((sw = checkBICmd(args[0])) > 0) {
                switch (sw) {
                    case 1:
                        exiter();
                    case 2:
                        cmd->code = logPrint();
                        break;
                    case 3:
                        cmd->code = printer(args);
                        break;
                    case 4:
                        cmd->code = changeTheme(args[1]);
                        break;
                }
            } else if (isEnvVar(args[0]) && valEnvVar(args[0])) {
                EnvVar *var = malloc(sizeof(EnvVar));
                var->name = strdup(strtok(args[0], "="));
                var->value = strdup(strtok(NULL, "="));
                if(checkEVar(var->name)==0){
                    cmd->code = addEVar(var);
                }else{
                    cmd->code = upEVar(var);
                }
            } else {
                if (!(nonBuildCmd(args))) {
                    cmd->code = 0;
                } else {
                    Helpme();
                    exiter();
                    cmd->code = 1;
                }
            }
            addlog(cmd);
        }
    }else{
        printf("Using script mode\n");
        char *args[100], cmdline[1024];
        if(strstr(argv[1],".txt")==NULL){
            strcat(argv[1],".txt");
        }
        FILE *fp = fopen(argv[1], "r");
        if(fp == NULL){
            printf("Unable to open the file\n");
            exit(EXIT_FAILURE);
        }
        while (fgets(cmdline, sizeof(cmdline),fp)!=NULL){
            command *cmd = malloc(sizeof (command));
            time(&rtime);
            cmd->time = *localtime(&rtime);
            stringparser(cmdline, args);
            cmd->name = strdup(args[0]);
            if ((sw = checkBICmd(args[0])) > 0) {
                switch (sw) {
                    case 1:
                        exiter();
                    case 2:
                        cmd->code = logPrint();
                        break;
                    case 3:
                        cmd->code = printer(args);
                        break;
                    case 4:
                        cmd->code = changeTheme(args[1]);
                        break;
                }
            } else if (isEnvVar(args[0]) && valEnvVar(args[0])) {
                EnvVar *var = malloc(sizeof(EnvVar));
                var->name = strdup(strtok(args[0], "="));
                var->value = strdup(strtok(NULL, "="));
                if(checkEVar(var->name)==0){
                    cmd->code = addEVar(var);
                }else{
                    cmd->code = upEVar(var);
                }
            } else {
                if (!(nonBuildCmd(args))) {
                    cmd->code = 0;
                } else {
                    Helpme();
                    exiter();
                    cmd->code = 1;
                }
            }
            addlog(cmd);
        }
    }
    return 0;
}
