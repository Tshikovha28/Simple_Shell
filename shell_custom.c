#include "shell_custom.h"

/**
 * hsh_custom - main shell loop
 * @info: the parameter & return info struct
 * @av: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int hsh_custom(info_t *info, char **av)
{
    ssize_t r = 0;
    int builtin_ret = 0;

    while (r != -1 && builtin_ret != -2)
    {
        clear_info_custom(info);
        if (interactive_custom(info))
            custom_puts("$ ");
        custom_putchar(BUF_FLUSH);
        r = get_input_custom(info);
        if (r != -1)
        {
            set_info_custom(info, av);
            builtin_ret = find_builtin_custom(info);
            if (builtin_ret == -1)
                find_cmd_custom(info);
        }
        else if (interactive_custom(info))
            custom_putchar('\n');
        free_info_custom(info, 0);
    }
    write_history_custom(info);
    free_info_custom(info, 1);
    if (!interactive_custom(info) && info->status)
        custom_exit(info->status);
    if (builtin_ret == -2)
    {
        if (info->err_num == -1)
            custom_exit(info->status);
        custom_exit(info->err_num);
    }
    return (builtin_ret);
}

/**
 * find_builtin_custom - finds a builtin command
 * @info: the parameter & return info struct
 *
 * Return: -1 if builtin not found,
 *         0 if builtin executed successfully,
 *         1 if builtin found but not successful,
 *         -2 if builtin signals exit()
 */
int find_builtin_custom(info_t *info)
{
    int i, built_in_ret = -1;
    builtin_table builtintbl[] = {
        {"exit", _myexit_custom},
        {"env", _myenv_custom},
        {"help", _myhelp_custom},
        {"history", _myhistory_custom},
        {"setenv", _mysetenv_custom},
        {"unsetenv", _myunsetenv_custom},
        {"cd", _mycd_custom},
        {"alias", _myalias_custom},
        {NULL, NULL}};

    for (i = 0; builtintbl[i].type; i++)
        if (custom_strcmp(info->argv[0], builtintbl[i].type) == 0)
        {
            info->line_count++;
            built_in_ret = builtintbl[i].func(info);
            break;
        }
    return (built_in_ret);
}

/**
 * find_cmd_custom - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void find_cmd_custom(info_t *info)
{
    char *path = NULL;
    int i, k;

    info->path = info->argv[0];
    if (info->linecount_flag == 1)
    {
        info->line_count++;
        info->linecount_flag = 0;
    }
    for (i = 0, k = 0; info->arg[i]; i++)
        if (!is_delim_custom(info->arg[i], " \t\n"))
            k++;
    if (!k)
        return;

    path = find_path_custom(info, _getenv_custom(info, "PATH="), info->argv[0]);
    if (path)
    {
        info->path = path;
        fork_cmd_custom(info);
    }
    else
    {
        if ((interactive_custom(info) || _getenv_custom(info, "PATH=") || info->argv[0][0] == '/') && is_cmd_custom(info, info->argv[0]))
            fork_cmd_custom(info);
        else if (*(info->arg) != '\n')
        {
            info->status = 127;
            print_error_custom(info, "not found\n");
        }
    }
}

/**
 * fork_cmd_custom - forks an exec thread to run cmd
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fork_cmd_custom(info_t *info)
{
    pid_t child_pid;

    child_pid = fork();
    if (child_pid == -1)
    {
        /* TODO: PUT ERROR FUNCTION */
        perror("Error:");
        return;
    }
    if (child_pid == 0)
    {
        if (execve(info->path, info->argv, get_environ_custom(info)) == -1)
        {
            free_info_custom(info, 1);
            if (errno == EACCES)
                custom_exit(126);
            custom_exit(1);
        }
        /* TODO: PUT ERROR FUNCTION */
    }
    else
    {
        wait(&(info->status));
        if (WIFEXITED(info->status))
        {
            info->status = WEXITSTATUS(info->status);
            if (info->status == 126)
                print_error_custom(info, "Permission denied\n");
        }
    }
}
