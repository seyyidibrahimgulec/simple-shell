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
    int exit = 0;
    printf("Shell is starting ...\n");
    while(!exit) {
        printf("> ");
        line = readLine();
        if(line != NULL) {
            if(!strcmp(line, "exit")) {// exit komutu girilirse program sonlanır
                exit = 1;
                printf("Shell is closing ...\n");
            } else {
                command = parseCommand(line);
                executeCommand(command);
                free(command);
                free(line);
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
        execvp(command[0], command);
    } else {
        wait(NULL);
    }
}
