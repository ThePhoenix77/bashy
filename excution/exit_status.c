#include "bashy.h"

// Determine the exit status based on the status of a child process
int	exit_status(int status, t_global *global)
{
	if (WIFEXITED(status)) // Check if the child exited normally
		return (WEXITSTATUS(status)); // Return the exit status
	if (WTERMSIG(status) != SIGINT) // Check if terminated by a signal other than SIGINT
	{
		if (WTERMSIG(status) == SIGQUIT) // Check if terminated by SIGQUIT
		{
			reset_tty(global); // Reset the terminal settings
			printf("Quit: "); // Print "Quit: "
			printf("%d", WTERMSIG(status)); // Print the signal number
		}
	}
	printf("\n"); // Print a newline for formatting
	return (128 + WTERMSIG(status)); // Return the exit status based on the signal
}

// Close the read and write ends of a pipe
int	close_pipe(int *pipe)
{
	if (close(pipe[0]) < 0) // Close the read end of the pipe
		return (-1); // Return -1 if closing fails
	if (close(pipe[1]) < 0) // Close the write end of the pipe
		return (-1); // Return -1 if closing fails
	return (2); // Return the number of pipes closed
}

// Set the signal handling for SIGINT and SIGQUIT to default behavior
void	sig_dfl(void)
{
	signal(SIGINT, SIG_DFL); // Set SIGINT to default action
	signal(SIGQUIT, SIG_DFL); // Set SIGQUIT to default action
}

// Ignore the SIGINT and SIGQUIT signals
void	sig_ign(void)
{
	signal(SIGINT, SIG_IGN); // Ignore SIGINT
	signal(SIGQUIT, SIG_IGN); // Ignore SIGQUIT
}

// Set the signal handler for SIGINT and SIGQUIT to a custom handler
void	sig_hnd(void)
{
	signal(SIGINT, signals_handler); // Set the custom handler for SIGINT
	signal(SIGQUIT, signals_handler); // Set the custom handler for SIGQUIT
}
