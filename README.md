# 🧠 MyShell — A Lightweight, Portable Command Shell in C

**Author:** Swayamdeep Nanda  
**Course:** Linux System Programming / Capstone  
**Date:** 09-Nov-2025  
**Repository:** [https://github.com/SwayamdeepNanda/MyShell](https://github.com/SwayamdeepNanda/MyShell)

---

## 🌟 Overview

**MyShell** is a minimal yet production-minded command-line shell implemented in **C**.  
It demonstrates key **Operating System concepts** such as process creation, command parsing,  
I/O redirection, and inter-process communication through pipelines.

The goal is to create a **modular, cross-platform, and safe shell** —  
simple enough for learning, yet structured like a real-world system.

---

## 🎯 Objectives

- Implement a portable **command shell** using standard C.
- Demonstrate **process management**, **piping**, and **redirection**.
- Support essential **built-in commands** (`cd`, `pwd`, `exit`).
- Integrate **cross-platform compatibility** (Windows & Linux).
- Maintain clean formatting, build automation, and documentation for evaluation.

---

## 🧩 Project Structure

| Directory / File | Description |
|------------------|-------------|
| `src/` | Core source files — main shell engine, parsing, execution |
| `include/` | Header files and interfaces |
| `plugins/` | Future extension system (for modular commands) |
| `tests/` | Unit and integration tests |
| `docs/` | Report, design architecture, and documentation |
| `Makefile`, `CMakeLists.txt` | Build automation (GCC/CMake) |
| `.clang-format`, `.editorconfig`, `.gitignore` | Code quality and consistency |
| `scripts/` | Helper scripts for formatting, linting, and Valgrind testing |

---

## ⚙️ Key Features

- ✅ Built-ins: `cd`, `pwd`, `exit`  
- ✅ Command execution using `fork()` and `execvp()`  
- ✅ Input/output redirection (`>`, `<`, `>>`)  
- ✅ Pipeline support using `|` (Linux only)  
- ✅ Graceful handling of `SIGINT` (`Ctrl+C`)  
- ✅ Portable to Windows via MinGW (limited features)

---

## 🧱 Build and Run Instructions

### 🐧 On Linux / WSL (Recommended)

```bash
# build
make

# run
./bin/myshell

# example commands
echo hello | tr a-z A-Z
echo hi > test.txt && cat test.txt
pwd
ls
