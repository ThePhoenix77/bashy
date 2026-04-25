#include "bashy.h"

// Check if the command is a built-in command and execute it
int	check_builtins(t_global *global, char **cmd)
{
	if (ft_strcmp(cmd[0], "echo") == SUCCESS)
		return (ft_echo(global, cmd), SUCCESS);
	else if (ft_strcmp(cmd[0], "cd") == SUCCESS)
		return (ft_cd(global, cmd), SUCCESS);
	else if (ft_strcmp(cmd[0], "pwd") == SUCCESS)
		return (ft_pwd(global), SUCCESS);
	else if (ft_strcmp(cmd[0], "export") == SUCCESS)
		return (ft_export(global, cmd), SUCCESS);
	else if (ft_strcmp(cmd[0], "unset") == SUCCESS)
		return (ft_unset(global, cmd), SUCCESS);
	else if (ft_strcmp(cmd[0], "env") == SUCCESS)
		return (ft_env(global), SUCCESS);
	else if (ft_strcmp(cmd[0], "exit") == SUCCESS)
		return (ft_exit(global, cmd), SUCCESS);
	else
		return (FAILURE); // Not a built-in command
}

// Check the type of file specified by the command
void	check_file_type(char **cmd)
{
	struct stat	file_struct; // Structure to hold file information

	// Get file status
	if (lstat(cmd[0], &file_struct) == -1)
		(perror("bashy: lstat"), exit(1)); // Error handling for lstat

	// Check if it's a directory
	if (S_ISDIR(file_struct.st_mode))
	{
		ft_putstr_fd("bashy: ", 2);
		ft_putstr_fd(cmd[0], 2);
		ft_putstr_fd(": is a directory\n", 2);
		exit(126); // Exit with status indicating command is a directory
	}

	// Check if the file is executable
	if (!(file_struct.st_mode & S_IXUSR))
	{
		ft_putstr_fd("bashy: ", 2);
		ft_putstr_fd(cmd[0], 2);
		ft_putstr_fd(": Permission denied\n", 2);
		exit(126); // Exit with status indicating permission denied
	}
}

// Execute the given command
int	execute_cmd(t_global *global, char **cmd, t_type_node type)
{
	// If the command is a built-in, execute it
	if (type == BUILTIN)
	{
		check_builtins(global, cmd);
		exit (global->exit_status); // Exit with the built-in command's exit status
	}

	// Handle special command case
	if (cmd[0][0] == '\x02')
	{
		global->exit_status = 127; // Set exit status for error
		ft_cmd_err(cmd[0]); // Handle command error
	}

	// Check if the command file exists
	if (access(cmd[0], F_OK) == -1)
	{
		ft_putstr_fd("bashy: ", 2);
		(perror(cmd[0]), exit(127)); // Print error and exit with command not found status
	}

	check_file_type(cmd); // Check if the command is a valid file and executable

	// Execute the command using execve
	if (execve(cmd[0], cmd, global->myenv) == -1)
		perror("bashy: execve:"); // Print error if execve fails
	exit(1); // Exit if execve fails
}
