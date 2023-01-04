#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "minishell.h"

#include <string.h>

char	*search_path(const char *filename);
void	validate_access(const char *path, const char *filename);
pid_t	exec_pipeline(t_node *node);
int		wait_pipeline(pid_t last_pid);
int		exec(t_node *node);

int	exec(t_node *node)
{
	pid_t	last_pid;
	int		status;

	if (open_redir_file(node) < 0)
		return (ERROR_OPEN_REDIR);
	if (node->next == NULL && is_builtin(node))
		status = exec_builtin(node);
	else
	{
		last_pid = exec_pipeline(node);
		status = wait_pipeline(last_pid);
	}
	return (status);
}

char	*search_path_mode(const char *filename, int mode)
{
	char	path[PATH_MAX];
	char	*value;
	char	*end;

	value = xgetenv("PATH");
	while (*value)
	{
		// /bin:/usr/bin
		//     ^
		//     end
		bzero(path, PATH_MAX);
		end = strchr(value, ':');
		if (end)
			strncpy(path, value, end - value);
		else
			strlcpy(path, value, PATH_MAX);
		strlcat(path, "/", PATH_MAX);
		strlcat(path, filename, PATH_MAX);
		if (access(path, mode) == 0)
		{
			char	*dup;

			dup = strdup(path);
			if (dup == NULL)
				fatal_error("strdup");
			return (dup);
		}
		if (end == NULL)
			return (NULL);
		value = end + 1;
	}
	return (NULL);
}

char	*search_path(const char *filename)
{
	char	*path;

	path = search_path_mode(filename, X_OK);
	if (path)
		return (path);
	path = search_path_mode(filename, F_OK);
	return (path);
}

void	validate_access(const char *path, const char *filename)
{
	struct stat	st;

	if (path == NULL)
		err_exit(filename, "command not found", 127);
	if (strcmp(filename, "") == 0)
		err_exit(filename, "command not found", 127);
	if (strcmp(filename, "..") == 0)
		err_exit(filename, "command not found", 127);
	if (access(path, F_OK) < 0)
		err_exit(filename, "command not found", 127);
	if (stat(path, &st) < 0)
		fatal_error("fstat");
	if (S_ISDIR(st.st_mode))
		err_exit(filename, "is a directory", 126);
	if (access(path, X_OK) < 0)
		err_exit(path, "Permission denied", 126);
}

int	exec_nonbuiltin(t_node *node) __attribute__((noreturn));
int	exec_nonbuiltin(t_node *node)
{
	char		*path;
	char		**argv;

	do_redirect(node->command->redirects);
	argv = token_list_to_argv(node->command->args);
	path = argv[0];
	if (strchr(path, '/') == NULL)
		path = search_path(path);
	validate_access(path, argv[0]);
	execve(path, argv, get_environ(envmap));
	free_argv(argv);
	reset_redirect(node->command->redirects);
	fatal_error("execve");
}

pid_t	exec_pipeline(t_node *node)
{
	pid_t		pid;

	if (node == NULL)
		return (-1);
	prepare_pipe(node);
	pid = fork();
	if (pid < 0)
		fatal_error("fork");
	else if (pid == 0)
	{
		// child process
		reset_signal();
		prepare_pipe_child(node);
		if (is_builtin(node))
			exit(exec_builtin(node));
		else
			exec_nonbuiltin(node);
	}
	// parent process
	prepare_pipe_parent(node);
	if (node->next)
		return (exec_pipeline(node->next));
	return (pid);
}

int	wait_pipeline(pid_t last_pid)
{
	pid_t	wait_result;
	int		status;
	int		wstatus;

	while (1)
	{
		wait_result = wait(&wstatus);
		if (wait_result == last_pid)
		{
			if (WIFSIGNALED(wstatus))
				status = 128 + WTERMSIG(wstatus);
			else
				status = WEXITSTATUS(wstatus);
		}
		else if (wait_result < 0)
		{
			if (errno == ECHILD)
				break ;
			else if (errno == EINTR)
				continue ;
			else
				fatal_error("wait");
		}
	}
	return (status);
}
