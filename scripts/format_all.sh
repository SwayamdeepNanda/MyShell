#!/usr/bin/env bash
set -euo pipefail
echo "Running clang-format on src/ and include/"
clang-format -i src/*.c include/*.h || true
