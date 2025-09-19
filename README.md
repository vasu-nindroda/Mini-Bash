# Mini Bash Shell (mbash25)

This project is a **mini command-line shell in C** (`mbash25`).  
It replicates core functionalities of the Unix Bash shell, supporting **command execution, I/O redirection, piping, conditional execution, and file operations** — all using **system calls** (`fork()`, `exec()`, `dup2()`, etc.) without relying on `system()`.

---

## ✨ Features

- **Command Execution**
  - Executes any valid system command or user program.
  - Supports execution via `./program` or from the `$PATH`.

- **Built-in Commands**
  - `cd` → Change directories (supports `cd ~` for home).
  - `killterm` → Exit the `mbash25` shell.

- **Special Operators**
  - `&` → Run background processes (up to 4 concurrently).  
  - `|` → Standard piping (up to 4 pipes / 5 commands).  
  - `~` → Reverse piping (data flows from rightmost command → leftmost).  
  - `+` → Append contents of two `.txt` files into each other.  
  - `#` → Count words in up to 4 `.txt` files.  
  - `++` → Concatenate multiple `.txt` files (up to 5) and print result to stdout.  
  - `<, >, >>` → Input redirection, output redirection, and append redirection.  
  - `;` → Sequential execution of multiple commands (up to 4).  
  - `&&`, `||` → Conditional execution of commands (up to 4 operators).  

- **Argument Rules**
  - Each command supports **1 ≤ argc ≤ 5**.
  - Works with or without spaces around operators (e.g., `ls|wc`, `ls | wc`).

---


