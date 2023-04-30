#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Special Characters: 
// |, <, >, &, \n, \t, 

struct pipeline_command* pipeline_command_init(){
    struct pipeline_command *commands_m = malloc(sizeof(struct pipeline_command));
    int i; 
    commands_m->next = NULL;
    commands_m->redirect_in_path = NULL;
    commands_m->redirect_out_path = NULL;
    for(i = 0; i< MAX_ARGV_LENGTH; i++){
        commands_m->command_args[i] = NULL;
    }
    return commands_m;
}

struct pipeline* pipeline_init(){
    struct pipeline* pipe_m = malloc(sizeof(struct pipeline));

    pipe_m->commands = NULL;
    pipe_m->is_background = false;

    return pipe_m;
}

//Finds next NULL pointer in pline->commands->next and links it to given initialized cline. 
void link(struct pipeline* pline, struct pipeline_command* cline){
    struct pipeline_command* temp;

    if(pline->commands == NULL){
        pline->commands = cline;
    } else{
        temp = pline->commands;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = cline;
    }
}

char* replace(char* str, char* torep, char* toins){
    int rlen = strlen(torep);
    int nlen = strlen(toins);
    int count = 0;
    char* p = strstr(str, torep);
    while(p!=NULL){
        count++;
        p = strstr(p+rlen, torep);
    }
    char* output = (char*) malloc(strlen(str)-count* (rlen - nlen) +1);
    char *r = output;
    p = str;
    while(*p){
        if(strstr(p, torep) == p){
            strcpy(r, toins);
            r += nlen;
            p += rlen;
        }
        else{ 
            *r++ = *p++;
        }
    }
    *r = '\0';
    return output; 
}

void parse(char* line, struct pipeline_command **cline){
    struct pipeline_command *cmd = pipeline_command_init();

    if(strstr(line,"<")){
        char* tokens;
        char* str = strdup(line);
        tokens = strtok(str, "<");
        tokens = strtok(NULL, "<");
        tokens = strtok(tokens, " \t\n");
        tokens = strtok(tokens, "|<>&");
        cmd->redirect_out_path = tokens;
        str = replace(line,tokens,"");
        line = strdup(str);
        free(str);
    }

    if(strstr(line,">")){
        char* tokens;
        char* str = strdup(line);
        tokens = strtok(str, ">");
        tokens = strtok(NULL, ">");
        tokens = strtok(tokens, " \t\n");
        tokens = strtok(tokens, "|<>&");
        cmd->redirect_in_path = tokens;
        str = replace(line,tokens,"");
        line = strdup(str);
        free(str);
    }

    char* tokens;
    char* s;
    int i=0;

    while((tokens = strtok_r(line, " \t\n",&line))){
        while((s = strtok_r(tokens, "|<>&", &tokens))){
            cmd->command_args[i] = s;
        }
        i++;
    }

    *cline = cmd;
}

//Initializes given cline pointer, populates elements with data from token. 
// void parse(char *token, struct pipeline_command** cline){
//     char* redirect=NULL;
//     char* arg = NULL;
//     char* cpy = strdup(token);
//     int i=0;
//     struct pipeline_command* temp = pipeline_command_init();

//     arg = strtok(token, " ");
//     while(arg != NULL && i < MAX_ARGV_LENGTH-1){
//         temp->command_args[i] = strdup(arg);
//         i++;
//         arg = strtok(NULL," ");
//     }

//     redirect = NULL;
//     while((redirect = strpbrk(cpy,"<>"))){
//         if(strcmp(redirect,"<")==0){
//             temp->redirect_in_path = strdup(redirect+1);
//         }
//         else if(strcmp(redirect,">")==0){
//             temp->redirect_out_path = strdup(redirect+1);
//         }
//         *redirect = ' ';
//     }
//     *cline = temp; 
// }

void deleteamp(char *str, char c){
    char *len = strchr(str,c);
    if(len){
        memmove(len,len+1,strlen(len));
    }
}

struct pipeline *pipeline_build(const char *command_line){
    struct pipeline* pline;
    struct pipeline_command* cline;
    char* token; 
    char* line; 
    char* save;
    int len;

    //Initialization 
    len = strcspn(command_line,"\n");
    line = strdup(command_line);
    line[len] = '\0';
    pline = pipeline_init();
    cline = pipeline_command_init();

    if(strstr(line,"&")){
        pline->is_background = true;
        deleteamp(line,'&');
    }

    save = strdup(line);
    token = strtok_r(line,"|",&save);

    do{
        parse(token,&cline);
        link(pline,cline);
    }while((token = strtok_r(NULL,"|",&save)));

    free(line);

    return pline;
}

void pipeline_free(struct pipeline *pipeline){
    struct pipeline_command* cline = pipeline->commands;
    int i;

    while(cline != NULL){
        for(i = 0; cline->command_args[i] != NULL; i++){
            free(cline->command_args[i]);
        }
        free(cline->command_args);
        free(cline->redirect_in_path);
        free(cline->redirect_out_path);
        pipeline->commands = cline->next;
        cline = pipeline->commands;
    }

    free(pipeline);
}

