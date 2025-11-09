# MyShell — Custom Shell Implementation (Capstone Project)

**Author:** Swayamdeep Nanda  
**Course:** Linux System Programming / Capstone  
**Date:** 09-Nov-2025  
**Repository:** https://github.com/SwayamdeepNanda/MyShell

---

## 1. Title
MyShell — A Lightweight, Portable Command Shell implemented in C

---

## 2. Objective
Build a production-minded custom shell in C that demonstrates core OS concepts:
process creation and control, command parsing, built-in commands, background jobs,
and POSIX-style piping and redirection (Linux). The implementation emphasizes
modularity, safety, and clear documentation suitable for academic evaluation.

---

## 3. Project Structure (files listed)
- `src/main.c` — core shell engine (parsing, builtins, exec, piping/redirection)
- `Makefile` — developer build (debug/release/test/valgrind)
- `CMakeLists.txt` — alternative, cross-platform build support
- `README.md` — project overview and usage
- `docs/design/architecture.md` — high-level architecture notes
- `scripts/` — helper scripts:
  - `format_all.sh`, `run_checks.sh`, `run_valgrind.sh`
- `.clang-format`, `.editorconfig`, `.gitignore` — tooling and hygiene files

---

## 4. Code (key file excerpt)
Below is the top-level implementation summary and main function (full code is in `src/main.c`).

```c
// main.c — simplified excerpt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
  #include <unistd.h>
  #include <sys/wait.h>
  #include <signal.h>
  #include <fcntl.h>
#endif


```c
// Tokenizer, builtins (cd, pwd, exit), background (&) support,
// pipeline & redirection logic for POSIX, Windows fallbacks in place.

int main(void) {
    char *line = NULL;
    size_t bufsize = 0;
    while (1) {
        printf("myshell> ");
        fflush(stdout);
        if (getline(&line, &bufsize, stdin) == -1) break;
        // parse line, execute (pipeline/redirection or simple exec)
    }
    free(line);
    return 0;
}

