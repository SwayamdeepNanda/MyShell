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

#define TOK_DELIM " \t\r\n\a"

#ifdef _WIN32
void sigint_handler(int sig) { (void)sig; }
#else
void sigint_handler(int sig) { (void)sig; write(STDOUT_FILENO, "\nmyshell> ", 10); }
#endif

char **tokenize_input(char *line) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    if (!tokens) { fprintf(stderr, "myshell: allocation error\n"); exit(EXIT_FAILURE); }
    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += 64;
            char **tmp = realloc(tokens, bufsize * sizeof(char*));
            if (!tmp) { free(tokens); fprintf(stderr, "myshell: allocation error\n"); exit(EXIT_FAILURE); }
            tokens = tmp;
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int is_builtin(char **args) {
    if (args == NULL || args[0] == NULL) return 0;
    return (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0 || strcmp(args[0], "pwd") == 0);
}

int run_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        char *dir = args[1];
        if (!dir) dir = getenv("HOME");
        if (!dir) dir = ".";
#ifdef _WIN32
        if (chdir(dir) != 0) { perror("cd"); return -1; }
#else
        if (chdir(dir) != 0) { perror("cd"); return -1; }
#endif
        return 0;
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            return 0;
        } else { perror("pwd"); return -1; }
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    return -1;
}

void execute_command(char **args) {
    if (args == NULL || args[0] == NULL) return;

    int background = 0;
    /* detect trailing & and remove it */
    int i = 0;
    while (args[i]) i++;
    if (i > 0 && strcmp(args[i-1], "&") == 0) {
        background = 1;
        args[i-1] = NULL;
    }

    if (is_builtin(args)) {
        run_builtin(args);
        return;
    }

#ifdef _WIN32
    if (background) {
        intptr_t rc = _spawnvp(_P_NOWAIT, args[0], (const char * const *)args);
        if (rc == -1) perror("myshell");
        else printf("[bg] pid %ld\n", (long)rc);
    } else {
        intptr_t rc = _spawnvp(_P_WAIT, args[0], (const char * const *)args);
        if (rc == -1) perror("myshell");
    }
#else
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        if (background) {
            printf("[bg] pid %d\n", pid);
            /* do not wait for background job */
        } else {
            int status;
            pid_t wpid;
            do {
                wpid = waitpid(pid, &status, 0);
            } while (wpid > 0 && !WIFEXITED(status) && !WIFSIGNALED(status));
        }
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
        args = tokenize_input(line);
        execute_command(args);
        free(args);
    }
    free(line);
    return 0;
}
