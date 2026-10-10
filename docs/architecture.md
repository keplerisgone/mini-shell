# Architecture

This project is currently a small C program that demonstrates the basic process
control flow used by a shell.

## Program Flow

`main.c` starts in `main()` and immediately calls `fork()` to create a child
process.

- If `fork()` fails, the program prints the error with `perror()` and exits.
- In the child process, the program builds an argument list for `ls -l` and
  replaces the child process image with `execvp()`.
- If `execvp()` fails, the child prints the error and exits.
- In the parent process, the program prints a message and waits for the child to
  finish with `waitpid()`.

## Key System Calls

- `fork()` creates a separate child process.
- `execvp()` runs an external command in the child process.
- `waitpid()` keeps the parent process alive until the child process exits.

This structure is the core pattern for a minimal shell: create a child process,
execute a command in that child, and wait for the command to complete in the
parent process.
