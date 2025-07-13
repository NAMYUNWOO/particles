# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Awareness & Context
- **Always read `PLANNING.md`** at the start of a new conversation to understand the project's architecture, goals, style, and constraints.
- **Check `TASK.md`** before starting a new task. If the task isn’t listed, add it with a brief description and today's date.
- **Use consistent naming conventions, file structure, and architecture patterns** as described in `PLANNING.md`.


### Code Structure & Modularity
- **Never create a file longer than 500 lines of code.** If a file approaches this limit, refactor by splitting it into modules or helper files.
- **Organize code into clearly separated modules**, grouped by feature or responsibility.
- **Use clear, consistent imports** (prefer relative imports within packages).
- **memory safe programming**, avoid dynamic memory allocation, try to use Memory Pool.

### Testing & Reliability
- **Always create unit tests for new features** use `src/minunit/minunit.h`. example is in  `examples/minunit/` (functions, etc).
- **After updating any logic**, check whether existing unit tests need to be updated. If so, do it.

### Task Completion
- **Mark completed tasks in `TASK.md`** immediately after finishing them.
- Add new sub-tasks or TODOs discovered during development to `TASK.md` under a “Discovered During Work” section.

### Style & Conventions
- **Use C** as the primary language.
- **Raylib Conventions** `RAYLIB_CONVENTIONS.md`.
- Write **docstrings for every function** using the Google style:

### 📚 Documentation & Explainability
- **Update `README.md`** when new features are added, dependencies change, or setup steps are modified.
- **Update `PLANNING.md`** when new logics are added, Architecture change. 
- **Comment non-obvious code** and ensure everything is understandable to a mid-level developer.
- When writing complex logic, **add an inline `// Reason:` comment** explaining the why, not just the what.

### 🧠 AI Behavior Rules
- **Never assume missing context. Ask questions if uncertain.**
- **Never hallucinate libraries or functions** – only use known, verified C packages and Raylib Library.
- **Always confirm file paths and module names** exist before referencing them in code or tests.
- **Never delete or overwrite existing code** unless explicitly instructed to or if part of a task from `TASK.md`.

## Platform-Specific Notes
- **macOS**: Raylib installed via Homebrew at `/opt/homebrew/Cellar/raylib/5.5`
- **Windows**: Uses W64Devkit with Raylib at `C:/Users/namyunwoo/W64Devkit/w64devkit`

