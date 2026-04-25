#include "bashy.h"

// Free an array of strings (tab)
void	free_tab(char **tab)
{
	int	i;

	i = 0;
	if (!tab) // Check for NULL pointer
		return ;
	while (tab[i]) // Free each string in the array
		free(tab[i++]);
	free(tab); // Free the array itself
}

// Find the executable path for a command in the given split paths
char	*find_path(char **split, char *cmd)
{
	int		i;
	char	*path;
	char	*tmp;

	i = 0;
	while (split[i]) // Iterate through the split paths
	{
		// Join the current path with the command
		tmp = ft_strjoin(split[i], "/");
		path = ft_strjoin(tmp, cmd);
		free(tmp); // Free temporary path
		if (path && access(path, F_OK | X_OK) == 0) // Check if the file exists and is executable
			return (path); // Return the valid path
		free(path); // Free the path if it is not valid
		i++;
	}
	return (NULL); // No valid path found
}

// Get the PATH environment variable from the global environment
char	*get_the_path(t_global *global)
{
	t_env_list	*env_path;
	char		*path;
	int			len_skip;

	env_path = get_var_env(global, "PATH="); // Get the PATH variable
	if (!env_path)
		return (NULL); // Return NULL if PATH is not set
	path = ft_strdup(env_path->content); // Duplicate the PATH content
	if (!path)
		malloc_failed(global); // Handle memory allocation failure
	len_skip = ft_strlen_un_char(path, '='); // Find the length to skip the "PATH="
	if (len_skip == -2)
	{
		free(path);
		return (NULL); // Return NULL if there was an error
	}
	update_line(&path, len_skip + 1, global); // Update the line to remove "PATH="
	return (path); // Return the updated path
}

// Get the full path for the given command
char	*get_path(char *cmd, t_global *global)
{
	// int		i;
	char	**split;
	char	*path;
	char	*path_env;

	// i = 0;
	if (ft_strchr(cmd, '/')) // Check if the command has a path
		return (ft_strdup(cmd)); // Return the command itself if it has a path
	path_env = get_the_path(global); // Get the PATH environment variable
	if (!path_env)
		return (NULL); // Return NULL if PATH is not found
	split = ft_split(path_env, ':'); // Split the PATH into an array
	free(path_env); // Free the original PATH string
	path = find_path(split, cmd); // Find the command path in the split paths
	free_tab(split); // Free the split array
	if (path)
		return (path); // Return the valid path found
	return (NULL); // No valid path found
}
