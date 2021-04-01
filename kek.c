#include<stdio.h>
#include<unistd.h>
#include<string.h> 
#include<stdlib.h>
#include<sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define TRUE 1
#define INPUT_BUFFER_SIZE 1024
#define MAX_NUM_OF_COMMANDS 100
#define MAX_PROMT_SIZE 1024
#define MAX_USER_SIZE 16
#define MAX_CWD_SIZE 1024
#define MAX_ARG_NUM 64
#define MAX_NUMBER_OF_PIPES 16
#define MAX_USER_SIZE 16

static void prompt();
static int parse_input_and_exec(char*);

char *COMMANDS[100];
int PIPE_COUNT=0;

static void 
prompt() {
    char user[MAX_USER_SIZE]; 
    char cwd[MAX_CWD_SIZE];
    char myPrompt[MAX_PROMT_SIZE];
    if(getlogin_r(user,sizeof(user))==0){
        strcpy(myPrompt, user);
        strcat(myPrompt, ": ");
    }else{
        perror("getlogin_r() error\n");
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcat(myPrompt, cwd);
    }
    else{
        perror("getcwd() error\n");
    }
    strcat(myPrompt, " >>");
    printf("%s", myPrompt);
}

static int 
execute_command(char** args){
    if ((strcmp(args[0],"cat")==0) 
        && (strcmp(args[1],">")==0)){
        int new_file = open(args[2], O_CREAT | O_WRONLY | O_TRUNC ,0644);
        dup2(new_file ,STDOUT_FILENO);
        close(new_file);

        char * const arr[2] = {args[0], 0};
        if (execvp(args[0],arr) > 0){
            char msg[80];
            sprintf(msg,"cammnad %s failed\n", args[0]);
            perror(msg);
            exit(1);
        }
    }
    else if(
            (strcmp(args[0],"ls")==0)
            || (strcmp(args[0],"pwd")==0)
            || (strcmp(args[0],"cat")==0)
            || (strcmp(args[0],"nano")==0)
            || (strcmp(args[0],"wc")==0)
            || (strcmp(args[0],"cp")==0)
            || (strcmp(args[0],"whoami")==0)
            || (strcmp(args[0],"grep")==0)
            || (strcmp(args[0],"man")==0)
            || (strcmp(args[0],"tree")==0)
      ){

        if (execvp(args[0], args) > 0){
            char msg[80];
            sprintf(msg,"cammnad %s failed\n", args[0]);
            perror(msg);
            exit(1);
        }
    }else{
        printf("%s is not supported\n", args[0]);
        exit(1);
    }
}

static int 
parse_input_and_exec(char* input)
{
    char* commands[MAX_NUM_OF_COMMANDS];
    int n=1, first=1, last=0, len, ret_status, number_of_pipes, pid, returnStatus;

    int pipefds[MAX_NUMBER_OF_PIPES][2];

    /* parsing inpout */
    len = strlen(input);
    input[len -1]='\0';
    commands[0]=strtok(input,"|");
    while ((commands[n]=strtok(NULL,"|"))!=NULL)
        n++;
    commands[n]=NULL;

    number_of_pipes = n - 1;

    /* creates all needed pipes at the start */
    for(int i=0; i < number_of_pipes; i++ ){
        if( pipe(pipefds[i]) < 0 ){
            for (int j=0; j<i; j++){
                close(pipefds[j][0]);
                close(pipefds[j][1]);
            }
            perror("pipe error");
            exit(1);
        }
    }

    for (int i=0; i < n; i++){

        /* spliting command into args  */
        char* args[MAX_ARG_NUM];
        int m=1;
        args[0]=strtok(commands[i]," ");       
        while((args[m]=strtok(NULL," "))!=NULL)
            m++;
        args[m]=NULL;

        if (i==n-1) last=1;

        pid = fork();
        if (pid == 0){
            if (!first) 
            {
                close(STDIN_FILENO);
                dup2(pipefds[i-1][0],STDIN_FILENO);
            } 
            if (!last) 
            {
                close(STDOUT_FILENO);
                dup2(pipefds[i][1],STDOUT_FILENO);
            } 
            /* closing pipes  */
            for(int i=0; i < number_of_pipes; i++ ){
                close(pipefds[i][0]);
                close(pipefds[i][1]);
            }
            
            execute_command(args);
            /* if (execvp(args[0], args) > 0){ */
            /*     exit(1); */
            /* } */
        }


        if(i){
            close(pipefds[i-1][0]);
            close(pipefds[i-1][1]);
        }
        waitpid(pid, &returnStatus, 0);  // Parent process waits here for child to terminate.

        if (first) first=0;
    }

    return ret_status;
}

int 
main()
{
    int ret;
    char input_buffer[ INPUT_BUFFER_SIZE ];
    char *ch = "\n";
    char *exit= "exit\n";
    while(TRUE){
        prompt();
        fgets(input_buffer, sizeof(input_buffer), stdin);
        //read(STDIN_FILENO, input_buffer, sizeof(input_buffer));
        if(strcmp(input_buffer, ch)==0)
        {
            continue;
        }
        if(strcmp(input_buffer, exit)==0)
        {
            break;
        }
        if(input_buffer[0] == 'c' && input_buffer[1] == 'd'){
            char* args[MAX_ARG_NUM];
            int m=1;
            args[0]=strtok(input_buffer," ");       
            while((args[m]=strtok(NULL," "))!=NULL)
                m++;
            args[m]=NULL;
            char *h="/home";   
            if(args[1]==NULL)
                    chdir(h);
            else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
                    chdir(h);
            else if(chdir(args[1])<0)
                printf("bash: cd: %s: No such file or directory\n", args[1]);
            continue;
        }

        ret = parse_input_and_exec(input_buffer);
    }
    return 0;
}
