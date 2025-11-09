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
MyShell/
│
├── src/ # Core logic (main.c)
├── include/ # Header files
├── docs/ # Reports and documentation
│ ├── design/
│ │ └── architecture.md
│ └── report.md
├── scripts/ # Helper scripts (build, valgrind, format)
├── tests/ # Future unit & integration tests
├── Makefile # Build automation
├── CMakeLists.txt # Cross-platform build configuration
└── README.md # Overview and guide


---

## 🧪 Example Usage

```bash
echo hello
pwd
cd ..
ls
echo "hi there" > test.txt && cat test.txt
ls | grep src


