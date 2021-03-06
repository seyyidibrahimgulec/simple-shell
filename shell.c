#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define ARRAY_SIZE 128
#define ARGUMENT_SIZE 128

void startShell();
char* readLine();
char** parseCommand(char*);
void executeCommand(char**);
int isPipeExist(char*);
char** seperatePipe(char*);
void executeCommandWithPipe(char**);

int main(int argc, char *argv[])
{
    /*
      1. adım satırı oku
      2. adım satırı parçalara böl
      3. adım komutu çalıştır
      4. adım 1. adıma geri dön
     */
    startShell();
    return 0;
}

void startShell() {
    char *line;
    char **command;
    char **commands;
    int exit_program = 0;
    printf("Shell is starting ...\n");
    while(!exit_program) {
        printf("> ");
        line = readLine();
        if(line != NULL) {
            if(!strcmp(line, "exit")) {// exit komutu girilirse program sonlanır
                exit_program = 1;
                printf("Shell is closing ...\n");
            } else {
                if(!isPipeExist(line)) {
                    command = parseCommand(line);
                    executeCommand(command);
                    free(command);
                    free(line);
                } else {
                    commands = seperatePipe(line);
                    executeCommandWithPipe(commands);
                }
            }
        }
    }
}

char* readLine() {
    char *buffer;

    buffer = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    if(buffer == NULL) {
        printf("Allocation error!\n");
        exit(EXIT_FAILURE);
    }
    if(fgets(buffer, BUFFER_SIZE, stdin) == NULL && ferror(stdin)) {
        printf("Error reading line!\n");
        exit(EXIT_FAILURE);
    }
    if(buffer[0] == '\n') {
        return NULL;
    }
    buffer[strlen(buffer) - 1] = '\0';

    return buffer;
}

char** parseCommand(char *cmd) {
    char** command;
    char *arg;
    int i = 0, j;

    command = (char**) malloc(sizeof(char*) * ARRAY_SIZE);
    if(command == NULL) {
        printf("Allocation error!\n");
        exit(EXIT_FAILURE);
    }
    for(i=0; i < ARRAY_SIZE; i++) {
        command[i] = (char*) malloc(sizeof(char) * ARGUMENT_SIZE);
    }

    i = 0;
    arg = strtok(cmd, " \0");

    command[i] = arg;

    while((arg = strtok(NULL, " \0")) != NULL) {
        i++;
        command[i] = arg;
    }
    i++;

    command[i] = NULL;

    return command;
}

void executeCommand(char **command) {
    pid_t pid;

    pid = fork();

    if(pid < 0) {
        printf("Fork error!\n");
    } else if(pid == 0) {
        if(!strcmp(command[0], "cd")) {
            chdir(command[1]);
        } else {
            execvp(command[0], command);
        }
    } else {
        wait(NULL);
    }
}

int isPipeExist(char *line) {
    int i = 0;

    while(line[i]) {
        if(line[i] == '|')
            return 1;
        i++;
    }
    return 0;
}

char** seperatePipe(char *line) {
    char **commands, *arg;
    int i = 0;

    commands = (char**) malloc(sizeof(char*) * 2);
    if(commands == NULL) {
        printf("Allocation error!\n");
        exit(EXIT_FAILURE);
    }
    for(i=0; i < ARRAY_SIZE; i++) {
        commands[i] = (char*) malloc(sizeof(char) * ARGUMENT_SIZE);
    }

    arg = strtok(line, "|");
    commands[0] = arg;

    arg = strtok(NULL, "\0");
    commands[1] = arg ;

    return commands;
}

void executeCommandWithPipe(char **commands) {
    char **command1, **command2;
    int pipeArr[2];
    pid_t pid1;
    pid_t pid2;

    command1 = parseCommand(commands[0]);
    command2 = parseCommand(commands[1]);

    pipe(pipeArr);

    pid1 = fork();
    if(pid1 < 0) {
        printf("Fork Error\n");
    } else if(pid1 == 0) {
        close(STDOUT_FILENO);
        dup(pipeArr[1]);
        close(pipeArr[0]);
        close(pipeArr[1]);

        execvp(command1[0], command1);

    }
    pid2 = fork();
    if(pid2 < 0) {
        printf("Fork Error\n");
    } else if(pid2 == 0) {
        close(STDIN_FILENO);
        dup(pipeArr[0]);
        close(pipeArr[1]);
        close(pipeArr[0]);

        execvp(command2[0], command2);
    }

    close(pipeArr[0]);
    close(pipeArr[1]);
    wait(NULL);
    wait(NULL);
}
