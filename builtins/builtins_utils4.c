#include "bashy.h"

/* Updates the value of an existing environmental variable. It constructs
   a new content string in the format "name=new_val", ensuring to properly 
   handle memory by freeing previous values. Returns the updated content
   or NULL if the variable does not exist or memory allocation fails. */
char	*update_the_env_var(t_global *global, char *name, char *new_val)
{
    t_env_list *var;
    char	   *new_cont;
    char	   *tmp;

    var = find_var(global->env_list, name); // Find the variable by name
    if (var)
    {	
        tmp = ft_strjoin(name, "="); // Create "name="
        if (!tmp)
            return (NULL); // Return NULL on malloc failure
        new_cont = ft_strjoin(tmp, new_val); // Construct new content
        free(tmp); // Free temporary string after use
        if (!new_cont)
            return (NULL); // Return NULL on malloc failure
        free(var->content); // Free the old content
        var->content = new_cont; // Update the variable content
        return (var->content); // Return updated content
    }
    return (NULL); // Return NULL if variable not found
}

/* Obtains the current working directory and updates the "PWD" environmental
   variable accordingly. It handles errors in retrieving the working directory 
   and performs necessary cleanup. */
void	update_pwd(t_global *global)
{
    char	*cwd;

    cwd = getcwd(NULL, 0); // Get current working directory
    if (!cwd)
    {
        perror("getcwd"); // Handle error in getting cwd
        return ; // Exit the function on failure
    }
    update_the_env_var(global, "PWD", cwd); // Update "PWD" variable
    free(cwd); // Free allocated cwd string
}

/* Changes the current working directory to the one stored in "OLDPWD". 
   It handles cases where "OLDPWD" is not set or is empty, as well as 
   errors from the `chdir` system call, providing feedback to the user. */
void	change_to_oldpwd(t_global *global)
{
    t_env_list	*old_pwd;
    char		*old_pwd_value;

    old_pwd = find_var(global->env_list, "OLDPWD"); // Retrieve "OLDPWD"
    if (!old_pwd) // Handle case when "OLDPWD" is not set
    {
        ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO); // Error message
        global->exit_status = 1; // Set exit status to indicate failure
        return ;
    }
    old_pwd_value = get_env_value(old_pwd->content); // Get value of "OLDPWD"
    if (old_pwd_value[0] == '\0') // Check if value is empty
    {
        ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO); // Error message
        global->exit_status = 1; // Set exit status
        return ;
    }
    if (!old_pwd_value || chdir(old_pwd_value) != 0) // Try changing to OLDPWD
    {
        perror("cd"); // Handle error from chdir
        global->exit_status = 1; // Set exit status
        return ;
    }
    ft_putstr_fd(old_pwd_value, STDOUT_FILENO); // Print new working directory
    ft_putstr_fd("\n", STDOUT_FILENO); // New line for clarity
}

/* Checks if the "OLDPWD" variable exists in the environment. It provides
   feedback if it is not set and updates the exit status accordingly. */
int	oldpwd_check(t_global *global)
{
    if (!find_var(global->env_list, "OLDPWD")) // Verify existence of "OLDPWD"
    {
        ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO); // Error message
        global->exit_status = 1; // Set exit status
        return (1); // Return indication of failure
    }
    return (0); // Return success
}

/* Prints a standardized error message for the cd command when an error occurs.
   This utilizes the provided args array to specify the issue clearly. */
void	err_msg(char **args)
{
    ft_putstr_fd("cd: ", 2); // Prefix error with "cd: "
    ft_putstr_fd(args[1], 2); // Include the problematic argument
    ft_putstr_fd(" : ", 2); // Separator for clarity
}
