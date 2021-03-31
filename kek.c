#include<stdio.h>
#include<unistd.h>
#include<string.h> 
#include<stdlib.h>
#include<sys/wait.h>

#define TRUE 1
#define INPUT_BUFFER_SIZE 1024
#define MAX_NUM_OF_COMMANDS 100
#define MAX_PROMT_SIZE 1024
#define MAX_CWD_SIZE 1024


static void prompt();
static int parse_input_and_exec(char*);

char *COMMANDS[100];
int PIPE_COUNT=0;

static void 
prompt() {
    char cwd[MAX_CWD_SIZE];
    char myPrompt[MAX_PROMT_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // TODO add user
        strcpy(myPrompt, "#######user ");
        strcat(myPrompt, cwd);
        strcat(myPrompt, " >>>>>>>>>>> ");

        printf("%s", myPrompt);
    }
    else
        perror("getcwd() error");
}

/* void change_directory(char** args){ */
/*     char *h="/home";    */
/*     if(args[1]==NULL) */
/*         chdir(h); */
/*     else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0)) */
/*         chdir(h); */
/*     else if(chdir(args[1])<0) */
/*         printf("bash: cd: %s: No such file or directory\n", args[1]); */
/* } */

/* void execute(char *cmd_exec) */
/* { */
/*     int pid; */
/*  */
/*     if (args[0] != NULL)  */
/*     { */
/*  */
/*         if (strcmp(args[0], "exit") == 0)  */
/*             exit(0); */
/*         if(strcmp("cd",args[0])==0) */
/*         { */
/*             change_directory(args); */
/*             return; */
/*         } */
/*         #<{(| else if(strcmp("pwd",args[0])==0) |)}># */
/*         #<{(| { |)}># */
/*         #<{(|     parent_directory(); |)}># */
/*         #<{(|     return 1; |)}># */
/*         #<{(| } |)}># */
/*     } */
/*  */
/*  */
/*     pid = fork(); */
/*  */
/*     if (pid == 0){ */
/*         execvp(args[0],args); */
/*     }else{ */
/*         waitpid(pid, 0, 0);   */
/*     } */
/*  */
/* } */

static int 
execute(char** args,int first,int last, int* fd)
{
    int pid;
    pid = fork();
    if (pid == 0){
        if (first==1 && last==0) 
        {
        } 
        else if (first==0 && last==0 ) 
        {
        } 
        else 
        {
        }

        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    }else{
        waitpid(pid, 0, 0);   
        if (first==1 && last==0) 
        {
        } 
        else if (first==0 && last==0 ) 
        {
        } 
        else 
        {
        }
        return fd[0];
   }
}

    static int 
parse_input_and_exec(char* input)
{
    char* commands[MAX_NUM_OF_COMMANDS];
    int n=1, first=1, last=0, ret_status;

    /* parsing inpout */
    commands[0]=strtok(input,"|");
    while ((commands[n]=strtok(NULL,"|"))!=NULL)
        n++;
    commands[n]=NULL;

    // pipe
    int fd[2];
    if (pipe(fd) < 0){
        perror("pipe error");
        exit(1);
    }
    for (int i=0; i < n; i++){
        char* args[80];

        int m=1;
        args[0]=strtok(commands[i]," ");       
        while((args[m]=strtok(NULL," "))!=NULL)
            m++;
        args[m]=NULL;


        if (i==n-1)
            last=1;
        ret_status=execute(args,first,last,fd);
        if (first)
            first=0;
    }
    return 1;
}

    int 
main()
{
    int len;
    char input_buffer[ INPUT_BUFFER_SIZE ];
    char *ch = "\n";
    while(TRUE){
        prompt();
        fgets(input_buffer, sizeof(input_buffer), stdin);
        if(strcmp(input_buffer, ch)==0)
        {
            continue;
        }

        len = strlen(input_buffer);
        input_buffer[len -1]='\0';
        //TODO retun int
        parse_input_and_exec(input_buffer);
    }
    return 0;
}
