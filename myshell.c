#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// SigHandler 
void handler(){
    pid_t pid;
    int status; 
    while(1){
        pid = waitpid(-1,&status,WNOHANG);
        if(pid<=0){
            return;
        }
    }
}

//Execute 
int execute(struct pipeline *pipeline1, int input, int first, int last){
    pid_t child;
    int fd[2];
    pipe(fd); 
    int status; 

    child = fork();

    if(!child){
        if(pipeline1->commands->redirect_out_path){
            if((fd[1] = creat(pipeline1->commands->redirect_out_path, 0644)) < 0){
                perror("error: ");
                exit(0);
            }
        }
        if(pipeline1->commands->redirect_in_path){
            if((fd[1] = open(pipeline1->commands->redirect_in_path, O_RDONLY, 0)) < 0){
                perror("error: ");
                exit(0);
            }
        }
        
    if(pipeline1->is_background){
        close(fd[1]);
    }

    if(first == 1 && last == 0 && input == 0){
        dup2(fd[1], STDOUT_FILENO);
    }
    else if(first == 0 && last == 0 && input != 0){
        dup2(input, STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);
    }
    else{
        if(pipeline1->commands->redirect_out_path){
            dup2(fd[1], STDOUT_FILENO);
        }
        if(pipeline1->commands->redirect_in_path){
            dup2(fd[1], STDIN_FILENO);
        }
        dup2(input, STDIN_FILENO);
    }


    if(execvp(pipeline1->commands->command_args[0], pipeline1->commands->command_args) == -1){
        perror("error: ");
        exit(1); 
    }

    close(fd[1]);
    close(STDIN_FILENO);
    dup(STDIN_FILENO);
    close(fd[0]);
    }

    else{
        if(!(pipeline1->is_background)){
            wait(&status);
        }
        close(fd[1]);
    }

    if(input!=0){
        close(input);
    }

    if(last ==1){
        close(fd[0]);
    }

    return fd[0]; 
}

//Main
int main(int arg, char* argv[]){
    char buffer[1024];
    struct pipeline* pipeline1;
    int isprompt;
    char *chars = "-n";

    if((arg >1) && (strcmp(argv[1],chars)==0)){
        isprompt = 1; 
    }

    signal(SIGCHLD,handler); 

    while(1){
        if(isprompt){

        }
        else{
            printf("myshell$ ");
        }

        if (fflush(NULL)){
            perror("Error: ");
            return errno; 
        }

        const char *command_line;
        if((command_line = fgets(buffer, MAX_LINE_LENGTH, stdin)) == NULL){
            break;
        }
        else{
            pipeline1 = pipeline_build(command_line);
            int input = 0, first = 1;

            while(pipeline1->commands->next != NULL){
                input = execute(pipeline1, input, first, 0); //execute command func 
                first = 0;
                pipeline1-> commands = pipeline1->commands->next;
            }
        input = execute(pipeline1, input, first, 1); // execute command 
        }

    }

    pipeline_free(pipeline1);

    return 0;
}