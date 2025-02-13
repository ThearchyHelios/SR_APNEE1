/*
 * @Author: ThearchyHelios work@thearchyhelios.com
 * @Date: 2025-02-13 08:17:24
 * @LastEditors: ThearchyHelios work@thearchyhelios.com
 * @LastEditTime: 2025-02-13 11:03:34
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

int main()
{

	struct cmdline *l;
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

		/* Display each command of the pipe */
		for (i = 0; l->seq[i] != 0; i++)
		{
			const char **cmd = l->seq[i];
			printf("seq[%d]: ", i);

			if (strcmp(cmd[0], "quit") == 0)
			{
				printf(" je vais quitter");
				exit(0);
			}

			pid_t pid;
			pid = Fork();

			switch (pid)
			{
			case -1:
				unix_error("Fork error");
				exit(1);
				break;
			case 0:
				if (l->in)   //redirection de l'entree standard
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

				if (l->out) //redirection de la sortie standard
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
				int code = execvp(cmd[0], cmd);
				if (code == -1){
					perror("execvp error ");
					exit(-1);
				}
				// printf("code: %d", code);
				exit(0);
				break;
			default:
				waitpid(pid, NULL, 0);
				if (pid < 0)
					unix_error("waitpid error");
				break;
			}
			for (j = 0; cmd[j] != 0; j++)
			{
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}
	}
}
