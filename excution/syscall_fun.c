#include "bashy.h"

/**
 * ft_dup2 - Wrapper around the `dup2` system call to duplicate file descriptors.
 * @fd1: The source file descriptor to duplicate.
 * @fd2: The destination file descriptor to overwrite.
 * 
 * This function duplicates `fd1` into `fd2` using the `dup2` system call. It 
 * checks for errors and prints a custom error message if `dup2` fails. If 
 * successful, it closes `fd1` after duplication.
 * 
 * Return: SUCCESS on successful duplication, FAILURE otherwise.
 */
bool	ft_dup2(int fd1, int fd2)
{
	// Perform the dup2 system call and check for failure
	if (dup2(fd1, fd2) < 0)
		return (perror("bashy: dup2:"), FAILURE);

	// Close the original file descriptor after duplication
	close(fd1);

	return (SUCCESS);
}

/**
 * ft_dup - Wrapper around the `dup` system call to create a copy of a file descriptor.
 * @fd1: The variable to hold the duplicated file descriptor.
 * @fd2: The file descriptor to duplicate.
 * 
 * This function duplicates `fd2` using the `dup` system call and assigns the 
 * result to `fd1`. It checks for errors and prints a custom error message if 
 * `dup` fails.
 * 
 * Return: The new file descriptor on success, or -1 on failure.
 */
int	ft_dup(int fd1, int fd2)
{
	// Perform the dup system call and store the result in fd1
	fd1 = dup(fd2);

	// Check if the duplication failed
	if (fd1 < 0)
		return (perror("bashy: dup:"), -1);

	return (fd1);
}

/**
 * ft_close - Wrapper around the `close` system call to close a file descriptor.
 * @fd: The file descriptor to close.
 * 
 * This function closes the given file descriptor and checks for errors. 
 * If the `close` system call fails, it prints a custom error message.
 * 
 * Return: SUCCESS on successful closure, FAILURE otherwise.
 */
bool	ft_close(int fd)
{
	// Perform the close system call and check for failure
	if (close(fd) < 0)
		return (perror("bashy: close:"), FAILURE);

	return (SUCCESS);
}

/**
 * reset_sett - Resets file descriptors for standard input/output streams.
 * @fds: Pointer to a structure holding input (`fds->in`) and output (`fds->out`)
 *        file descriptors.
 * 
 * This function resets standard input/output to their respective file descriptors
 * stored in the `fds` structure. It duplicates the file descriptors to stdin (0)
 * and stdout (1) using `dup2`. After resetting, it sets the `fds->in` and 
 * `fds->out` fields to -1 to indicate they are no longer in use.
 * 
 * Return: SUCCESS on successful reset, FAILURE otherwise.
 */
bool	reset_sett(t_fd *fds)
{
	// Reset input file descriptor if it has been set
	if (fds->in != -1)
		if (ft_dup2(fds->in, 0) == FAILURE)  // Duplicate to stdin
			return (FAILURE);

	// Reset output file descriptor if it has been set
	if (fds->out != -1)
		if (ft_dup2(fds->out, 1) == FAILURE)  // Duplicate to stdout
			return (FAILURE);

	// Mark the file descriptors as inactive
	fds->in = -1;
	fds->out = -1;

	return (SUCCESS);
}
