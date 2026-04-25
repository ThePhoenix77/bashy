#include "bashy.h"

// Signal handler for SIGINT and SIGQUIT
void	signals_handler(int sig)
{
	if (sig == SIGINT) // Handle Ctrl+C
	{
		printf("\n"); // Print a newline
		rl_on_new_line(); // Move to a new line in the readline buffer
		rl_redisplay(); // Redisplay the prompt
		g_exit_status = 1; // Set the global exit status to 1
	}
	else if (sig == SIGQUIT) // Handle Ctrl+\ (SIGQUIT)
		signal(SIGQUIT, SIG_IGN); // Ignore SIGQUIT
}

// Update the exit status based on the global variable
void	exit_status_update(t_global *global)
{
	if (g_exit_status == 1) // Check if the exit status is set
	{
		global->exit_status = g_exit_status; // Update global exit status
		g_exit_status = 0; // Reset global exit status
	}
}

// Set up signal handling for SIGINT and SIGQUIT
void	mask_and_catch_signals(t_sigaction *sig)
{
	sig->sa_handler = signals_handler; // Assign the handler
	sigemptyset(&sig->sa_mask); // Initialize signal mask
	sigaddset(&sig->sa_mask, SIGINT); // Add SIGINT to the mask
	sigaddset(&sig->sa_mask, SIGQUIT); // Add SIGQUIT to the mask
	sigaction(SIGINT, sig, NULL); // Set the action for SIGINT
	sigaction(SIGQUIT, sig, NULL); // Set the action for SIGQUIT
}

// Initialize the signal actions for the bashy
void	init_sigaction(void)
{
	t_sigaction	sig; // Declare a sigaction struct

	ft_memset(&sig, 0, sizeof(sig)); // Zero out the struct
	mask_and_catch_signals(&sig); // Set up the signal handlers
}
