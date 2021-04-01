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
#define MAX_ARG_NUM 64
#define MAX_NUMBER_OF_PIPES 16

static void prompt();
static int parse_input_and_exec(char*);
static void swap_fd(int*, int*);

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





static int 
parse_input_and_exec(char* input)
{
    char* commands[MAX_NUM_OF_COMMANDS];
    int n=1, first=1, last=0, ret_status, number_of_pipes, pid, returnStatus;
    int pipefds[MAX_NUMBER_OF_PIPES][2];

    /* parsing inpout */
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
                //dup2(pipefds[i-1][0],STDIN_FILENO);
            } 
            else if (!last) 
            {
                //dup2(pipefds[i][1],STDOUT_FILENO);
            } 
            /* closing pipes  */
            for(int i=0; i < number_of_pipes; i++ ){
                close(pipefds[i][0]);
                close(pipefds[i][1]);
            }

            printf("writing to %d and reading from %d \n",pipefds[i][1], pipefds[i-1][0]);
            exit(0);
            
            /* if (execvp(args[0], args) > 0){ */
            /*     perror("execvp failed"); */
            /*     exit(1); */
            /* } */
        }

        if(i){
            close(pipefds[i-1][0]);
            close(pipefds[i-1][1]);
        }
        waitpid(pid, &returnStatus, 0);  // Parent process waits here for child to terminate.

        if (ret_status) {
            exit(ret_status);
        }
        if (first) first=0;
    }

    return 1;
}

int 
main()
{
    int len , ret;
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
