# 🐚 Minishell

A lightweight, POSIX-compliant shell implementation written in C that replicates core functionality of bash. This project demonstrates advanced Unix system programming concepts including process management, signal handling, file descriptors, and memory management with **zero memory leaks** (including still reachable blocks).

![C](https://img.shields.io/badge/language-C-blue.svg)
![Memory Safe](https://img.shields.io/badge/memory-leak%20free-brightgreen.svg)
![Norminette](https://img.shields.io/badge/norminette-passing-success.svg)

---

## 📋 Table of Contents

- [Features](#-features)
- [Architecture](#-architecture)
- [Installation](#-installation)
- [Usage](#-usage)
- [Built-in Commands](#️-built-in-commands)
- [Advanced Features](#-advanced-features)
- [Memory Management](#-memory-management)
- [Testing](#-testing)
- [Project Structure](#-project-structure)
- [Technical Implementation](#-technical-implementation)
- [Known Limitations](#-known-limitations)
- [Contributing](#-contributing)

---

## ✨ Features

### Core Shell Functionality
- **Interactive command prompt** with readline integration
- **Command execution** via PATH resolution or absolute/relative paths
- **Pipeline support** - Multiple commands connected with pipes (`|`)
- **Input/Output redirection** - `<`, `>`, `>>`
- **Here-documents** (`<<`) with variable expansion control
- **Quote handling** - Single (`'`) and double (`"`) quotes with proper semantics
- **Signal handling** - Proper SIGINT (Ctrl-C) and SIGQUIT (Ctrl-\) behavior
- **Environment variable expansion** - Including `$?` for exit status
- **Command history** - Navigate through previous commands

### Built-in Commands
- `echo` with `-n` flag support
- `cd` with relative/absolute paths, `~`, `-`, and PWD management
- `pwd` - Print working directory
- `export` - Set environment variables with sorted display
- `unset` - Remove environment variables
- `env` - Display environment variables
- `exit` - Exit shell with numeric argument support

### Quality Assurance
✅ **Zero memory leaks** - Verified with Valgrind (including still reachable)  
✅ **No file descriptor leaks** - All FDs properly closed  
✅ **Comprehensive error handling** - Graceful failure modes  
✅ **Signal-safe operations** - Proper signal handling during execution  

---

## 🏗 Architecture

```
Input Line
    ↓
┌─────────────┐
│   Lexer     │  → Tokenizes input into words, operators, quotes
└─────────────┘
    ↓
┌─────────────┐
│  Expander   │  → Handles variable expansion and quote removal
└─────────────┘
    ↓
┌─────────────┐
│   Parser    │  → Builds command structures with redirections
└─────────────┘
    ↓
┌─────────────┐
│  Executor   │  → Executes commands (builtin or external)
└─────────────┘
```

### Module Breakdown

| Module | Responsibility | Key Files |
|--------|----------------|-----------|
| **Lexer** | Tokenization, quote detection | `lexer/lexer.c`, `lexer/lexer_special.c` |
| **Expander** | Variable expansion, quote processing | `expander/expander.c`, `expander/expander_variables.c` |
| **Parser** | AST construction, syntax validation | `parser/parser.c`, `parser/parser_valid.c` |
| **Executor** | Process management, I/O redirection | `executor/executor.c`, `executor/executor_pipe.c` |
| **Builtins** | Built-in command implementation | `builtin/*.c` |

---

## 🚀 Installation

### Prerequisites
- GCC compiler
- GNU Readline library (`libreadline-dev`)
- GNU Ncurses library (`libncurses-dev`)
- Make

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd minishell

# Compile the project
make

# Run the shell
./minishell
```

### Additional Make Targets

```bash
make clean   # Remove object files
make fclean  # Remove object files and executable
make re      # Rebuild from scratch
make leaks   # Run with Valgrind (interactive mode)
```

### Valgrind Testing

```bash
# Interactive mode
make leaks

# With input file
make leaks INPUT=test_input.txt
```

---

## 💻 Usage

### Basic Examples

```bash
# Simple command execution
minishell$ ls -la

# Pipelines
minishell$ cat file.txt | grep "pattern" | wc -l

# Input/Output redirection
minishell$ cat < input.txt > output.txt
minishell$ echo "append this" >> file.txt

# Here-documents
minishell$ cat << EOF
> Hello World
> This is a heredoc
> EOF

# Environment variables
minishell$ echo $HOME
minishell$ echo "Exit code: $?"

# Quote handling
minishell$ echo "Hello $USER"     # Variable expanded
minishell$ echo 'Hello $USER'     # Literal string
```

### Advanced Usage

```bash
# Complex pipelines with redirections
minishell$ < input.txt grep pattern | sort | uniq > output.txt

# Multiple redirections
minishell$ echo "test" > file1.txt > file2.txt  # Last one applies

# Here-doc with variable expansion control
minishell$ cat << EOF          # Variables expanded
minishell$ cat << 'EOF'        # Variables NOT expanded

# Background jobs and process control
minishell$ sleep 10 & echo "Running in background"
```

---

## 🛠️ Built-in Commands

### `echo` - Display text
```bash
echo [-n] [arguments...]
```
- `-n`: Do not output trailing newline
- Supports multiple `-n` flags consecutively

### `cd` - Change directory
```bash
cd [directory]
```
- No arguments: Change to `$HOME`
- `-`: Change to previous directory (`$OLDPWD`)
- `~`: Change to home directory
- Updates `PWD` and `OLDPWD` environment variables

### `pwd` - Print working directory
```bash
pwd
```
Displays the current working directory absolute path.

### `export` - Set environment variables
```bash
export [name[=value]...]
```
- No arguments: Display all exported variables (sorted)
- With arguments: Set or update environment variables
- Validates variable names (alphanumeric + underscore, no leading digit)

### `unset` - Remove environment variables
```bash
unset [name...]
```
Removes the specified environment variables.

### `env` - Display environment
```bash
env
```
Prints all environment variables in `NAME=VALUE` format.

### `exit` - Exit shell
```bash
exit [n]
```
- No argument: Exit with last command's exit status
- Numeric argument: Exit with specified code (modulo 256)
- Non-numeric: Error message, exit with 2

---

## 🔥 Advanced Features

### Here-Documents (`<<`)

Here-documents allow multi-line input with intelligent variable expansion:

```bash
# With variable expansion
minishell$ cat << EOF
Current user: $USER
Home directory: $HOME
EOF

# Without variable expansion (quoted delimiter)
minishell$ cat << 'EOF'
Literal $USER and $HOME
EOF
```

**Implementation highlights:**
- Proper signal handling (Ctrl-C cancels heredoc)
- Delimiter matching with quote detection
- Variable expansion control based on delimiter quoting
- EOF warning for unclosed heredocs

### Pipeline Execution

Complex multi-command pipelines with proper file descriptor management:

```bash
minishell$ cmd1 | cmd2 | cmd3 | cmd4
```

**Features:**
- Parallel process creation for each command
- Proper pipe setup between consecutive commands
- First and last command special handling
- Proper cleanup on errors
- Exit status from last command in pipeline

### Variable Expansion

Sophisticated expansion system with context awareness:

```bash
minishell$ echo $HOME               # Environment variable
minishell$ echo $?                  # Exit status of last command
minishell$ echo "$USER is here"     # Expansion in double quotes
minishell$ echo '$USER is here'     # No expansion in single quotes
```

**Edge cases handled:**
- Empty variable expansion → empty string
- Special characters (`$?`, `$$`)
- Quoted contexts (single vs double quotes)
- Unquoted word expansion

### Signal Handling

Three distinct signal handling contexts:

1. **Interactive mode** (main shell)
   - SIGINT (Ctrl-C): Display new prompt
   - SIGQUIT (Ctrl-\\): Ignored

2. **Command execution**
   - Parent ignores signals during child execution
   - Child processes handle signals normally

3. **Here-document mode**
   - SIGINT cancels heredoc input
   - Returns to main prompt

---

## 🧠 Memory Management

### Zero Leak Guarantee

This implementation achieves **zero memory leaks** including:
- ✅ Definitely lost: 0 bytes
- ✅ Indirectly lost: 0 bytes
- ✅ Possibly lost: 0 bytes
- ✅ Still reachable: 0 bytes

### Memory Management Strategy

```c
// Every malloc has a corresponding free
char *str = ft_strdup(input);
if (!str)
    return handle_error();
// ... use str ...
free(str);  // Always freed on all code paths
```

**Key techniques:**
1. **Structured cleanup functions** - Centralized deallocation
2. **Error path consistency** - Free on all exit paths
3. **Parent/child separation** - Clear ownership semantics
4. **Readline history management** - `rl_clear_history()` before exit

### File Descriptor Management

All file descriptors properly tracked and closed:

```c
// Save original FDs
int save_stdin = dup(STDIN_FILENO);
int save_stdout = dup(STDOUT_FILENO);

// ... do work with redirections ...

// Always restore
dup2(save_stdin, STDIN_FILENO);
dup2(save_stdout, STDOUT_FILENO);
close(save_stdin);
close(save_stdout);
```

---

## 🧪 Testing

### Manual Testing

```bash
# Basic command execution
minishell$ /bin/ls
minishell$ echo test
minishell$ pwd

# Redirections
minishell$ echo "test" > file.txt
minishell$ cat < file.txt
minishell$ cat file.txt | grep test

# Error handling
minishell$ /nonexistent/path
minishell$ cat <missing_file.txt
minishell$ |  # Syntax error
```

### Valgrind Testing

```bash
# Full leak check with file descriptor tracking
make leaks

# With suppression file for readline
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --track-fds=yes \
         --suppressions=readline.supp \
         ./minishell
```

### Automated Test Suite

Create a test input file:
```bash
# test_input.txt
echo "Test 1: Basic echo"
pwd
cd /tmp
pwd
export TEST_VAR=hello
echo $TEST_VAR
unset TEST_VAR
echo $TEST_VAR
exit 0
```

Run tests:
```bash
make leaks INPUT=test_input.txt
```

---

## 📁 Project Structure

```
minishell/
├── Makefile                    # Build configuration
├── minishell.h                 # Main header file
├── main.c                      # Entry point and REPL
│
├── builtin/                    # Built-in command implementations
│   ├── cd.c                    # cd command
│   ├── cd_utils.c              # cd helpers
│   ├── echo.c                  # echo command
│   ├── env.c                   # env command
│   ├── exit.c                  # exit command
│   ├── export.c                # export command
│   ├── export_utils*.c         # export helpers
│   ├── pwd.c                   # pwd command
│   └── unset.c                 # unset command
│
├── executor/                   # Command execution engine
│   ├── executor.c              # Main execution logic
│   ├── executor2.c             # Builtin detection & execution
│   ├── executor3.c             # PATH resolution
│   ├── executor4.c             # External command execution
│   ├── executor_redir.c        # I/O redirection handling
│   ├── executor_pipe.c         # Pipeline management
│   ├── executor_heredoc*.c     # Here-document implementation
│   ├── pipe_child_*.c          # Child process management
│   └── pipe_pipeline_setup.c   # Pipe setup utilities
│
├── expander/                   # Variable expansion
│   ├── expander.c              # Main expansion logic
│   ├── expander_quote.c        # Quote removal
│   ├── expander_type.c         # String expansion
│   └── expander_variables.c    # Variable resolution
│
├── lexer/                      # Tokenization
│   ├── lexer.c                 # Main lexer logic
│   ├── lexer_special.c         # Special token handling
│   └── lexer_utils.c           # Lexer utilities
│
├── parser/                     # Syntax analysis
│   ├── parser.c                # Main parser
│   ├── parser_cmd.c            # Command structure creation
│   ├── parser_redir.c          # Redirection parsing
│   ├── parser_utils.c          # Parser utilities
│   ├── parser_valid.c          # Syntax validation
│   └── parser_free_utils.c     # Memory cleanup
│
├── src/                        # Core utilities
│   ├── env_data.c              # Environment management
│   ├── signals.c               # Signal handling
│   └── signals2.c              # Additional signal utilities
│
└── utils/                      # Helper functions
    ├── ft_split.c              # String splitting
    ├── numeric_atoll.c         # Numeric conversions
    ├── utils.c                 # String utilities
    ├── utils2.c                # Additional utilities
    └── utils3.c                # More utilities
```

---

## 🔧 Technical Implementation

### Token Types

```c
typedef enum e_token_type {
    T_WORD,          // Regular word
    T_WORD_SINGLE,   // Single-quoted string
    T_WORD_DOUBLE,   // Double-quoted string
    T_PIPE,          // |
    T_REDIR_IN,      // <
    T_REDIR_OUT,     // >
    T_APPEND,        // >>
    T_HEREDOC,       // <<
} t_token_type;
```

### Command Structure

```c
typedef struct s_command {
    char         **args;        // Command arguments
    t_redir      *redirs;       // Linked list of redirections
    t_heredoc    *heredocs;     // Linked list of heredocs
    int          redir_error;   // Redirection error flag
    struct s_command *next;     // Next command in pipeline
} t_command;
```

### Redirection Handling

```c
typedef struct s_redir {
    t_redir_type    type;       // R_IN, R_OUT, R_APPEND
    char           *file;       // Target filename
    struct s_redir *next;       // Next redirection
} t_redir;
```

### Here-Document Implementation

```c
typedef struct s_heredoc {
    char    *delimiter;         // End marker
    int     expand_vars;        // Expand variables?
    int     pipe_fd[2];        // Pipe for content
    struct s_heredoc *next;    // Next heredoc
} t_heredoc;
```

---

## ⚠️ Known Limitations

### By Design
1. **No wildcard expansion** (`*`, `?`) - Not required by subject
2. **No logical operators** (`&&`, `||`) - Beyond scope
3. **No subshells** - `()` not implemented
4. **No background jobs** - `&` not supported
5. **Limited history** - Basic readline integration only

### Edge Cases
1. **Empty pipes** - `| |` handled with syntax error
2. **Unclosed quotes** - Detected and reported
3. **Maximum path length** - Limited by `PATH_MAX` (4096 bytes)
4. **Environment size** - Dynamic allocation, system-limited

---

## 🤝 Contributing

This is an educational project, but contributions for bug fixes or improvements are welcome!

### Coding Standards
- Follows **42 Norminette** coding standard
- Maximum 25 lines per function
- Maximum 5 functions per file (with exceptions)
- No global variables (except for signal handling)

### Pull Request Process
1. Ensure code passes Norminette
2. Test with Valgrind for memory leaks
3. Add test cases for new features
4. Update documentation

---

## 📝 License

This project is part of the 42 School curriculum. All rights reserved.

---

## 👥 Authors

- **[@eauzun](https://github.com/eauzun)** - Core implementation and architecture
- **[@Pika221](https://github.com/Pika221)** - Development and testing

---

## 🙏 Acknowledgments

- 42 School for the project subject
- GNU Readline library developers
- The open-source community

---

## 📚 Additional Resources

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/)
- [POSIX Shell Specification](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [Advanced Linux Programming](https://www.advancedlinuxprogramming.com/)
- [Unix System Calls](https://man7.org/linux/man-pages/)

---

**Note**: This implementation prioritizes correctness, memory safety, and code clarity over feature completeness. It serves as an excellent learning resource for understanding Unix shell internals and system programming concepts.****
