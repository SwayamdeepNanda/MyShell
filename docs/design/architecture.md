Architecture (phase 1)

This document captures the high-level architecture and reasoning for MyShell Phase 1.

- Modular layers: lexer, parser, executor, builtin, plugins, signal management.
- Safety-first: centralized allocation helpers to ease auditing.
- Testability: unit test stubs and clear separation of concerns.
