// PID: 730233100
// I pledge the COMP 211 honor code.

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shell.h"

const char *BUILT_IN_COMMANDS[] = {"cd", "exit", NULL};
const char *PATH_SEPARATOR = ":";
char *path_pointer;

void parse(char *line, command_t *p_cmd){
    char *token;
    int argc = 0;
    
    while((token = strtok_r(line, " \n\t\r", &line)) != NULL){
        strcpy(p_cmd->argv[argc], token);
        argc++;
    }

    strcpy(p_cmd->path, p_cmd->argv[0]);
    p_cmd->argc = is_builtin(p_cmd) || find_fullpath(p_cmd) ? argc : ERROR;
} // end parse function

bool find_fullpath(command_t *p_cmd){
    // TODO: complete this function!

    // Path environmental variables. Looks something like:
    // /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/mnt/learncli/bin
    char path[300];
    strcpy(path, getenv("PATH"));

    char *token = strtok(path, PATH_SEPARATOR);

    while(token != NULL){
        // Here, token is a directory on the path, such as "/usr/local/bin"
        // You'll want to use some functions in string.h to make a (potential) full path.
        // For example, for `ls`, `/usr/local/bin/ls` is a potential full path.
        // Then, you want to use the `stat` function to tell if it is the full path.

        // Debug:
        // printf("%s\n", token);
        struct stat buffer;
        int exists;
        char file_or_dir[300]; // string representing the full path of a file/dir
        char ch = '/';
        strcpy(file_or_dir, token); // Copy token to file path variable.
        strncat(file_or_dir, &ch, 1); // Concat '/' to the end of file path for formatting.
        strcat(file_or_dir, p_cmd->path); // Concat the command to the file path variable.
        exists = stat(file_or_dir, &buffer);
        if(exists == 0 && (S_IFREG & buffer.st_mode)){
            // File exists
            // Copy path string to p_cmd->path.
            strcpy(p_cmd->path, file_or_dir);
            return true;
        }
        token = strtok(NULL, PATH_SEPARATOR);
    }
    return false;
} // end find_fullpath function

int execute(command_t *p_cmd){
    // TODO: complete this function!
    int status = SUCCESSFUL;
    pid_t child_pid = fork(); // Child PID stored.
    if (child_pid == -1) {
        perror("Fork was unsuccessful!\n");
    }
    else if (child_pid == 0) {
        // We know this is the child process.
        // Execute path_pointer, dealloc path_pointer, and exit.
        p_cmd->argv[p_cmd->argc] = NULL; // NULL must be last value of arg array for execv.
        execv(p_cmd->path, p_cmd->argv); // Execute the command, clobber child.
    }
    else {
        // If the process is the parent, wait on the child.
        wait(NULL);
    }
    return status;
} // end execute function


bool is_builtin(command_t *p_cmd){
    int i = 0;

    while(BUILT_IN_COMMANDS[i] != NULL){
        if(strcmp(p_cmd->path, BUILT_IN_COMMANDS[i]) == 0){
            return true;
        }

        i++;
    }
    return false;
} // end is_builtin function


int do_builtin(command_t *p_cmd){
    // For whatever reason, `exit` is handled in driver.c.
    // This just handles `cd`

    struct stat buff;
    int status = ERROR;

    if(p_cmd->argc == 1){

        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental 
        // variable

        status = chdir(getenv("HOME"));
    }else if((stat(p_cmd->argv[1], &buff) == 0 && (S_IFDIR & buff.st_mode))){
        // -----------------------
        // cd with one arg 
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir(p_cmd->argv[1]);
    }

    return status;
} // end do_builtin function

void cleanup(command_t *p_cmd){
    int i = 0;

    while(p_cmd->argv[i] != NULL){
        free(p_cmd->argv[i]);
        i++;
    }

    free(p_cmd->argv);
    free(p_cmd->path);	
} // end cleanup function
