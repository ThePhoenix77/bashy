/*
 * Header file defining various structures and enums used throughout the 
 * bashy project. These structures facilitate command processing, 
 * environment handling, input/output redirection, and syntax parsing. 
 * Consistent use of types and clear struct definitions will help maintain 
 * a readable and maintainable codebase.
 */

#ifndef STRUCTS_H
# define STRUCTS_H

extern int					g_exit_status; // Global exit status for process tracking
typedef struct sigaction	t_sigaction; // Typedef for signal action structures

/*
 * Enumeration for different token types. This categorizes different 
 * elements in command parsing, such as words, redirects, and whitespace,
 * allowing the shell to differentiate between them during execution.
 */
typedef enum e_type
{
    WORD = 1,               // Normal word token
    REDIR_OUT = 2,         // Output redirection
    REDIR_IN = 3,          // Input redirection
    HERE_DOC = 4,          // Here-document
    DREDIR_OUT = 5,        // Double output redirection
    PIPE = 6,              // Pipe operator
    WHITE_SPACE = 7,       // Whitespace tokens
    DOUBLE_QUOTE = 8,      // Double-quoted strings
    ENV = 10,              // Environment variables
    NEW_LINE = 11,         // Newline token
    TABS = 12,             // Tab characters
    SINGEL_QOUTE = 13,     // Single-quoted strings
    EMPTY_ENV = 14,        // Empty environment variable case
    ENV_SPL = 15,          // Environment variable split
    EXP_HERE_DOC = 16,     // Expanded here-document
    HERE_DOC_FILE = 17,    // File associated with here-document
    ERROR_DIS = 18,        // Error display type
    SKIP = 19,             // Skip token type
}	t_type;

/*
 * Enumeration for environment variable visibility. This is used to control 
 * whether an environment variable should be shown or hidden in the shell context.
 */
typedef enum e_env_type
{
    SHOW, // Environment variable should be shown
    HIDE, // Environment variable should be hidden
}	t_env_type;

/*
 * Enumeration for different parsing states. This allows the parsing logic 
 * to maintain context regarding whether it is currently inside quotes or in 
 * general mode, which affects how tokens are handled.
 */
typedef enum e_state
{
    GENERAL,    // General state (no quotes)
    IN_DQUOTE,  // Inside double quotes
    IN_SQUOTE,  // Inside single quotes
}	t_state;

/*
 * Structure to represent a doubly linked list node for generic tokens.
 * Each node contains the content of the token, pointers to the next and 
 * previous nodes, its associated state, type, and length of the content.
 */
typedef struct s_lst
{
    char			*content;      // Content of the token
    struct s_lst	*next;         // Pointer to the next token
    struct s_lst	*prev;         // Pointer to the previous token
    t_state			state;        // State of parsing for this token
    t_type			type;         // Type of token
    int				len;          // Length of the token content
}	t_lst;

/*
 * Structure for a doubly linked list node specific to environment variables.
 * Each node contains the content of the environment variable and pointers
 * to the next and previous nodes, as well as its type for visibility control.
 */
typedef struct s_env_list
{
    char				*content;      // Content of the environment variable
    struct s_env_list	*next;         // Pointer to the next environment variable
    struct s_env_list	*prev;         // Pointer to the previous environment variable
    int					type;         // Type (show/hide) of the environment variable
}	t_env_list;

/*
 * Structure for a doubly linked list node for expanded variables.
 * This structure facilitates variable expansion handling within 
 * input processing, maintaining type information for the expanded content.
 */
typedef struct s_expand_list
{
    char					*content;   // Expanded content
    struct s_expand_list	*next;      // Pointer to the next expanded variable
    struct s_expand_list	*prev;      // Pointer to the previous expanded variable
    t_type					type;       // Type of the expanded variable
}	t_expand_list;

/*
 * Structure representing file descriptors for input and output management.
 * This enables unified handling within the shell when managing process I/O.
 */
typedef struct s_fd
{
    int	in;  // Input file descriptor
    int	out; // Output file descriptor
}	t_fd;

/*
 * Enumeration representing the types of files used in redirection.
 * Each type corresponds to different forms of file interactions in command execution.
 */
typedef enum e_file_type
{
    OUT = 2,   // Standard output redirection
    IN = 3,    // Standard input redirection
    DOUT = 5,  // Double output
    HRDC = 17, // Here-document file type
    ERR_DIS = 18, // Error display type
}	t_file_type;

/*
 * Structure for managing redirection nodes within a linked list.
 * This includes pointers for the next redirection node, the file name to which 
 * the redirection applies, and its associated type.
 */
typedef struct e_redir
{
    struct e_redir	*next;      // Pointer to the next redirection node
    char			*file_name; // Name of the file for redirection
    t_type			file_type;  // Type of file (e.g., output, input)
    t_type			type;       // Additional type for the redirection
}	t_redir;

/*
 * Structure for representing command argument nodes in a linked list.
 * This is used for storing command arguments to be executed in the shell.
 */
typedef struct s_cmd_args
{
    char				*content;      // Content of the command argument
    struct s_cmd_args	*next;         // Pointer to the next command argument
}	t_cmd_args;

/*
 * Enumeration for different types of nodes used in command execution trees.
 * This helps classify nodes as either commands, built-in commands, or pipelines.
 */
typedef enum e_type_node
{
    PIPE_LINE,  // Represents a pipeline
    CMD,        // Represents a command
    BUILTIN,    // Represents a built-in command
}	t_type_node;

/*
 * Structure for managing execution nodes in a linked list, which includes 
 * details about the command executions, file descriptors, redirections, and arguments.
 */
typedef struct s_exc_list
{
    t_fd				fd;           // File descriptors for the command execution
    t_type_node		type;         // Type of the node (command, builtin, or pipeline)
    struct s_exc_list	*next;        // Pointer to the next execution node
    t_redir				*redir;       // List of redirections associated with the command
    t_cmd_args			*cmd_args;    // List of command arguments to execute
}	t_exc_list;

/*
 * Structure for representing a command execution tree, allowing the shell 
 * to manage complex command chains with possibly nested commands.
 */
typedef struct s_tree
{
    t_fd			fds;           // File descriptors for this node
    t_type_node		type;         // Type of the node (command, builtin, etc.)
    struct s_tree	*left;        // Pointer to the left child (nested command)
    t_redir			*redir;       // List of redirections for this command
    struct s_tree	*right;       // Pointer to the right child (next command)
    char			**cmd_args;    // Array of command arguments for execution
}	t_tree;

/*
 * Structure representing the global state of the shell, encapsulating all 
 * necessary information for shell operation, including environment, command line input, 
 * linked lists of commands and variables, and status tracking for execution flow.
 */
typedef struct s_global
{
    char			*line_input;     // User input line
    char			*content;        // Current content for processing
    int				list_len;        // Length of the command list
    char			**env;           // Environment variables array
    char			**myenv;         // Shell's internal environment variables
    t_exc_list		*root;           // Root of the execution list
    t_lst			*head;           // Head of the linked list for tokens
    t_lst			*pre_head;       // Previous head for token list management
    t_lst			*tmp;            // Temporary node for operations
    t_env_list		*env_list;       // Head of the environment variable list
    t_expand_list	*exp_head;       // Head of the expanded variable list
    t_tree			*root_tree;      // Root of the command execution tree
    int				state;           // Current parsing state
    int				type;            // Current type for processing
    int				exit_status;     // Current exit status for the shell
    int				here_doc_num;    // Counter for here-documents
    int				exc_size;        // Size of the execution list
    t_fd			save_fd;         // Saved file descriptors for restoring
    int				pipe[2];         // Pipe file descriptors for process communication
    struct termios	t_termios;       // Terminal settings for TTY handling
}	t_global;

#endif
