/*
 * @Author: ThearchyHelios work@thearchyhelios.com
 * @Date: 2025-02-13 08:17:24
 * @LastEditors: ThearchyHelios work@thearchyhelios.com
 * @LastEditTime: 2025-02-20 11:39:23
 * @FilePath: /APNEE1/src/shell.c
 * @Description:
 */
/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <string.h>

#define BUFSIZE 1024

void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main()
{
	struct cmdline *l;
	signal(SIGCHLD, sigchld_handler);
	while (1)
	{
		int i, j;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l)
		{
			printf("exit\n");
			exit(0);
		}

		if (l->err)
		{
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in)
			printf("in: %s\n", l->in);
		if (l->out)
			printf("out: %s\n", l->out);

		// Compter les commandes
		int nb_cmd = 0;
		while (l->seq[nb_cmd] != 0)
		{
			nb_cmd++;
		}
		if (nb_cmd == 0)
		{
			continue;
		}

		int background = 0;

		int pipefd[nb_cmd - 1][2];
		for (i = 0; i < nb_cmd - 1; i++)
		{
			if (pipe(pipefd[i]) == -1)
			{
				perror("pipe");
				exit(1);
			}
		}

		pid_t pids[nb_cmd];

		int lastChar = 0;
		while (l->seq[nb_cmd - 1][lastChar] != 0)
		{
			lastChar++;
		}
		if (strcmp(l->seq[nb_cmd - 1][lastChar - 1], "&") == 0)
		{
			background = 1;
			l->seq[nb_cmd - 1][lastChar - 1] = NULL;
		}

		/* Display each command of the pipe */
		for (i = 0; l->seq[i] != 0; i++)
		{
			const char **cmd = l->seq[i];
			printf("seq[%d]: ", i);

			// determiner si le processus est en arriere-plan ou non
			int last = 0;
			while (cmd[last] != 0)
			{
				last++;
			}

			if (last > 0 && strcmp(cmd[last - 1], "&") == 0)
			{
				if (i != nb_cmd - 1) // si le symbole & est dans une commande autre que la derniere
				{
					printf("Error: & must be at the end of the command\n");
					break;
				}
			}

			// determiner si la commande est "quit"
			if (strcmp(cmd[0], "quit") == 0)
			{
				printf(" je vais quitter");
				exit(0);
			}

			pids[i] = Fork();

			switch (pids[i])
			{
			case -1:
				unix_error("Fork error");
				exit(1);
				break;
			case 0:
				if (i > 0) // detecter si il est pas le premier commande
				{
					int in_pipe = pipefd[i - 1][0];
					Dup2(in_pipe, STDIN_FILENO);
					Close(pipefd[i - 1][0]);
				}
				else if (l->in) // redirection de l'entree standard
				{
					int fd = open(l->in, O_RDONLY, 0644);
					if (fd < 0)
					{
						perror("Error opening input file");
						exit(1);
					}
					Dup2(fd, STDIN_FILENO);
					close(fd);
				}
				if (i < nb_cmd - 1) // detecter si il est pas le dernier commande
				{
					int out_pipe = pipefd[i][1];
					Dup2(out_pipe, STDOUT_FILENO);
					Close(pipefd[i][1]);
				}
				else if (l->out) // redirection de la sortie standard
				{
					int fd = open(l->out, O_WRONLY | O_CREAT, 0644);
					if (fd < 0)
					{
						perror("Error opening output file");
						exit(1);
					}
					Dup2(fd, STDOUT_FILENO);
					close(fd);
				}
				// Fermer tous les pipes en cas de mauvaise gestion
				for (j = 0; j < nb_cmd - 1; j++)
				{
					close(pipefd[j][0]);
					close(pipefd[j][1]);
				}

				if (execvp(cmd[0], cmd) == -1)
				{
					perror("execvp error");
					exit(1);
				}
				break;
			default:
				// Si ce n'est pas la premiere commande, on ferme le read de derniere pipe 
				if (i > 0)
				{
					close(pipefd[i - 1][0]);
				}
				// Si ce n'est pas la derniere commande, on ferme le read de derniere pipe 
				if (i < nb_cmd - 1)
				{
					close(pipefd[i][1]);
				}
				if (background)
				{
					printf("[%d] %s running in background\n", pids[i], cmd[0]);
				}
				break;
			}
			for (j = 0; cmd[j] != 0; j++)
			{
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}
		for (i = 0; i < nb_cmd; i++)
		{
			if (!background)
			{ //executer le wait lorsqu'il n'y a pas d'indication de &
				int status;
				waitpid(pids[i], &status, 0);
				if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
				{
					printf("Command failed with status %d\n", WEXITSTATUS(status));
				}
			}
		}
	}
}
