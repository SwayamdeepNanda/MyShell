```markdown
# 🧩 MyShell — Lightweight, Portable Command Shell in C

**Author:** Swayamdeep Nanda  
**Course:** Linux System Programming / Capstone  
**Date:** 09-Nov-2025  
**Repository:** [MyShell on GitHub](https://github.com/SwayamdeepNanda/MyShell)

---

## 🧠 Project Overview

**MyShell** is a lightweight, modular command shell implemented in **C**, designed to demonstrate deep understanding of **UNIX process control**, **system calls**, and **command execution**.  
It supports **built-in commands**, **directory navigation**, **piping**, **I/O redirection**, and **cross-platform execution (Windows + POSIX)** — built for both **academic rigor** and **practical exploration**.

This project was developed as part of a **Linux System Programming / Capstone** course to showcase real-world shell internals and modular system design.

---

## 🎯 Objectives

- Build a production-style shell implementation from scratch in C.  
- Demonstrate process creation, management, and inter-process communication.  
- Include modular and safe design patterns (tokenizer, parser, built-ins, executor).  
- Provide cross-platform support and clean documentation.  
- Include reproducible builds using **Makefile** and **CMake**.

---

## 🧩 Features

✅ **Core Built-ins:**  
`cd`, `pwd`, `exit`, and `echo` with proper path and environment handling.  

✅ **Execution Engine:**  
Handles command parsing, process forking, and exec-family calls.

✅ **Piping and Redirection (Linux):**  
Supports `|`, `>`, `>>`, `<` operators and multi-command pipelines.

✅ **Cross-Platform Fallback:**  
Windows build executes simple commands using `_spawnvp()` safely.

✅ **Formatting, Testing, and Memory Checking:**  
- `scripts/format_all.sh` — auto-code formatter (clang-format)  
- `scripts/run_valgrind.sh` — memory safety checks (Linux only)  
- `scripts/run_checks.sh` — placeholder for static analysis, linting

---

## 🏗️ Directory Structure

```

MyShell/
│
├── src/               # Core implementation (main.c)
├── include/           # Header files (future modular expansion)
├── docs/              # Architecture & report documentation
│   ├── design/
│   │   └── architecture.md
│   └── report.md
├── scripts/           # Utility scripts (format, valgrind, tests)
├── tests/             # Future unit tests
├── Makefile           # Build automation
├── CMakeLists.txt     # Alternative cross-platform build
└── README.md          # Project overview

````

---

## ⚙️ Build Instructions

### On Linux (recommended)
```bash
make
./bin/myshell
````

### On Windows (MinGW)

```bash
gcc -std=gnu11 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_GNU_SOURCE src/main.c -o bin/myshell.exe
./bin/myshell.exe
```

---

## 🧪 Example Commands

```bash
echo hello
pwd
cd ..
ls
echo "hi there" > test.txt && cat test.txt
ls | grep src
```

---

## 🧱 Architecture Highlights

The codebase follows a clean modular structure emphasizing:

* **Tokenizer** — breaks user input into executable tokens.
* **Executor** — handles forking, execvp, and redirection.
* **Built-ins** — minimal yet robust internal commands.
* **Pipeline Manager (POSIX)** — executes complex multi-stage commands.

Architecture summary: [`docs/design/architecture.md`](docs/design/architecture.md)
Detailed explanation and code excerpt: [`docs/report.md`](docs/report.md)

---

## 📘 Academic Value

This project bridges **OS theory** and **system-level programming practice**.
Students and evaluators can trace process flow from parsing to execution, gaining a real-world understanding of how shells manage input/output, redirection, and concurrency.

---

## 👏 Acknowledgements

Special thanks to instructors, peers, and open-source contributors who inspired the modular and test-driven approach of **MyShell**.

---

> “A shell is not just a command interpreter — it’s the art of orchestrating processes.”

```
```
