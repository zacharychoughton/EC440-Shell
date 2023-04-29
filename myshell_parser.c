#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Special Characters: 
// |, <, >, &, \n, \t, 
enum specialchars{
    pipe = '|',
    rin = '<',
    rout = '>',
    amp = '&'
};

struct pipeline* pipeline_init(){
    struct pipeline* pipe_m = malloc(sizeof(struct pipeline));
    struct pipeline_command *commands_m = malloc(sizeof(struct pipeline_command));
    int i;

    commands_m->next = NULL;
    commands_m->redirect_in_path = NULL;
    commands_m->redirect_out_path = NULL;
    for(i = 0; i< MAX_ARGV_LENGTH; i++){
        commands_m->command_args[i] = NULL:
    }

    pipe_m->commands = commands_m;
    pipe_m->is_background = false;

    return pipe_m;
}

void parse(char *token, struct pipeline* pline){
    char* coms, p, temp;

    temp = strtok(token," ");

    if (strstr(token,"<")){
        p = strchr(token,'<');
        char redirect[strlen(p+1)];
        strcpy(redirect,p+1);
    }
    else if(strstr(token,">")){
        p = strchr(token,'>');
        char redirect[strlen(p+1)];
        strcpy(redirect,p+1);
    }

    while(temp){
        strcpy(pline->commands->command_args[k],temp);
        temp = strtok(NULL," ");
    }

}

struct pipeline *pipeline_build(const char *command_line){
    struct pipeline* pline;
    char* token; 
    char* line; 
    int len;

    //Initialization 
    len = strcspn(command_line,"\n");
    line = strdup(command_line);
    line[len] = '\0';
    pline = pipeline_init();

    if(strstr(line,"&")){
        pline->is_background = true;
        len = strcspn(line,"&");
        line[len] = '\0';
    }

    token = strtok(line,"|");

    do{
        parse(token,pline);
    }while(token = strtok(NULL,"|"));

    return pline;
}

void pipeline_free(struct pipeline *pipeline){
    struct pipeline_command* nextcom = pipeline->commands;

    while(nextcom != NULL){
        pipeline->commands = nextcom->next;
        free(nextcom->command_args);
        free(nextcom);
        nextcom = pipeline->commands;
    }

    free(pipeline);
}

//Structures: struct pipeline_command {
// 	char *command_args[MAX_ARGV_LENGTH]; /* List of pointers to each
// 						argument for a command. The
// 						first entry is the command
// 						name. The last entry is NULL.
// 						E.g., input "ls -al" is
// 						equivalent to ["ls", "-al",
// 						NULL] */
// 	char *redirect_in_path; /* Name of a file to redirect in from, or NULL
// 				   if there is no stdin redirect */
// 	char *redirect_out_path; /* Name of a file to redirect out to, or NULL if
// 				    there is no stdout redirect */
// 	struct pipeline_command *next; /* Pointer to the next command in the
// 					  pipeline. NULL if this is the last
// 					  command in the pipeline */
// };

// /*
//  * Represents a collection of commands that are connected through a pipeline.
//  */
// struct pipeline {
// 	struct pipeline_command *commands; /* Pointer to the first command in
// 					      the pipeline*/
// 	bool is_background; /* True if this pipeline should execute in the
// 			       background */
// };