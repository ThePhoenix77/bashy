#include "bashy.h"

// Global variable to store the exit status of the shell
int g_exit_status = 0;

// Function that handles user input in the shell
void get_line(t_global *global)
{
    // Load command history from the specified file
    read_history(HISTORY_FILE);
    while (1) // Infinite loop to continuously get user input
    {
        // Prompt user for input and read a line
        global->line_input = readline("bashy$ ");

        // Validate the user input; if valid, continue to next iteration
        if (check_valid_in(global) == SUCCESS)
            continue;

        // Handle the case when input validation fails
        if (check_valid_in(global) == FAILURE)
        {
            free_memory(global); // Free allocated memory for the current context
            if (global->env_list) // If there are environment variables, free them
                free_env_list(&global->env_list);
            write(1, "\x1B[Fbashy$ exit\n", 15); // Print exit message
            g_exit_status = 0; // Reset exit status
            break; // Exit the loop
        }

        exit_status_update(global); // Update the exit status based on the command result
        // Add to history and handle any error in executing a command
        (add_history(global->line_input), skip_ec_t(global, 0));

        // Check input validity again
        if (check_valid_in(global) == SUCCESS)
            continue;

        // Attempt to start command execution
        if (start(global) == FAILURE)
            continue;

        // Clean up memory on failure
        free_memory(global);
    }
    // Write the updated command history to file
    write_history(HISTORY_FILE);
}

// Function to execute commands based on parsed input
bool start(t_global *global)
{
    global->type = WORD; // Set command type
    global->state = GENERAL; // Set state to general
    ft_tokeniz(global); // Tokenize the input command

    // Check for syntax errors in the command
    if (check_syntax(global) == FAILURE)
    {
        free_memory(global); // Free resources on failure
        return (FAILURE);
    }

    // Check for too many heredocs
    if (check_num_heredoc(global) == FAILURE)
    {
        ft_putstr_fd(ERR_MANY_HERDOCS, 2); // Output error message
        free_memory(global); // Clean up memory
        if (global->env_list) // Free environment list if it exists
            free_env_list(&global->env_list);
        global->exit_status = 2; // Set exit status to indicate error
        exit(global->exit_status); // Exit the shell
    }

    // Check for valid heredoc
    if (check_heredoc(global) == FAILURE)
        return (free_memory(global), FAILURE); // Clean up and fail

    // Process any variable expansions
    if (check_expand(global) == FAILURE)
        return (free_memory(global), FAILURE); // Clean up and fail

    // Execute the main expansion function and handle redirections
    (main_exp_fun(global), check_redir(global));
    // Open heredoc files if needed and execute the main command
    (open_heredoc_file(global), main_exc(global));
    
    return (SUCCESS); // Indicate success
}

// Main function to initialize the shell
int main(int arc, char **arv, char **env)
{
    t_global global; // Declare global structure to hold shell state

    arc = 0; // Unused parameter
    arv = NULL; // Unused parameter
    (void)arc;
    (void)*arv;
    if (isatty(0) == 0) // Check if the input is from a terminal
        return (-1); // Return error if not

    init_sigaction(); // Set up signal handling
    init_global(&global, env); // Initialize global context with environment variables

    // Check if the environment is valid; if not, handle memory allocation failure
    if (check_env(&global) == FAILURE)
        malloc_failed(&global);

    // Start the input reading loop
    get_line(&global);
    
    // Clear command history before exiting
    clear_history();
}
