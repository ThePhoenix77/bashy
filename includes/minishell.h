/* 
 * Header file for the Minishell project, defining constants, function prototypes, 
 * and includes necessary for shell operations. Well-structured organization ensures 
 * maintainability and readability for both new and existing developers. Consider 
 * utilizing self-descriptive function names and adding more specific comments for 
 * clarity on complex functions.
 */

#ifndef MINISHELL_H
# define MINISHELL_H

/* Error handling constants to manage process flow clearly */
# define SUCCESS true
# define FAILURE false
# define HISTORY_FILE "~/.bash_history" // Path to history file for command recall
# define META_CHARS " \t\n\'\"><|"

/*~~~~~~~~~~~Syntax Error~~~~~~~~~~~~*/
# define UNCLOSED_QUOTE "bashy: syntax error: unexpected end of file.\n" // Error for unclosed quotes
# define ERROR "bashy: syntax error near unexpected token `" // General syntax error message
# define ERR_MANY_HERDOCS "bashy$: maximum here-document count \
exceeded\n" // Limit exceeded for here-documents

/*~~~~~~~~~~~~~Execution Errors~~~~~~~~~~*/
# define ERR_CMD_NF ": command not found\n" // CMD not found error

/*
 * Include necessary libraries, external headers, and additional modules,
 * ensuring dependency clarity. Consider encapsulating library-specific helpers 
 * in utility functions for modularity.
 */
# include "../libft/libft.h" // Custom library for utility functions
# include <stdbool.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
# include <sys/wait.h>
# include <errno.h>
# include <limits.h>
# include <fcntl.h>
# include <termios.h>
# include <readline/readline.h> // For command line input
# include <readline/history.h>
# include "structs.h" // Include structs used throughout the shell
# include <sys/stat.h>

/*~~~~~~~~~~~~~~~~~~~~~Signal Handling~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Function prototypes for signal handling. Proper signal management is essential 
 * for a responsive shell environment. Consider adding comments detailing signal 
 * behavior and integration with the shell's command execution flow.
 */
void			init_sigaction(void); // Initialize signal actions
void			signals_handler(int sig); // Handle signals received
void			exit_status_update(t_global *global); // Update shell exit status
void			sig_ign(void); // Ignore specific signals
void			sig_dfl(void); // Restore default signal handlers
void			sig_hnd(void); // Handle specific signals in shell

/*----------------Linked List Functions--------------*/
/*
 * Linked list management function prototypes. It’s crucial for dynamic memory 
 * management in the shell. Ensure memory is properly freed for all structures 
 * to prevent leaks.
 */
void			free_lst(t_lst **head); // Free linked list
t_lst			*last_node(t_lst *head); // Retrieve the last node
void			add_list(t_global *global); // Add a node to the list
t_lst			*new_node(t_global *global); // Create a new node 
bool			add_back(t_lst **head, t_lst **new); // Add new node to the back of the list

/*----------------Linked Environment List Functions--------------*/
/*
 * Function prototypes for managing environment variable linked lists.
 * This is critical for maintaining the state and configuration of the shell.
 */
t_env_list		*new_env_node(char *content); // Create a new env node
t_env_list		*last_env_node(t_env_list *head); // Get last environment node
void			free_env_list(t_env_list **head); // Free environment list
void			add_env_back(t_env_list **head, t_env_list *new); // Append new env node

/*------------------Environment Functions-----------------------*/
/*
 * Functions related to environment management. Making these operations 
 * efficient and safe is key to a robust shell implementation.
 */
bool			check_env(t_global *global); // Check environment variable validity
bool			empty_env(t_global *global); // Check if environment is empty
bool			shell_level(t_global *global); // Check shell nesting level
t_env_list		*get_var_env(t_global *global, char *var); // Get specific env variable
bool			update_env(t_env_list **env, char *new_val); // Update env variable value

/*----------------Utility Functions-----------------------*/
/*
 * Utility function prototypes. Streamlining common operations can drastically 
 * increase code clarity and reduce duplication.
 */
int				ft_strcmp(char *s1, char *s2); // String comparison
void			free_memory(t_global *global); // Frees allocated memory for the shell environment
int				check_valid_in(t_global *global); // Validate input
int				ft_strlen_un_char(char *str, char c); // Calculate string length without a character
int				ft_strlen_un_del(char *sep, char *str); // Calculate length ignoring separators
void			init_global(t_global *global, char **env); // Initialize global structure

/*~~~~~~~~~~~~~~Tokenization Functions~~~~~~~~~~~~~~~~*/
/*
 * Functions for parsing command input. Effective tokenization is crucial 
 * for accurately interpreting user commands and handling arguments.
 */
void			get_opr(t_global *global); // Get operator tokens
void			ft_tokeniz(t_global *global); // Tokenize input command
void			check_type(t_global *global); // Check token types
void			check_state(t_global *global); // Validate current state of parsing
bool			check_syntax(t_global *global); // Check syntax validity
void			check_here_doc(t_global *global); // Handle here-document syntax
bool			check_operator(t_global *global); // Validate operator tokens
bool			check_num_heredoc(t_global *global); // Check heredoc count
bool			check_heredoc_dred(t_global *global); // Validate heredoc redirection
void			skip_ec_t(t_global *global, int flag); // Skip escape characters
void			get_str_inside_quote(t_global *global); // Extract string within quotes
char			*get_word(char **line, int len, t_global *global); // Get word from line
void			update_line(char **line, int len_skip, t_global *global); // Update line to skip processed part

/*~~~~~~~~~~~~~~~~~~~~Expanding~~~~~~~~~~~~~~~~~~~~*/
/*
 * Expanding functions for handling variable expansion within commands. 
 * Proper management ensures user and environment variables are resolved effectively.
 */
t_expand_list	*new_exp_node(char *content); // Create a new expansion node
t_expand_list	*last_exp_node(t_expand_list *head); // Get the last expansion node
void			join_result(t_global *global); // Join expansion results
bool			check_expand(t_global *global); // Validate expansion
bool			main_exp_fun(t_global *globol); // Main expansion function
void			transform_env(t_global *global); // Transform environment variables
void			update_var_name(t_global *global); // Update variable names during expansion
void			free_exp_list(t_expand_list **head); // Free expansion list
bool			split_exp(t_global *global, char **line); // Split expanded line
bool			get_exp_var(t_global *global, char **line); // Get expanded variable
void			finish_exp(t_global *global, t_lst **node); // Complete variable expansion
void			last_step_exp(t_global *global, t_lst *node); // Finish last step of expansion
char			*ft_strjoin2(char const *s1, char const *s2); // Safe string join
bool			special_expand(t_global *global); // Handle special cases in expansion
bool			add_exp_back(t_expand_list **head, t_expand_list *new); // Add new expansion node at the back

/*~~~~~~~~~~~~~~~~~~~~~Here Document~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Functions for handling here-documents. Ensure robust processing for multi-line 
 * input directly into commands.
 */
bool			check_heredoc(t_global *global); // Validate here-doc input
void			open_heredoc_file(t_global *global); // Open file for here-doc
void			empty_here_doc(t_global *global, t_lst *node); // Handle empty here-doc entry

/*~~~~~~~~~~~~~~~~~~~~~Execution~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Core execution functions to manage command execution flow. Each function should 
 * validate inputs and ensure proper error handling to prevent runtime issues.
 */
void			main_exc(t_global *global); // Main execution handler
void			add_pre_list(t_global *global); // Prepare command list for execution
int				exc_list_size(t_exc_list *exc); // Get size of execution list
t_exc_list		*last_exc_node(t_exc_list *head); // Get last node in execution list
void			bulid_list_exc(t_global *global); // Build execution command list
void			free_exc_list(t_exc_list **head); // Free execution list
int				execute_one_cmd(t_global *global); // Run a single command
void			env_list_2d_array(t_global *global); // Convert environment list to 2D array
void			handle_redir_once(t_global *global); // Manage redirection for a single exec
int				join_cont(t_global *global, t_lst *tmp); // Join continuation commands
void			split_env(t_global *global, t_lst *tmp); // Split environment variables
void			create_node(t_global *global, t_lst **node); // Create node for command
void			add_back_exc(t_exc_list **head, t_exc_list *new); // Append to execution list
t_exc_list		*new_exc_spot(t_global *global, t_type_node type); // Create new execution node
bool			redir_one_node(t_global *global, t_exc_list **exc_node); // Handle redirection for a command node
bool			execute_one_builtin(t_global *global, char **cmd_args, t_exc_list *exc_node); // Execute built-in command

/*~~~~~~~~~~~~~~~~~~~~~~Builtins~~~~~~~~~~~~~~~~~~~~*/
/*
 * Built-in command functions. Ensure these are designed for maximum 
 * reliability and efficiency, as they are frequently called by users.
 */
int				ft_pwd(t_global *global); // Print current working directory
void			ft_env(t_global *global); // Display environment variables
void			ft_echo(t_global *global, char **args); // Print arguments to output
size_t			ft_strnlen(char *s, size_t maxlen); // Safe string length with limit
void			ft_export(t_global *global, char **args); // Export variables to environment
char			*ft_strcpy(char *dest, const char *src); // String copy
void			export_without_args(t_global *global); // Handle export with no arguments
char			*get_env_value(char *content); // Get value of an environment variable
void			update_pwd(t_global *global); // Update the PWD variable
void			change_to_oldpwd(t_global *global); // Change working directory to old
int				oldpwd_check(t_global *global); // Check old PWD validity
int				is_valid_name_char(char c); // Validate variable name characters
int				name_checker(char *s); // Check if name is valid
int				arg_size(char **args); // Get size of argument list
char			*update_the_env_var(t_global *global, char *name, char *new_val); // Update environment variable value
void			handle_key_value(t_global *global, char *content); // Handle key=value formatting
t_env_list		*find_var(t_env_list *env_list, char *key); // Find specific environment variable
int				check_builtins(t_global *global, char **cmd); // Check if command is a built-in
void			existing_var_case(t_global *global, char *key); // Handle existing var scenarios
void			handle_key_value(t_global *global, char *content); // Handle key-value in export
void			add_export_node(t_env_list **head, t_env_list *new); // Add node to export list
char			*ft_strndup(char *src, int n); // String duplicate with limit
char			*create_new_content(char *key, char *value); // Create formatted key=value string
void			ft_cd(t_global *global, char **args); // Change directory
void			ft_exit(t_global *global, char **args); // Exit shell
void			ft_unset(t_global *global, char **args); // Remove variable from environment
void			err_msg(char **args); // Print error message for command
void			update_env_var(t_env_list *env_var, char *new_content); // Update existing var in env list
void			add_or_update_env_var(t_global *global, char *key, char *value, int type); // Add or update environment variable

/*~~~~~~~~~~~~~~~~~~~~~~Command~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Command execution logging functions. These assist in debugging and 
 * follow user command history. Ensure messages are clear and informative.
 */
void			cmd_msg(char *cmd); // Log command to console or debug
void			ft_cmd_err(char *cmd); // Print command error message
void			free_cmd_list(t_cmd_args **head); // Free command argument list
char			*find_path(char **split, char *cmd); // Retrieve command path
char			*get_path(char *cmd, t_global *global); // Get full command path
void			check_builtins_cmds(t_exc_list *exc_node); // Verify built-in commands
void			store_cmd_args(t_global *global, t_lst **node); // Store command arguments
t_cmd_args		*new_cmd_element(t_global *global, char *content); // Create new command args element
void			add_back_element(t_cmd_args **head, t_cmd_args *new); // Append new command args
char			**get_args_arr(t_global *global, t_cmd_args *cmd_args, t_type_node type); // Get argument array
void			add_list_cmd(t_global *global, t_exc_list *exc, t_lst **node); // Add command to list

/*~~~~~~~~~~~~~~~~~~~~~~Redirections~~~~~~~~~~~~~~~~~*/
/*
 * Redirection handling functions. These ensure that output and 
 * input streams are properly managed during execution, enhancing 
 * flexibility in command handling.
 */
void			check_redir(t_global *global); // Handle redirection checking
void			free_redir_list(t_redir **head); // Free redirection list
void			handle_redir_in(t_redir *redir, t_fd *fds); // Manage input redirection
t_redir			*new_vertex(t_global *global, t_lst *node); // Create new redirection node
void			handle_redir_out(t_redir *redir, t_fd *fds); // Manage output redirection
void			handle_dredir_out(t_redir *redir, t_fd *fds); // Handle double output redirection
void			add_back_redir(t_redir **head, t_redir *new); // Append new redirection
void			build_redir_list(t_global *global, t_lst **node); // Construct redirection list
bool			handle_redirection(t_redir *redir_list, t_fd *fds); // Execute redirection handling
bool			check_doubel_redir(t_global *global, t_redir **redir_list); // Check for double redirection

/*~~~~~~~~~~~~~~~~~~~~Main Functions~~~~~~~~~~~~~~~~~~~*/
/*
 * Core shell functions for initialization and starting 
 * command processing. This is critical for the shell’s 
 * lifecycle.
 */
bool			start(t_global *global); // Start shell operation
void			get_line(t_global *global); // Get user input line
void			free_2d_array(char ***leaks); // Free 2D array memory
void			malloc_failed(t_global *global); // Handle malloc failure gracefully

/*~~~~~~~~~~~~~~~~~~Tree Struct~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Function prototypes for handling command execution as a 
 * tree structure, enabling complex command compositions.
 */
int				exit_status(int status, t_global *global); // Get exit status of command
void			free_tree(t_tree **root); // Free command execution tree
int				execute_tree(t_global *global, t_tree *root); // Execute command tree
t_tree			*build_tree(t_global *global, t_exc_list **old); // Build execution tree from command list
int				execute_left(t_global *global, t_tree *root, int *pipe); // Execute left subtree
int				execute_right(t_global *global, t_tree *root, int *pipe); // Execute right subtree
int				execute_cmd(t_global *global, char **cmd, t_type_node type); // Execute command in tree

/*~~~~~~~~~~~~~~~~~~TTY Functions~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * TTY management functions to ensure correct interaction 
 * with user terminal input/output, particularly important 
 * for a CLI application like this.
 */
void			save_tty(t_global *global); // Save terminal state
void			reset_tty(t_global *global); // Reset terminal to saved state
void			handler(int sig); // Handle specific terminal signals

/*~~~~~~~~~~~~~~~~~~~Syscall Functions~~~~~~~~~~~~~~~~~~~*/
/*
 * Syscall wrapper functions for common operations. 
 * Implementing error handling within these can streamline 
 * broader operation error management.
 */
bool			ft_dup2(int fd1, int fd2); // Safe duplicate file descriptor
int				ft_dup(int fd1, int fd2); // Duplicate file descriptor with error check
bool			ft_close(int fd); // Close file descriptor with error check
bool			reset_sett(t_fd *fds); // Reset terminal settings
int				close_pipe(int *pipe); // Close a pipe

#endif
