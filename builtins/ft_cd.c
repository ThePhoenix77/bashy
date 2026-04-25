#include "bashy.h"

/* Retrieves the value of an environment variable from a string formatted as 
   "key=value". It searches for the '=' character, returning the substring 
   after it, or NULL if the '=' is not found. */
char	*get_env_value(char *content)
{
    char	*equal_sign;

    equal_sign = ft_strchr(content, '='); // Locate the equal sign
    if (!equal_sign)
        return (NULL); // Return NULL if not found
    return (equal_sign + 1); // Return the substring after '='
}

/* Obtains the current working directory, allocating memory for it. 
   Handles errors by printing a message and returning NULL if unable to get the cwd. */
char	*get_pwd(void)
{
    char	*cwd;

    cwd = getcwd(NULL, 0); // Get current working directory
    if (cwd == NULL)
    {
        perror("getcwd"); // Handle error
        return (NULL);
    }
    return (cwd); // Return the cwd
}

/* Sets the OLDPWD environment variable. If it already exists, this function 
   updates its value based on the provided current directory, managing memory 
   correctly to avoid leaks. */
void	set_oldpwd(t_global *global, char *the_pwd)
{
    t_env_list	*old_tmp;
    char		*old_tmp_val;

    old_tmp = find_var(global->env_list, "OLDPWD"); // Find OLDPWD variable
    if (!old_tmp) // If OLDPWD does not exist
    {
        add_or_update_env_var(global, "OLDPWD", "", SHOW); // Initialize with empty string
        return ;
    }
    old_tmp_val = ft_strdup(old_tmp->content + 7); // Duplicate current OLDPWD value
    if (old_tmp_val)
    {
        if (old_tmp_val[0] == '\0') // If OLDPWD was empty
            add_or_update_env_var(global, "OLDPWD", the_pwd, SHOW); // Set OLDPWD to the current
        else
            add_or_update_env_var(global, "OLDPWD", "", SHOW); // Otherwise, clear it

        free(old_tmp_val); // Free temporary duplication
    }
}

/* Updates the OLDPWD variable with the current value of PWD. If PWD does not exist,
   it calls set_oldpwd with the provided pwd. This is crucial for tracking changes in 
   the working directory. */
void	update_oldpwd(t_global *global, char *the_pwd)
{
    t_env_list	*tmp;
    char		*old_pwd;

    tmp = find_var(global->env_list, "PWD"); // Get current PWD variable
    if (tmp)
    {
        old_pwd = ft_strdup(tmp->content + 4); // Duplicate the current PWD value
        if (old_pwd)
        {
            add_or_update_env_var(global, "OLDPWD", old_pwd, HIDE); // Update OLDPWD
            free(old_pwd); // Free the duplicated string
        }
    }
    else
        set_oldpwd(global, the_pwd); // If no PWD set, call the initializer
}

/* Retrieves the home path from the environment variables. If HOME is not set or
   invalid, it prints an error message and updates the global exit status accordingly. */
char	*get_home_path(t_global *global)
{
    t_env_list	*home;
    char		*home_p;

    home = find_var(global->env_list, "HOME"); // Find HOME variable
    if (!home) // Handle missing HOME variable
    {
        ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO); // Print error
        global->exit_status = 1; // Update exit status to indicate failure
        return (NULL);
    }
    home_p = get_env_value(home->content); // Get the value of HOME
    if (!home_p) // Handle invalid HOME value
    {
        ft_putstr_fd("cd: HOME value is invalid\n", STDERR_FILENO); // Print error
        global->exit_status = 1; // Update exit status
        return (NULL);
    }
    return (home_p); // Return the home path
}

/* Handles changing the directory to the previous working directory (OLDPWD). 
   Checks if OLDPWD is set before proceeding, and updates directory variables accordingly. */
void	handle_dash(t_global *global, char *the_pwd)
{
    if (oldpwd_check(global) == 1) // Check if OLDPWD is available
    {
        free(the_pwd); // Free current working directory
        return ;
    }
    change_to_oldpwd(global); // Change to OLDPWD
    update_oldpwd(global, the_pwd); // Update OLDPWD variable
    update_pwd(global); // Update current PWD variable
    global->exit_status = 0; // Set exit status to success
    free(the_pwd); // Free allocated current working directory
}

/* Handles changing to the specified new path, checking for errors, and
   updating directory environment variables. If an error occurs, 
   it prints the relevant error message. */
void	handle_cd_path(t_global *global, char *new_p, char *the_pwd)
{
    if (chdir(new_p) != 0) // Attempt to change directory
    {
        err_msg(&new_p); // Print error message
        perror(""); // Provide system error detail
        global->exit_status = 1; // Set exit status to indicate failure
        free(the_pwd); // Clean up allocated path
        return ;
    }
    update_oldpwd(global, the_pwd); // Update OLDPWD
    update_pwd(global); // Update PWD
    global->exit_status = 0; // Set exit status to success
    free(the_pwd); // Free previous working directory
}

/* Main function to handle the cd command. It manages changing directories, 
   handles paths (including home and OLDPWD), and keeps track of previous 
   directories by updating environment variables. */
void	ft_cd(t_global *global, char **args)
{
    char	*new_p;
    char	*the_pwd;

    the_pwd = get_pwd(); // Get current working directory
    if (!the_pwd)
        return ; // Exit if unable to get cwd
    if (args[1] && ft_strcmp(args[1], "-") == SUCCESS) // Check if argument is '-'
    {
        handle_dash(global, the_pwd); // Handle changing to OLDPWD
        return ; // Exit function after handling
    }
    if (args[1]) // If a path is provided
        new_p = args[1]; // Set new path to the provided argument
    else
        new_p = get_home_path(global); // If no path, default to HOME
    if (!new_p || ft_strcmp(new_p, "") == SUCCESS) // Check for valid new path
    {
        free(the_pwd); // Free current directory
        return ; // Exit if new path is invalid
    }
    handle_cd_path(global, new_p, the_pwd); // Change to the new specified path
}
