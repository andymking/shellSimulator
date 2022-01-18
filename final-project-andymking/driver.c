#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shell.h"

int main(){
    bool done = false;
    int cmd_cnt = 0;

    char shell_cmd[100];

    command_t command;

    // -------------------------------------------
    // Go ahead and allocate memory
    // makes things less complicated :)
    
    // ^ I cannot emphasize enough that this is like really bad,
    // but I don't really have time to change it -Eric
    command.path = malloc(sizeof(char)*100);
    command.argv = malloc(sizeof(char*)*50);
    for(int i = 0; i < 50; i++) command.argv[i] = malloc(sizeof(char)*100);

    while(!done){
        printf("thsh(%d)$ ", cmd_cnt++);
        fgets(shell_cmd, 500, stdin);
        
        parse(shell_cmd, &command);

        if ((command.argc > 0) && strcmp(command.path, "exit") == 0){
            done = true;
        }else if(command.argc == ERROR){ 
            fprintf(stderr, "%s: command not found!\n", command.path);
        }else if(command.argc > 0){
            if(is_builtin(&command)){
                if (do_builtin(&command) != SUCCESSFUL){
                    fprintf(stderr, "%s command failed: requested folder [%s] does not exist!\n", command.path, command.argv[0]);
                }
            }else if(execute(&command) != SUCCESSFUL){
                fprintf(stderr, "%s command failed!\n", command.path);
            }
        }
    }

    cleanup(&command);

    return 0;
} // end main function
