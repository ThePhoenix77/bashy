#include "bashy.h"

/**
 * handle_redir_out - Handles output redirection ('>') by opening the file in
 *                    overwrite mode.
 * @redir: Pointer to the redirection structure that contains the file name.
 * @fds: Pointer to the file descriptor structure used during execution.
 * 
 * This function opens the file specified in the redirection node for writing 
 * (with truncation), and updates the output file descriptor (`fds->out`). 
 * If an output file descriptor is already open, it is closed before assigning 
 * the new one.
 */
void	handle_redir_out(t_redir *redir, t_fd *fds)
{
	int	fd;

	// Open the file for writing, create if necessary, and truncate its content
	fd = open(redir->file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)  // Handle file open failure
		perror("bashy: open");

	// Close the current output file descriptor if it is already open
	if (fds->out >= 0)
		close(fds->out);

	// Update the output file descriptor with the newly opened file
	fds->out = fd;
}

/**
 * handle_redir_in - Handles input redirection ('<') by opening the file for reading.
 * @redir: Pointer to the redirection structure that contains the file name.
 * @fds: Pointer to the file descriptor structure used during execution.
 * 
 * This function opens the file specified in the redirection node for reading 
 * and updates the input file descriptor (`fds->in`). If the file cannot be 
 * opened, it prints an error message. If a here-document is involved, the 
 * corresponding file is deleted after being used.
 */
void	handle_redir_in(t_redir *redir, t_fd *fds)
{
	int	fd;

	// Open the file for reading
	fd = open(redir->file_name, O_RDONLY);
	if (fd < 0)  // Handle file open failure and print a custom error message
	{
		ft_putstr_fd("bashy: ", 2);
		perror(redir->file_name);
	}

	// Close the current input file descriptor if it is already open
	if (fds->in >= 0)
		close(fds->in);

	// Update the input file descriptor with the newly opened file
	fds->in = fd;

	// If the file is a here-document, delete it after use
	if (redir->file_type == HERE_DOC_FILE)
		unlink(redir->file_name);
}

/**
 * handle_dredir_out - Handles append redirection ('>>') by opening the file in 
 *                     append mode.
 * @redir: Pointer to the redirection structure that contains the file name.
 * @fds: Pointer to the file descriptor structure used during execution.
 * 
 * This function opens the file specified in the redirection node for writing
 * in append mode (with creation if necessary). It updates the output file 
 * descriptor (`fds->out`). If an output file descriptor is already open, it 
 * is closed before assigning the new one.
 */
void	handle_dredir_out(t_redir *redir, t_fd *fds)
{
	int	fd;

	// Open the file for writing in append mode, create if necessary
	fd = open(redir->file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)  // Handle file open failure
		perror("bashy: open");

	// Close the current output file descriptor if it is already open
	if (fds->out >= 0)
		close(fds->out);

	// Update the output file descriptor with the newly opened file
	fds->out = fd;
}

/**
 * handle_redirection - Processes the entire redirection list and performs
 *                      necessary file redirections (input/output).
 * @redir_list: Pointer to the head of the redirection list.
 * @fds: Pointer to the file descriptor structure used during execution.
 * 
 * This function iterates through the redirection list, handling each redirection 
 * type accordingly (input, output, or append). It also detects errors like 
 * ambiguous redirections. If the file descriptors are successfully updated, 
 * the function returns SUCCESS; otherwise, it returns FAILURE.
 * 
 * Return: SUCCESS if at least one redirection is processed, FAILURE otherwise.
 */
bool	handle_redirection(t_redir *redir_list, t_fd *fds)
{
	t_redir	*redir;

	// Traverse the redirection list
	redir = redir_list;
	while (redir)
	{
		// Handle ambiguous redirection errors
		if (redir->file_type == ERROR_DIS)
		{
			ft_putstr_fd(redir->file_name, 2);
			ft_putchar_fd('\n', 2);
			exit(1);
		}

		// Handle different redirection types (output, input, append)
		else if (redir->type == REDIR_OUT)
			handle_redir_out(redir, fds);
		else if (redir->type == REDIR_IN)
			handle_redir_in(redir, fds);
		else if (redir->type == DREDIR_OUT)
			handle_dredir_out(redir, fds);

		// Move to the next redirection in the list
		redir = redir->next;
	}

	// Check if any file descriptor was successfully updated
	if (fds->in > 0 || fds->out > 0)
		return (SUCCESS);
	return (FAILURE);
}
