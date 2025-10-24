/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nifromon <nifromon@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 11:01:06 by nifromon          #+#    #+#             */
/*   Updated: 2025/10/24 12:25:43 by nifromon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int	err(char *str)
{
	while (*str)
		write(2, str++, 1);
	return (1);
}

int	cd(char **argv, int i)
{
	if (i != 2)
		return (err("error: cd: bad arguments\n"));
	if (chdir(argv[1]) == -1)
		return (err("error: cd: cannot change directory to "), err(argv[1]), err("\n"));
	return (0);
}

int	exec(char **argv, char **env, int i)
{
	int	fd[2];
	int	pid;
	int	status;
	int	has_pipe = argv[i] && !strcmp(argv[i], "|");

	if (!has_pipe && !strcmp(*argv, "cd"))
		return (cd(argv, i));
	if (has_pipe && pipe(fd) == -1)
		return (err("error: fatal\n"));
	pid = fork();
	if (!pid)
	{
		argv[i] = 0;
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return (err("error: fatal\n"));
		if (!strcmp(*argv, "cd"))
			return (cd(argv, i));
		execve(*argv, argv, env);
		return (err("error: cannot execute "), err(*argv), err("\n"));
	}
	waitpid(pid, &status, 0);
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (err("error: fatal\n"));
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int	main(int argc, char **argv, char **env)
{
	int	i = 0;
	int	status = 0;

	if (argc > 1)
	{
		while (argv[i] && argv[++i])
		{
			argv += i;
			i = 0;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;
			if (i)
				status = exec(argv, env, i);
		}
	}
	return (status);
}