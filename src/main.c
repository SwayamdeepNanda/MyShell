#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <process.h>
  #include <windows.h>
  typedef intptr_t pid_t;
#else
  #include <unistd.h>
  #include <sys/wait.h>
  #include <signal.h>
#endif

#define INPUT_SIZE 1024
#define TOK_DELIM " \t\r\n\a"

#ifdef _WIN32
void sigint_handler(int sig) { (void)sig; /* no-op on Windows console */ }
#else
void sigint_handler(int sig) {
    (void)sig;
    write(STDOUT_FILENO, "\nmyshell> ", 10);
}
#endif

char **tokenize_input(char *line) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += 64;
            char **tmp = realloc(tokens, bufsize * sizeof(char*));
            if (!tmp) {
                free(tokens);
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
            tokens = tmp;
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void execute_command(char **args) {
    if (args == NULL || args[0] == NULL) return;

    if (strcmp(args[0], "exit") == 0) exit(0);

#ifdef _WIN32
    /* spawn and wait for command on Windows */
    /* _spawnvp expects const char * const *; cast is safe for argv usage here */
    intptr_t rc = _spawnvp(_P_WAIT, args[0], (const char * const *)args);
    if (rc == -1) {
        perror("myshell");
    }
#else
    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        int status;
        pid_t wpid;
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (wpid > 0 && !WIFEXITED(status) && !WIFSIGNALED(status));
    }
#endif
}

int main(void) {
    char *line = NULL;
    size_t bufsize = 0;
    char **args;

#ifndef _WIN32
    signal(SIGINT, sigint_handler);
#endif

    while (1) {
        printf("myshell> ");
        fflush(stdout);
        if (getline(&line, &bufsize, stdin) == -1) {
            printf("\n");
            break;
        }
        /* remove trailing newline is handled by strtok when tokenizing */
        args = tokenize_input(line);
        execute_command(args);
        free(args);
    }
    free(line);
    return 0;
}
