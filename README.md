# 🧩 MyShell — A Lightweight, Portable Command Shell in C

**Author:** Swayamdeep Nanda  
**Course:** Linux System Programming / Capstone  
**Date:** 09-Nov-2025  
**Repository:** [MyShell on GitHub](https://github.com/SwayamdeepNanda/MyShell)

---

## 🧠 Overview

**MyShell** is a lightweight and modular command-line shell built entirely in **C**, demonstrating core **Operating System** concepts —  
**process creation**, **command parsing**, **built-ins**, **piping**, and **I/O redirection** across both **Linux** and **Windows (MinGW)** environments.  

It’s designed for **academic evaluation** and **real-world insight** into shell internals, providing clean, maintainable, and well-documented source code.

---

## 🎯 Objectives

- Build a **production-style custom shell** in C.  
- Demonstrate **process creation, control, and IPC (Inter-Process Communication)**.  
- Implement **modular components** — tokenizer, parser, executor, and built-ins.  
- Support **cross-platform execution** with safe fallbacks for Windows.  
- Deliver **professional documentation** and reproducible builds.

---

## ⚙️ Core Features

### ✅ Built-in Commands  
Supports `cd`, `pwd`, `exit`, and `echo` with proper environment handling.

### ⚙️ Execution Engine  
Implements `fork()`, `execvp()`, and related system calls for command execution.

### 🔁 Piping & Redirection (POSIX)  
Handles complex commands with `|`, `>`, `>>`, `<`, and multiple chained operations.

### 💻 Cross-Platform Support  
Windows-compatible using `_spawnvp()` for basic execution.

### 🧰 Development Tools  
- `scripts/format_all.sh` — automatic code formatter (clang-format)  
- `scripts/run_valgrind.sh` — memory leak analysis (Linux only)  
- `scripts/run_checks.sh` — linting and static analysis placeholder  

---

## 🏗️ Directory Structure
```
MyShell/
│
├── docs/ # Documentation
│ ├── design/
│ │ └── architecture.md # High-level architecture overview
│ └── report.md # Project report / summary
│
├── scripts/ # Helper scripts for formatting & testing
│ ├── format_all.sh
│ ├── run_checks.sh
│ └── run_valgrind.sh
│
├── src/ # Core source code
│ └── main.c
│
├── .clang-format # Code formatting rules
├── .editorconfig # Editor consistency settings
├── .gitignore # Git ignore rules
├── CMakeLists.txt # Cross-platform build config
├── Makefile # Build automation
└── README.md # Project overview and documentation

```
---

## 🧪 Example Usage

```bash
echo hello
pwd
cd ..
ls
echo "hi there" > test.txt && cat test.txt
ls | grep src
```


## 🛠️ Build & Run Instructions
On Linux
```
make
./bin/myshell
```
On Windows (MinGW)
```
gcc -std=gnu11 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_GNU_SOURCE src/main.c -o bin/myshell.exe
./bin/myshell.exe
```
---

## **🧱 Architecture Summary**

The design follows a **modular architecture** for readability and maintainability:

| 🧩 **Module** | 🧠 **Responsibility** |
|:--------------|:----------------------|
| **Tokenizer** | Splits user input into executable tokens. |
| **Executor** | Manages `fork()`, `execvp()`, redirection, and background jobs. |
| **Built-ins** | Implements core commands like `cd`, `pwd`, and `exit`. |
| **Pipeline Manager** | Coordinates multi-stage commands and handles redirection. |

📄 **Detailed Design:** [`docs/design/architecture.md`](docs/design/architecture.md)  
📘 **Implementation Report:** [`docs/report.md`](docs/report.md)

---

## **🎓 Academic Relevance**

MyShell bridges theory and system-level programming practice, enabling students to explore:

How OS kernels handle process creation and I/O

How command interpreters parse and execute pipelines

How modular shell design improves extensibility

Evaluators can trace execution flow clearly from input parsing → process creation → result output.

---

## Capstone / Final note

This submission implements a production-minded lightweight shell in C with POSIX job control.  
Key achievements for the capstone evaluation:

- Robust command parsing and execution engine (fork/exec, execvp).
- POSIX-style piping and I/O redirection (`|`, `>`, `>>`, `<`) for multi-stage pipelines.
- Background execution (`&`), job table, `jobs` listing, and `fg` to bring jobs to foreground; automatic reaping with non-blocking `waitpid`.
- Cross-platform fallbacks for Windows (MinGW) while full features are validated on Linux/WSL.

Use `make` (or the GCC command shown) to build and `./bin/myshell` to test. The implementation has been verified to run background jobs and display job lifecycle notifications under WSL.

---

## **🙌 Acknowledgements**

Special thanks to professors, peers, and open-source contributors for guidance and resources.
This project was developed with a focus on clarity, structure, and cross-platform learning.

“A shell isn’t just a command interpreter — it’s the symphony of processes, orchestrated in C.”


---
