/*
 * @Author: ThearchyHelios work@thearchyhelios.com
 * @Date: 2025-02-13 08:17:24
 * @LastEditors: ThearchyHelios work@thearchyhelios.com
 * @LastEditTime: 2025-02-13 11:15:48
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

char *gerer_tube(char *buffin, char *buffout)
{
	int fd[2];
	pipe(fd);
	pid_t child_pid = Fork();
	int bytesin, bytesout;
	bytesin = strlen(buffin);
	if (child_pid != 0)
	{ /* pere */
		Close(fd[0]);
		bytesout = write(fd[1], buffout, strlen(buffout) + 1);
		printf("[%d]: write %d bytes, send %s to my child\n",
					 getpid(), bytesout, buffout);
	}
	else
	{ /* fils */
		Close(fd[1]);
		bytesin = read(fd[0], buffin, BUFSIZE);
		printf("[%d](fils): read %d bytes, my bufin is {%s} \n",
					 getpid(), bytesin, buffin);
	}
	return buffout;
}

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

			// gerer tube
				if (l->seq[i + 1] != 0)
				{
					char buffin[BUFSIZE];
					char buffout[BUFSIZE];
					strcpy(buffin, l->seq[i]);
					strcpy(buffout, l->seq[i+1]);
					gerer_tube(buffin, buffout);
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
				if (l->in) // redirection de l'entree standard
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

				if (l->out) // redirection de la sortie standard
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
				if (code == -1)
				{
					perror("execvp error ");
					exit(-1);
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
