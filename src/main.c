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
  #include <fcntl.h>
#endif

#define TOK_DELIM " \t\r\n\a"

#ifdef _WIN32
void sigint_handler(int sig) { (void)sig; }
#else
void sigint_handler(int sig) { (void)sig; write(STDOUT_FILENO, "\nmyshell> ", 10); }
#endif

/* Minimal job table for background jobs (simple, fixed-size) */
#ifndef _WIN32
#define MAX_JOBS 64
struct job {
    pid_t pid;
    int id;
    char *cmd;
    int running; /* 1 running, 0 finished */
} jobs[MAX_JOBS];
int next_job_id = 1;

void add_job(pid_t pid, const char *cmd) {
    for (int i=0;i<MAX_JOBS;i++){
        if (jobs[i].pid==0){
            jobs[i].pid = pid;
            jobs[i].id = next_job_id++;
            jobs[i].cmd = strdup(cmd ? cmd : "");
            jobs[i].running = 1;
            return;
        }
    }
}

void remove_job_by_pid(pid_t pid) {
    for (int i=0;i<MAX_JOBS;i++){
        if (jobs[i].pid==pid) {
            free(jobs[i].cmd);
            jobs[i].pid = 0; jobs[i].id = 0; jobs[i].running = 0;
            jobs[i].cmd = NULL;
            return;
        }
    }
}

void mark_job_done(pid_t pid) {
    for (int i=0;i<MAX_JOBS;i++){
        if (jobs[i].pid==pid) { jobs[i].running = 0; return; }
    }
}

void builtin_jobs() {
    for (int i=0;i<MAX_JOBS;i++){
        if (jobs[i].pid) {
            printf("[%d] %d %s %s\n", jobs[i].id, (int)jobs[i].pid,
                   jobs[i].running ? "Running" : "Done", jobs[i].cmd ? jobs[i].cmd : "");
        }
    }
}

int find_job_pid_by_id(int id) {
    for (int i=0;i<MAX_JOBS;i++){
        if (jobs[i].id == id) return (int)jobs[i].pid;
    }
    return -1;
}

void reap_background_jobs_nonblocking() {
    int status = 0;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        /* mark or remove job and notify user */
        for (int i=0;i<MAX_JOBS;i++){
            if (jobs[i].pid == pid) {
                jobs[i].running = 0;
                printf("\n[done] %d %s\nmyshell> ", (int)pid, jobs[i].cmd ? jobs[i].cmd : "");
                fflush(stdout);
                remove_job_by_pid(pid);
                break;
            }
        }
    }
}
#endif

/* simple whitespace-aware tokenizer (respects basic quoting handled by strtok in this context) */
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

/* trim whitespace wrapper */
static char *trim(char *s) {
    while (*s && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) { *end = '\0'; end--; }
    return s;
}

int is_builtin(char **args) {
    if (args == NULL || args[0] == NULL) return 0;
    return (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0 || strcmp(args[0], "pwd") == 0 || strcmp(args[0], "jobs") == 0 || strcmp(args[0], "fg") == 0);
}

int run_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        char *dir = args[1];
        if (!dir) dir = getenv("HOME");
        if (!dir) dir = ".";
        if (chdir(dir) != 0) { perror("cd"); return -1; }
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
#ifndef _WIN32
    else if (strcmp(args[0], "jobs") == 0) {
        builtin_jobs();
        return 0;
    } else if (strcmp(args[0], "fg") == 0) {
        if (!args[1]) { fprintf(stderr, "fg: missing job id\n"); return -1; }
        int id = atoi(args[1]);
        if (id <= 0) { fprintf(stderr, "fg: invalid id\n"); return -1; }
        int pid = find_job_pid_by_id(id);
        if (pid <= 0) { fprintf(stderr, "fg: no such job\n"); return -1; }
        /* bring to foreground by waiting for it */
        if (kill(pid, SIGCONT) == -1) perror("fg: kill(SIGCONT)");
        int status;
        if (waitpid(pid, &status, 0) == -1) perror("waitpid");
        remove_job_by_pid(pid);
        return 0;
    }
#endif
    return -1;
}

#ifndef _WIN32
/* execute a pipeline of commands with redirection support */
int execute_pipeline_from_line(char *line) {
    /* split by '|' into command strings (simple split, not fully quote-aware) */
    char *parts[64];
    int nparts = 0;
    char *saveptr = NULL;
    char *p = strtok_r(line, "|", &saveptr);
    while (p && nparts < 64) {
        parts[nparts++] = trim(p);
        p = strtok_r(NULL, "|", &saveptr);
    }
    if (nparts == 0) return 0;

    int prev_fd = -1;
    int pipefd[2];
    pid_t pgid = 0;
    pid_t last_pid = 0;

    for (int i = 0; i < nparts; ++i) {
        /* parse redirection tokens inside this part */
        char *cmdcopy = strdup(parts[i]);
        char *infile = NULL;
        char *outfile = NULL;
        int append = 0;

        /* handle > >> and < by simple token scan */
        char *tok_save = NULL;
        char *tokens_for_parse[128];
        int tcount = 0;
        char *tk = strtok_r(cmdcopy, TOK_DELIM, &tok_save);
        while (tk && tcount < 127) {
            if (strcmp(tk, ">") == 0) {
                tk = strtok_r(NULL, TOK_DELIM, &tok_save);
                if (tk) { outfile = strdup(tk); append = 0; tk = strtok_r(NULL, TOK_DELIM, &tok_save); continue; }
                else break;
            } else if (strcmp(tk, ">>") == 0) {
                tk = strtok_r(NULL, TOK_DELIM, &tok_save);
                if (tk) { outfile = strdup(tk); append = 1; tk = strtok_r(NULL, TOK_DELIM, &tok_save); continue; }
                else break;
            } else if (strcmp(tk, "<") == 0) {
                tk = strtok_r(NULL, TOK_DELIM, &tok_save);
                if (tk) { infile = strdup(tk); tk = strtok_r(NULL, TOK_DELIM, &tok_save); continue; }
                else break;
            } else {
                tokens_for_parse[tcount++] = strdup(tk);
            }
        }
        tokens_for_parse[tcount] = NULL;

        /* setup pipe if not last */
        if (i < nparts - 1) {
            if (pipe(pipefd) == -1) { perror("pipe"); return -1; }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        } else if (pid == 0) {
            /* child */
            if (pgid == 0) pgid = getpid();
            setpgid(0, pgid);

            /* input from previous pipe */
            if (prev_fd != -1) {
                if (dup2(prev_fd, STDIN_FILENO) == -1) { perror("dup2"); _exit(127); }
            }

            /* output to pipe if not last */
            if (i < nparts - 1) {
                close(pipefd[0]);
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) { perror("dup2"); _exit(127); }
                close(pipefd[1]);
            }

            /* redirections */
            if (infile) {
                int fd = open(infile, O_RDONLY);
                if (fd == -1) { perror("open infile"); _exit(127); }
                if (dup2(fd, STDIN_FILENO) == -1) { perror("dup2 infile"); _exit(127); }
                close(fd);
            }
            if (outfile) {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int fd = open(outfile, flags, 0644);
                if (fd == -1) { perror("open outfile"); _exit(127); }
                if (dup2(fd, STDOUT_FILENO) == -1) { perror("dup2 outfile"); _exit(127); }
                close(fd);
            }

            /* restore default signals */
            signal(SIGINT, SIG_DFL);

            /* exec */
            if (tokens_for_parse[0]) {
                execvp(tokens_for_parse[0], tokens_for_parse);
                perror("execvp");
            }
            _exit(127);
        } else {
            /* parent */
            if (pgid == 0) pgid = pid;
            setpgid(pid, pgid);
            last_pid = pid;

            /* close previous read end */
            if (prev_fd != -1) close(prev_fd);
            /* close write end in parent and keep read end for next */
            if (i < nparts - 1) {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
            /* free parse allocations */
            for (int j=0;j<tcount;j++) free(tokens_for_parse[j]);
            free(cmdcopy);
            if (infile) free(infile);
            if (outfile) free(outfile);
        }
    }

    /* wait for pipeline to finish (foreground) */
    int status;
    pid_t w;
    do {
        w = waitpid(-pgid, &status, 0);
    } while (w > 0 && !WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}
#endif

/* fallback single-command execution (used on Windows and simple cases) */
void execute_simple_command(char **args) {
    if (args == NULL || args[0] == NULL) return;

    if (is_builtin(args)) { run_builtin(args); return; }

#ifdef _WIN32
    intptr_t rc = _spawnvp(_P_WAIT, args[0], (const char * const *)args);
    if (rc == -1) perror("myshell");
#else
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) perror("myshell");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        int status;
        pid_t w;
        do {
            w = waitpid(pid, &status, 0);
        } while (w > 0 && !WIFEXITED(status) && !WIFSIGNALED(status));
    }
#endif
}

#ifndef _WIN32
/* execute a command in background (non-blocking) */
void execute_in_background(char **args, const char *cmdline_copy) {
    pid_t pid = fork();
    if (pid == 0) {
        /* child */
        if (execvp(args[0], args) == -1) {
            perror("myshell");
            _exit(127);
        }
        _exit(0);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        /* parent: register job and do not wait */
        add_job(pid, cmdline_copy);
        printf("[%d] %d\n", next_job_id-1, (int)pid);
    }
}
#endif

/* decide whether line contains a pipeline */
int contains_pipe(const char *line) {
    return strchr(line, '|') != NULL;
}

int main(void) {
    char *line = NULL;
    size_t bufsize = 0;
    char **args;

#ifndef _WIN32
    signal(SIGINT, sigint_handler);
#endif

    while (1) {
#ifndef _WIN32
        /* reap any finished background jobs without blocking */
        reap_background_jobs_nonblocking();
#endif

        printf("myshell> ");
        fflush(stdout);
        if (getline(&line, &bufsize, stdin) == -1) { printf("\n"); break; }
        char *linetrim = trim(line);
        if (linetrim[0] == 0) continue;

        if (contains_pipe(linetrim)) {
#ifndef _WIN32
            /* make a writable copy since execute_pipeline_from_line uses strtok_r */
            char *copy = strdup(linetrim);
            execute_pipeline_from_line(copy);
            free(copy);
#else
            printf("piping/redirection not supported on this Windows build\n");
#endif
            continue;
        }

        /* copy the trimmed line for job registration if needed */
        char *cmdline_copy = NULL;
#ifndef _WIN32
        cmdline_copy = strdup(linetrim);
#endif

        args = tokenize_input(linetrim);

        /* detect simple I/O redirection without pipes (>, >>, <) */
        int need_simple_redir = 0;
        for (int i=0; args[i]; ++i) {
            if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0) {
                need_simple_redir = 1;
                break;
            }
        }

        /* detect background '&' at end */
        int background = 0;
        int last = 0;
        while (args[last]) last++;
        if (last > 0 && strcmp(args[last-1], "&") == 0) {
            background = 1;
            args[last-1] = NULL; /* remove & */
        }

        if (need_simple_redir && !contains_pipe(linetrim)) {
#ifndef _WIN32
            /* handle single-command redirection by building a temporary pipeline-like call */
            /* very small helper: rebuild argv excluding redir tokens and perform fork+dup2 */
            char *infile = NULL; char *outfile = NULL; int append = 0;
            int w=0;
            for (int i=0; args[i]; ++i) {
                if (strcmp(args[i], "<") == 0) { infile = args[i+1]; args[i] = NULL; break; }
                if (strcmp(args[i], ">>") == 0) { outfile = args[i+1]; append = 1; args[i] = NULL; break; }
                if (strcmp(args[i], ">") == 0) { outfile = args[i+1]; append = 0; args[i] = NULL; break; }
            }
            pid_t pid = fork();
            if (pid == 0) {
                if (infile) {
                    int fd = open(infile, O_RDONLY);
                    if (fd == -1) { perror("open infile"); _exit(127); }
                    if (dup2(fd, STDIN_FILENO) == -1) { perror("dup2"); _exit(127); }
                }
                if (outfile) {
                    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                    int fd = open(outfile, flags, 0644);
                    if (fd == -1) { perror("open outfile"); _exit(127); }
                    if (dup2(fd, STDOUT_FILENO) == -1) { perror("dup2"); _exit(127); }
                }
                if (is_builtin(args)) { run_builtin(args); _exit(0); }
                if (execvp(args[0], args) == -1) { perror("myshell"); _exit(127); }
            } else if (pid < 0) {
                perror("fork");
            } else {
                int status; waitpid(pid, &status, 0);
            }
#else
            printf("redirection not supported on this Windows build\n");
#endif
            free(args);
            if (cmdline_copy) free(cmdline_copy);
            continue;
        }

        /* builtins */
        if (is_builtin(args)) {
            run_builtin(args);
            free(args);
            if (cmdline_copy) free(cmdline_copy);
            continue;
        }

#ifndef _WIN32
        if (background) {
            /* run in background: fork and do not wait */
            execute_in_background(args, cmdline_copy);
            free(args);
            if (cmdline_copy) free(cmdline_copy);
            continue;
        }
#endif

        /* foreground or Windows simple exec */
        execute_simple_command(args);
        free(args);
        if (cmdline_copy) free(cmdline_copy);
    }

    free(line);
    return 0;
}
