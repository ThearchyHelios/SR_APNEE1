/*
 * @Author: ThearchyHelios work@thearchyhelios.com
 * @Date: 2025-02-13 08:17:24
 * @LastEditors: ThearchyHelios work@thearchyhelios.com
 * @LastEditTime: 2025-02-13 09:48:58
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
				execvp(cmd[0], cmd);
				if (l->out)
				{
					printf("out: %s\n", l->out);
					int fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}
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
