# Bashy

**Bashy** is a fully featured Unix shell written in C, modelled after GNU Bash. It implements a complete pipeline from interactive input reading through lexing, parsing, variable expansion, redirection and process execution, including a built-in suite of seven commands and robust signal handling.

https://github.com/user-attachments/assets/67a65baa-0950-4422-9b7e-628d8f06f7ad

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Architecture Overview](#architecture-overview)
- [Data Structures](#data-structures)
  - [Global State — `t_global`](#global-state--t_global)
  - [Token List — `t_lst`](#token-list--t_lst)
  - [Environment List — `t_env_list`](#environment-list--t_env_list)
  - [Execution List — `t_exc_list`](#execution-list--t_exc_list)
  - [Execution Tree — `t_tree`](#execution-tree--t_tree)
  - [Redirection List — `t_redir`](#redirection-list--t_redir)
  - [Expand List — `t_expand_list`](#expand-list--t_expand_list)
- [Execution Pipeline](#execution-pipeline)
  - [1 — Input Reading](#1--input-reading)
  - [2 — Lexing / Tokenisation](#2--lexing--tokenisation)
  - [3 — Syntax Checking](#3--syntax-checking)
  - [4 — Here-document Processing](#4--here-document-processing)
  - [5 — Variable Expansion](#5--variable-expansion)
  - [6 — Redirection Preparation](#6--redirection-preparation)
  - [7 — Execution](#7--execution)
- [Built-in Commands](#built-in-commands)
  - [echo](#echo)
  - [cd](#cd)
  - [pwd](#pwd)
  - [env](#env)
  - [export](#export)
  - [unset](#unset)
  - [exit](#exit)
- [Redirections](#redirections)
- [Pipes](#pipes)
- [Signal Handling](#signal-handling)
- [Environment Management](#environment-management)
- [Shell Level (SHLVL)](#shell-level-shlvl)
- [Command History](#command-history)
- [Error Handling](#error-handling)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

---

## Features

| Feature | Description |
|---|---|
| Interactive prompt | `bashy$` with GNU Readline support |
| Command history | Persistent history saved to `~/.bash_history` |
| External commands | Searches `PATH` to execute any binary |
| Built-in commands | `echo`, `cd`, `pwd`, `env`, `export`, `unset`, `exit` |
| Pipes | Unlimited pipeline depth via a binary execution tree |
| Input redirection | `<` redirects stdin from a file |
| Output redirection | `>` truncates and writes stdout to a file |
| Append redirection | `>>` appends stdout to a file |
| Here-documents | `<<` reads multi-line input until a delimiter is reached |
| Variable expansion | `$VAR` and `$?` (last exit status) inside unquoted text and double-quoted strings |
| Single quoting | Suppresses all interpretation inside `'…'` |
| Double quoting | Suppresses word-splitting while still expanding `$VAR` |
| Signal handling | `Ctrl+C` (SIGINT) reprints the prompt; `Ctrl+\` (SIGQUIT) is ignored at the prompt |
| SHLVL tracking | Increments / clamps the `SHLVL` environment variable on each shell spawn |
| TTY save/restore | Terminal settings are saved on start and restored on exit |

---

## Project Structure

```
bashy/
├── main.c                  # Entry point, main loop, start() dispatcher
├── Makefile
├── includes/
│   ├── bashy.h         # All function prototypes, macros, and constants
│   └── structs.h           # All typedef'd structs and enums
├── builtins/
│   ├── ft_echo.c           # echo built-in
│   ├── ft_cd.c             # cd built-in
│   ├── ft_pwd.c            # pwd built-in
│   ├── ft_env.c            # env built-in
│   ├── ft_export.c         # export built-in
│   ├── ft_unset.c          # unset built-in
│   ├── ft_exit.c           # exit built-in
│   └── builtins_utils*.c   # Shared helpers for built-ins
├── env/
│   ├── env_utils*.c        # Environment list management
│   └── shell_level.c       # SHLVL initialisation and clamping
├── excution/
│   ├── main_exc.c          # Execution entry point
│   ├── build_tree.c        # Binary tree construction for pipelines
│   ├── execute_tree.c      # Recursive tree executor (fork + pipe)
│   ├── execute_left.c      # Left-side child of a pipe
│   ├── execute_right.c     # Right-side child of a pipe
│   ├── execute_cmd.c       # execve wrapper
│   ├── execute_one_cmd.c   # Single-command executor (builtin or external)
│   ├── path_finder.c       # PATH lookup for external commands
│   ├── redir_handle.c      # Apply redirections (open/dup2)
│   ├── redir_list.c        # Build redirection linked list
│   ├── cmd_list.c          # Build command-argument linked list
│   ├── prepare_exc_list.c  # Flatten token list into execution list
│   ├── exit_status.c       # Decode waitpid status
│   ├── syscall_fun.c       # Safe dup2 / close wrappers
│   └── ...
├── expanding/
│   ├── ft_expand.c         # Main expansion dispatcher
│   ├── check_node_expn.c   # Node-level expansion checks
│   ├── exp_utils*.c        # Expansion helpers
│   ├── last_step_exp.c     # Final expansion step
│   └── list_exp_fun.c      # Expand list management
├── utils/
│   ├── ft_tokeniz.c        # Lexer / tokeniser
│   ├── Utils_Tokeniz1.c    # Quote and operator handling
│   ├── Utils_Tokeniz2.c    # Word extraction helpers
│   ├── check_syntax.c      # Syntax validation
│   ├── check_redir.c       # Redirection token post-processing
│   ├── ft_heredoc.c        # Here-document reader
│   ├── here_doc_utils.c    # Here-document helpers
│   ├── signals.c           # Signal handlers and sigaction setup
│   ├── tty_handler.c       # Terminal state save/restore
│   ├── linked_list_fun.c   # Generic token list helpers
│   ├── clean_memory.c      # Memory cleanup
│   └── utils.c             # Miscellaneous helpers
└── libft/                  # Custom C utility library
```

---

## Architecture Overview

```
User Input (readline)
        │
        ▼
   check_valid_in()          ← reject empty / EOF
        │
        ▼
   ft_tokeniz()              ← Lexer: produces t_lst doubly-linked list
        │
        ▼
   check_syntax()            ← Reject unclosed quotes, bad operators
        │
        ▼
   check_num_heredoc()       ← Enforce here-doc count limit
        │
        ▼
   check_heredoc()           ← Read here-doc content interactively
        │
        ▼
   check_expand()            ← Pre-expansion validation
        │
        ▼
   main_exp_fun()            ← Variable expansion ($VAR, $?)
        │
        ▼
   check_redir()             ← Attach redirections to token nodes
        │
        ▼
   open_heredoc_file()       ← Write here-doc content to temp file
        │
        ▼
   main_exc()
     ├─ prepere_list()       ← Normalise token list
     ├─ bulid_list_exc()     ← Build t_exc_list from tokens
     ├─ check_builtins_cmds()← Tag BUILTIN vs CMD nodes
     ├─ env_list_2d_array()  ← Convert env list to char **
     └─ execute_one_cmd()    ← Single command
        or
        build_tree() ──►  execute_tree()   ← Pipeline via fork()+pipe()
```

---

## Data Structures

### Global State — `t_global`

The single `t_global` struct (declared on the stack in `main`) threads through every subsystem as a first-class parameter. It holds:

| Field | Type | Purpose |
|---|---|---|
| `line_input` | `char *` | Raw line from `readline` |
| `content` | `char *` | Scratch buffer used during lexing / expansion |
| `list_len` | `int` | Length of the current token list |
| `env` | `char **` | Original `envp` passed to `main` |
| `myenv` | `char **` | Flattened `char **` rebuilt before each `execve` |
| `head` | `t_lst *` | Head of the lexer token list |
| `pre_head` | `t_lst *` | Head of the normalised token list |
| `env_list` | `t_env_list *` | Head of the environment variable linked list |
| `exp_head` | `t_expand_list *` | Head of the expansion scratch list |
| `root` | `t_exc_list *` | Head of the execution command list |
| `root_tree` | `t_tree *` | Root of the pipeline binary tree |
| `state` | `int` | Current lexer state (GENERAL / IN_DQUOTE / IN_SQUOTE) |
| `type` | `int` | Current token type being processed |
| `exit_status` | `int` | Most recent command exit status |
| `here_doc_num` | `int` | Count of here-documents in the current command |
| `exc_size` | `int` | Number of commands in the execution list |
| `save_fd` | `t_fd` | stdin/stdout saved before redirections |
| `pipe[2]` | `int[2]` | Pipe file descriptors for the current pipeline node |
| `t_termios` | `struct termios` | Saved terminal settings |

---

### Token List — `t_lst`

Each element produced by the lexer is a `t_lst` node in a **doubly-linked list**:

```c
typedef struct s_lst {
    char     *content;  // Token string
    t_lst    *next;
    t_lst    *prev;
    t_state   state;    // GENERAL | IN_DQUOTE | IN_SQUOTE
    t_type    type;     // See token types below
    int       len;
} t_lst;
```

**Token types (`t_type` enum)**

| Value | Name | Meaning |
|---|---|---|
| 1 | `WORD` | Plain word / identifier |
| 2 | `REDIR_OUT` | `>` output redirection |
| 3 | `REDIR_IN` | `<` input redirection |
| 4 | `HERE_DOC` | `<<` here-document operator |
| 5 | `DREDIR_OUT` | `>>` append redirection |
| 6 | `PIPE` | `\|` pipe operator |
| 7 | `WHITE_SPACE` | Space / tab separator |
| 8 | `DOUBLE_QUOTE` | Content inside `"…"` |
| 10 | `ENV` | `$VAR` to be expanded |
| 11 | `NEW_LINE` | Newline character |
| 12 | `TABS` | Tab character |
| 13 | `SINGEL_QOUTE` | Content inside `'…'` |
| 14 | `EMPTY_ENV` | `$` with no valid variable name |
| 15 | `ENV_SPL` | Expanded env that requires word-splitting |
| 16 | `EXP_HERE_DOC` | Here-doc content after expansion |
| 17 | `HERE_DOC_FILE` | Temp file path for a here-doc |
| 18 | `ERROR_DIS` | Ambiguous redirection error marker |
| 19 | `SKIP` | Node already consumed, skip during processing |

---

### Environment List — `t_env_list`

Environment variables are stored as a **doubly-linked list** of `KEY=value` strings:

```c
typedef struct s_env_list {
    char         *content;  // "KEY=value"
    t_env_list   *next;
    t_env_list   *prev;
    int           type;     // SHOW or HIDE (controls export output)
} t_env_list;
```

Variables marked `HIDE` are present in the environment but suppressed when `export` is called without arguments.

---

### Execution List — `t_exc_list`

After token normalisation, the commands are represented as a **singly-linked list** of `t_exc_list` nodes — one per command segment (between pipes):

```c
typedef struct s_exc_list {
    t_fd         fd;        // Input/output file descriptors
    t_type_node  type;      // CMD | BUILTIN | PIPE_LINE
    t_exc_list  *next;
    t_redir     *redir;     // Redirection list for this command
    t_cmd_args  *cmd_args;  // Argument list (argv[0], argv[1], …)
} t_exc_list;
```

---

### Execution Tree — `t_tree`

For pipelines with two or more commands, the execution list is converted into a **left-leaning binary tree** where every internal node is `PIPE_LINE` and every leaf is either `CMD` or `BUILTIN`:

```
   cmd1 | cmd2 | cmd3
   
      PIPE_LINE
      /        \
   cmd1      PIPE_LINE
             /        \
           cmd2       cmd3
```

```c
typedef struct s_tree {
    t_fd         fds;
    t_type_node  type;
    t_tree      *left;
    t_tree      *right;
    t_redir     *redir;
    char       **cmd_args;  // NULL-terminated argv array
} t_tree;
```

`execute_tree` recurses the tree. At each `PIPE_LINE` node it calls `pipe()`, `fork()` twice, runs the left child in one process and the right subtree in another, then `waitpid`s for both.

---

### Redirection List — `t_redir`

```c
typedef struct e_redir {
    t_redir  *next;
    char     *file_name;   // Target file path (or here-doc delimiter)
    t_type    file_type;   // OUT | IN | DOUT | HRDC | ERR_DIS
    t_type    type;        // REDIR_OUT | REDIR_IN | DREDIR_OUT
} t_redir;
```

---

### Expand List — `t_expand_list`

A temporary scratch list used while expanding a single `$VAR` token into possibly multiple words:

```c
typedef struct s_expand_list {
    char            *content;
    t_expand_list   *next;
    t_expand_list   *prev;
    t_type           type;
} t_expand_list;
```

---

## Execution Pipeline

### 1 — Input Reading

`get_line()` uses GNU `readline` to display `bashy$` and read a line. History is loaded from `~/.bash_history` at startup and written back on exit. Empty and whitespace-only lines are silently skipped by `check_valid_in`. A `NULL` return from `readline` (EOF / `Ctrl+D`) triggers a clean exit.

### 2 — Lexing / Tokenisation

`ft_tokeniz()` walks the raw input character by character, splitting it into a `t_lst` doubly-linked list. Meta-characters (`space`, `\t`, `\n`, `'`, `"`, `>`, `<`, `|`) delimit tokens. The lexer tracks three states:

- `GENERAL` — outside any quotes
- `IN_DQUOTE` — inside `"…"`: `$` is still expanded, word-splitting is suppressed
- `IN_SQUOTE` — inside `'…'`: everything is literal

Each operator (`|`, `>`, `>>`, `<`, `<<`) is identified by `check_operator()` / `check_heredoc_dred()` and given the appropriate `t_type`.

### 3 — Syntax Checking

`check_syntax()` walks the token list and rejects:
- Unclosed single or double quotes → `bashy: syntax error: unexpected end of file.`
- Consecutive or dangling operators (e.g. `| |`, `> |`, trailing `|`) → `bashy: syntax error near unexpected token '…'`

If more here-documents appear in a single command than the system limit allows, `check_num_heredoc()` prints an error and exits with status `2`.

### 4 — Here-document Processing

`check_heredoc()` iterates the token list looking for `HERE_DOC` nodes. For each one:

1. The delimiter word is extracted from the following `WORD` nodes.
2. `open_heredoc()` calls `readline("> ")` in a loop, buffering lines into `global->content` until the delimiter is matched or EOF is reached.
3. The accumulated string is stored back on the `EXP_HERE_DOC` node.
4. Later, `open_heredoc_file()` writes the content to a temporary file whose path replaces the node content; the file is unlinked after it is opened for reading.

### 5 — Variable Expansion

`main_exp_fun()` iterates the token list and processes every `ENV` node (and unquoted `EXP_HERE_DOC`):

1. `split_exp()` — splits the `$VAR` or `$?` expression.
2. `special_expand()` — handles `$?` (last exit status).
3. `update_var_name()` — resolves the variable name.
4. `transform_env()` — looks up the name in the environment list.
5. `last_step_exp()` — splices the result back into the token list, performing word-splitting if the expansion occurred outside quotes.
6. `finish_exp()` — advances the iterator past the replaced nodes.

Variables inside `'…'` are **never** expanded. Variables inside `"…"` are expanded but not word-split.

### 6 — Redirection Preparation

`check_redir()` post-processes the token list:
- Attaches file names following `REDIR_OUT`, `REDIR_IN`, `DREDIR_OUT`, and `HERE_DOC` operators to `t_redir` nodes.
- Detects ambiguous redirections (expansion produced zero or multiple words) and marks them `ERROR_DIS`.

### 7 — Execution

`main_exc()` is the execution entry point:

1. `prepere_list()` — merges adjacent `WORD`/`ENV` tokens and splits `ENV_SPL` tokens.
2. `bulid_list_exc()` — converts the normalised token list into a `t_exc_list`, grouping tokens between `PIPE` nodes into individual command nodes.
3. `check_builtins_cmds()` — tags each node as `BUILTIN` or `CMD`.
4. `env_list_2d_array()` — rebuilds `global->myenv` as a `char **` for `execve`.
5. For a **single command**: `execute_one_cmd()` handles builtins in-process and externals via `fork()` + `execve()`.
6. For **pipelines**: `build_tree()` constructs the binary tree, then `execute_tree()` recursively executes it using `pipe()` + two `fork()` calls per node.

Redirections are applied with `handle_redirection()` immediately before `execve`/the builtin call, using `dup2` to wire file descriptors.

---

## Built-in Commands

Built-ins execute **in the current process** (no fork), so they can modify the shell's own state (working directory, environment, exit status).

### echo

```
echo [-n] [string ...]
```

Prints arguments separated by spaces. One or more consecutive `-n` flags at the start suppress the trailing newline.

```bash
echo Hello World          # Hello World\n
echo -n Hello World       # Hello World  (no newline)
echo -nnn Hello           # Hello        (no newline, -nnn treated as -n)
```

### cd

```
cd [path | - ]
```

Changes the current working directory and keeps `PWD` / `OLDPWD` in sync.

| Invocation | Behaviour |
|---|---|
| `cd` | Changes to `$HOME` |
| `cd /some/path` | Changes to the given absolute or relative path |
| `cd -` | Changes to `$OLDPWD` |

Errors (missing `HOME`, inaccessible path) are reported to stderr and set exit status `1`.

### pwd

```
pwd
```

Prints the absolute path of the current working directory (via `getcwd`). Sets exit status `1` on failure.

### env

```
env
```

Prints all environment variables that contain `=` (i.e. variables with values), one per line, in their current order.

### export

```
export [NAME[=VALUE] ...]
export [NAME+=VALUE ...]
```

Without arguments, prints all exported variables sorted alphabetically in `declare -x NAME="VALUE"` format.

With arguments:
- `NAME=VALUE` — creates or overwrites the variable.
- `NAME` (no `=`) — declares the variable without assigning a value (marks it for export).
- `NAME+=VALUE` — appends `VALUE` to the current value of `NAME`.
- Invalid identifiers (not starting with a letter or `_`, or containing illegal characters) print an error and set exit status `1`.

```bash
export PATH="/usr/bin:/bin"
export GREETING="Hello"
export GREETING+=" World"    # GREETING is now "Hello World"
export INVALID-NAME          # error: not a valid identifier
```

### unset

```
unset NAME [NAME ...]
```

Removes each named variable from the environment list. Invalid identifiers are reported to stderr. The variable is silently ignored if it does not exist.

```bash
unset MY_VAR TMP_VAR
```

### exit

```
exit [N]
```

Exits the shell with status `N` (0–255). `N` is normalised modulo 256 if out of range. If `N` is not a valid integer, the shell exits with status `255`. Passing more than one argument prints an error and keeps the shell open.

```bash
exit       # exit with last status
exit 0     # success
exit 42    # custom status
exit abc   # error: numeric argument required → exits 255
```

---

## Redirections

| Operator | Behaviour |
|---|---|
| `cmd < file` | Redirect stdin from `file` |
| `cmd > file` | Redirect stdout to `file` (truncate) |
| `cmd >> file` | Redirect stdout to `file` (append) |
| `cmd << DELIM` | Feed lines typed interactively as stdin until `DELIM` |

Multiple redirections on a single command are processed left to right; the last one for each stream wins. An ambiguous redirection (variable expands to zero or multiple words) prints an error and aborts execution.

Here-documents expand `$VAR` unless the delimiter is quoted:

```bash
cat << EOF
Hello $USER        # $USER is expanded
EOF

cat << 'EOF'
Hello $USER        # literal, not expanded
EOF
```

---

## Pipes

Commands separated by `|` are connected into a pipeline. The shell builds a **left-leaning binary tree** from the execution list and executes each `PIPE_LINE` node by:

1. Creating a `pipe(2)` pair.
2. Forking a **left child** that writes its stdout into the write end.
3. Forking a **right child** (or recursing into a subtree) that reads its stdin from the read end.
4. Waiting for both children before continuing.

```bash
ls -la | grep ".c" | wc -l
```

---

## Signal Handling

Signal handling is configured with `sigaction` at startup (`init_sigaction`):

| Signal | At the prompt | During execution |
|---|---|---|
| `SIGINT` (`Ctrl+C`) | Prints a new line, clears the readline buffer, redisplays the prompt, sets exit status to `1` | Delivered to the foreground process group; parent re-displays prompt |
| `SIGQUIT` (`Ctrl+\`) | Ignored (`SIG_IGN`) | Ignored |

Child processes restore default signal handling (`sig_dfl()`) before calling `execve`, and the parent ignores signals (`sig_ign()`) while waiting for children.

GNU Readline's own signal catching is disabled (`rl_catch_signals = 0`) so the shell retains full control.

---

## Environment Management

The environment is stored internally as a `t_env_list` doubly-linked list populated from the `envp` argument of `main`. The list is the single source of truth throughout the shell's lifetime. Key operations:

| Function | Description |
|---|---|
| `get_var_env()` | Finds a node whose content starts with the given prefix (e.g. `"PATH="`) |
| `find_var()` | Finds a node by exact key name |
| `update_env()` | Replaces the value part of an existing node |
| `add_or_update_env_var()` | Inserts a new node or updates an existing one |
| `free_env_list()` | Frees the entire list |

Before each `execve`, `env_list_2d_array()` flattens the list into a `char **` stored in `global->myenv` which is passed as the third argument to `execve`.

---

## Shell Level (SHLVL)

On startup, `shell_level()` locates `SHLVL` in the environment and increments it:

- If `SHLVL` does not exist, it is created as `SHLVL=1`.
- If `SHLVL` is `999`, it is reset to an empty string.
- If `SHLVL` exceeds `1000`, a warning is printed and the value is reset to `1`.
- Negative values are clamped to `0` before incrementing.

---

## Command History

- History is loaded from `~/.bash_history` at startup with `read_history()`.
- Every successfully executed command line is added to the in-memory history with `add_history()`.
- The full history is written back to disk with `write_history()` when the shell exits.
- History is cleared from memory with `clear_history()` before the process terminates.

---

## Error Handling

| Scenario | Behaviour |
|---|---|
| Unclosed quote | `bashy: syntax error: unexpected end of file.` → exit status 2 |
| Bad operator (e.g. `\|\|`, trailing `\|`) | `bashy: syntax error near unexpected token '…'` |
| Too many here-documents | `bashy$: maximum here-document count exceeded` → exits shell |
| Command not found | `bashy: <cmd>: command not found` → exit status 127 |
| Ambiguous redirection | Error printed to stderr → exit status 1 |
| `cd` with no `HOME` | `cd: HOME not set` → exit status 1 |
| `cd -` with no `OLDPWD` | `cd: OLDPWD not set` → exit status 1 |
| `exit` with non-numeric argument | `exit: <arg>: numeric argument required` → exits with status 255 |
| `exit` with too many arguments | `exit: too many arguments` → exit status 1, shell stays open |
| `malloc` failure | Prints error, frees state, exits |
| `fork` / `pipe` failure | `perror` to stderr |

All error messages are written to `STDERR_FILENO` (fd 2).

---

## Prerequisites

- A C compiler (`cc` / `gcc` / `clang`)
- GNU **Readline** library

Install Readline with Homebrew:

```bash
brew install readline
```

Then verify the library and include paths in the `Makefile` match your installation:

```make
READLINE_L = ~/.brew/opt/readline/lib
READLINE_I = ~/.brew/opt/readline/include
```

---

## Building

```bash
# Clone the repository
git clone https://github.com/ThePhoenix77/bashy.git
cd bashy

# Build (also builds the bundled libft)
make

# Remove object files
make clean

# Remove object files and the executable
make fclean

# Rebuild from scratch
make re
```

The resulting executable is named **`bashy`**.

---

## Usage

```bash
./bashy
```

You will see the prompt:

```
bashy$
```

Type any command and press **Enter**. Use **↑** / **↓** to navigate command history. Press **Ctrl+D** or type `exit` to leave the shell.

```bash
bashy$ echo "Hello, World!"
Hello, World!

bashy$ export GREETING="Hi"
bashy$ echo $GREETING
Hi

bashy$ ls -la | grep ".c" | wc -l
      3

bashy$ cat << EOF
> line one
> line two
> EOF
line one
line two

bashy$ cd /tmp && pwd
/tmp

bashy$ exit 0
```

---

## Contributing

Contributions are welcome. Please fork the repository, create a feature branch, and open a pull request with a clear description of your changes.

---

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
