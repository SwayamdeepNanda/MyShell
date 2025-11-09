#!/usr/bin/env bash
set -euo pipefail

BIN=./bin/myshell
if [ ! -x "$BIN" ]; then
  echo "Build the project first (make debug or make release)"
  exit 1
fi

mkdir -p valgrind-out
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=2 \
    --log-file=valgrind-out/valgrind.log "$BIN"
echo "Valgrind log: valgrind-out/valgrind.log"
