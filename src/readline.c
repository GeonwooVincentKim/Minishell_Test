/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geonwkim <geonwkim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 15:31:37 by geonwkim          #+#    #+#             */
/*   Updated: 2024/08/01 19:05:52 by geonwkim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include	"../include/readline.h"
#include	"../include/error.h"
#include	"../libft/libft.h"
#include	<sys/wait.h>
#include	<errno.h>

// __attribute__((destructor))
// static void destructor() {
//     system("leaks -q a.out");
// }

// char	*search_path(const char *file_name)
// {
// 	char	path[PATH_MAX];
// 	char	*value;
// 	char	*end;
// 	char	*dup;

// 	value = getenv("PATH");
// 	while (*value)
// 	{
// 		bzero(path, PATH_MAX);
// 		end = ft_strchr(value, ':');
// 		if (end)
// 			strncpy(path, value, end - value);
// 		else
// 			strlcpy(path, value, PATH_MAX);
// 		strlcat(path, "/", PATH_MAX);
// 		strlcat(path, file_name, PATH_MAX);
// 		if (access(path, X_OK) == 0)
// 		{
// 			dup = strdup(path);
// 			if (dup == NULL)
// 				fatal_error("strdup");
// 			return (dup);
// 		}
// 		if (end == NULL)
// 			return (NULL);
// 		value = end + 1;
// 	}
// 	return (NULL);
// }

void	validate_access(const char *path, const char *file_name)
{
	if (path == NULL)
		error_exit(file_name, "command not found", 127);
	if (access(path, F_OK) < 0)
		error_exit(file_name, "command not found", 127);
}

// int	exec_cmd(t_node *node)
// {
// 	extern char		**environ;
// 	char			*path;
// 	pid_t			pid;
// 	int				wstatus;
// 	char			**argv;

// 	if (node == NULL)
// 		return (-1);
// 	pid = fork();
// 	if (pid < 0)
// 		fatal_error("fork");
// 	else if (pid == 0)
// 	{
// 		argv = token_to_argv(node->args);
// 		path = argv[0];
// 		if (ft_strchr(path, '/') == NULL)
// 			path = find_path(path);
// 		validate_access(path, argv[0]);
// 		execve(path, argv, environ);
// 		fatal_error("execve");
// 	}
// 	else
// 	{
// 		wait(&wstatus);
// 		return (WEXITSTATUS(wstatus));
// 	}
// }
pid_t	exec_pipeline(t_node *node)
{
	extern char		**environ;
	char			*path;
	pid_t			pid;
	char			**argv;

	if (node == NULL)
		return (-1);
	prepare_pipe(node);
	pid = fork();
	if (pid < 0)
		fatal_error("fork");
	else if (pid == 0)
	{
		prepare_pipe_child(node);
		do_redirect(node->cmd->redirects);
		argv = token_to_argv(node->cmd->args);
		path = argv[0];
		if (ft_strchr(path, '/') == NULL)
			path = find_path(path);
		validate_access(path, argv[0]);
		execve(path, argv, environ);
		reset_redirect(node->cmd->redirects);
		fatal_error("execve");
	}
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
			status = WEXITSTATUS(wstatus);
		else if (wait_result < 0)
		{
			if (errno == ECHILD)
				break ;
		}
	}
	return (status);
}

int	exec(t_node *node)
{
	pid_t	last_pid;
	int		status;

	if (open_redir_file(node) < 0)
		return (ERROR_OPEN_REDIR);
	last_pid = exec_pipeline(node);
	status = wait_pipeline(last_pid);
	return (status);
}

// int	exec(t_node *node)
// {
// 	int	status;

// 	if (open_redir_file(node->redirects) < 0)
// 		return (ERROR_OPEN_REDIR);
// 	open_redir_file(node->redirects);
// 	do_redirect(node->redirects);
// 	status = exec_cmd(node);
// 	reset_redirect(node->redirects);
// 	return (status);
// }

// int	exec(char *argv[])
// {
// 	extern char		**environ;
// 	const char		*path = argv[0];
// 	pid_t			pid;
// 	int				wstatus;

// 	pid = fork();
// 	if (pid < 0)
// 		fatal_error("fork");
// 	else if (pid == 0)
// 	{
// 		if (ft_strchr(path, '/') == NULL)
// 			path = find_path(path);
// 		validate_access(path, argv[0]);
// 		execve(path, argv, environ);
// 		fatal_error("execve");
// 	}
// 	else
// 	{
// 		wait(&wstatus);
// 		return (WEXITSTATUS(wstatus));
// 	}
// }

/*
	@param
	*line -> Get the current line
	*state_loca -> State location
*/
// void	interpreter(char *line, int *state_loca)
// {
// 	t_token	*token;
// 	char	**argv;
// 	t_node	*node;
// 	bool	syntax_error;

// 	syntax_error = false;
// 	token = tokenizer(line);
// 	if (token->kind == TK_EOF)
// 		;
// 	else if (syntax_error)
// 		*state_loca = ERROR_TOKENIZE;
// 	else
// 	{
// 		node = parse(token);
// 		expand(node);
// 		argv = token_to_argv(node->args);
// 		*state_loca = exec(argv);
// 		free_argv(argv);
// 		free_node(node);
// 	}
// 	free_token(token);
// }
void	interpreter(char *line, int *state_loca)
{
	t_token	*token;
	t_node	*node;
	bool	syntax_error;

	syntax_error = false;
	token = tokenizer(line);
	if (at_eof(token))
		;
	else if (syntax_error)
		*state_loca = ERROR_TOKENIZE;
	else
	{
		node = parse(token);
		if (syntax_error)
			*state_loca = ERROR_PARSE;
		else
		{
			expand(node);
			*state_loca = exec(node);
		}
		free_node(node);
	}
	free_token(token);
}

int	main(void)
{
	char	*line;
	int		status;

	status = 0;
	rl_outstream = stderr;
	while (1)
	{
		line = readline("minishell$ ");
		if (line == NULL)
			break ;
		if (*line)
			add_history(line);
		interpreter(line, &status);
		free(line);
	}
	exit(status);
}
