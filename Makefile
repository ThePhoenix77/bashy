# Compiler command and options
CC = cc

# Name of the executable program
NAME = bashy

# Paths for the Readline library
READLINE_L = ~/.brew/opt/readline/lib
READLINE_I = ~/.brew/opt/readline/include

# Directory containing header files
HEADER = includes

# List of C source files for the project
CFILES = builtins/builtins_utils.c builtins/builtins_utils2.c \
         builtins/builtins_utils3.c builtins/builtins_utils4.c \
         builtins/ft_cd.c builtins/ft_echo.c \
         builtins/ft_env.c builtins/ft_exit.c builtins/ft_export.c \
         builtins/ft_pwd.c builtins/ft_unset.c \
         env/env_utils.c env/env_utils2.c env/env_utils3.c \
         env/shell_level.c \
         excution/build_tree.c excution/check_multi_redir.c \
         excution/cmd_list.c excution/exc_list_utils.c excution/exc_utils.c \
         excution/exc_utils2.c excution/execute_cmd.c excution/execute_left.c \
         excution/execute_one_cmd.c excution/execute_right.c \
         excution/execute_tree.c excution/exit_status.c \
         excution/main_exc.c excution/one_cmd_utils.c \
         excution/path_finder.c excution/prepare_exc_list.c \
         excution/redir_handle.c excution/redir_list.c \
         excution/syscall_fun.c \
         expanding/check_node_expn.c expanding/exp_utils.c \
         expanding/exp_utils2.c expanding/ft_expand.c \
         expanding/last_step_exp.c expanding/list_exp_fun.c \
         main.c utils/ft_tokeniz.c \
         Utils/check_redir.c Utils/check_syntax.c Utils/clean_memory.c \
         Utils/ft_heredoc.c Utils/here_doc_utils.c \
         Utils/linked_list_fun.c Utils/signals.c Utils/tty_handler.c \
         Utils/utils.c Utils/Utils_Tokeniz1.c Utils/Utils_Tokeniz2.c

# Object files derived from source files
OFILES = $(CFILES:%.c=%.o)

# Directory for the libft library
LIBFT_DIR = libft

# Library file for libft
LIBFT = $(LIBFT_DIR)/libft.a

# Command for removing files
RM = rm -rf

# Compiler flags for warnings
CC += -Wall -Werror -Wextra

# Default target to build the program
all: $(NAME)

# Target to build the libft library - makes sure it's built before linking
$(LIBFT):
	make -C $(LIBFT_DIR)

# Target to link object files and create the final executable
$(NAME): $(OFILES) $(LIBFT)
	$(CC) $(OFILES) -L $(LIBFT_DIR) -L $(READLINE_L) -lft -lreadline -o $(NAME)

# Implicit rule for generating object files from C source files
%.o: %.c $(HEADER)
	$(CC) -I $(HEADER) -I $(READLINE_I) -c $< -o $@

# Clean up object files and and the libft build
clean:
	make clean -C $(LIBFT_DIR) # Clean libft objects
	$(RM) -rf $(OFILES) # Remove object files

# Clean step including removing the final executable
fclean: clean
	make fclean -C $(LIBFT_DIR) # Clean libft fully
	$(RM) -rf $(NAME) # Remove final executable

# Rebuild everything from scratch
re: fclean all
