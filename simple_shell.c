#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Main function entry point for the simple shell.
 * @return Always returns 0 upon successful execution.
 */

int main(void)
{
char command[1024];
pid_t pid;
int status;
while (1)
{
printf("$ ");
if (!fgets(command, sizeof(command), stdin))
{
printf("\n");
break;
}
command[strcspn(command, "\n")] = '\0';
if (feof(stdin))
{
printf("\n");
break;
}
if (command[0] == '\0')
{
continue;
}
pid = fork();
if (pid < 0)
{
perror("fork");
exit(EXIT_FAILURE);
}
else if (pid == 0)
{
if (execve(command, NULL, environ) == -1)
{
perror(command);
exit(EXIT_FAILURE);
}
}
else
{
waitpid(pid, &status, 0);
}
}
return (0);
}

