#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <glob.h>
#include <sys/wait.h>
#include "sh.h"

void sig_handler(int sig)
{
	fprintf(stdout, "\n>> ");
	fflush(stdout);
}

int main(int argc, char **argv, char **envp)
{
	char buf[MAXLINE];
	char *arg[MAXARGS]; // an array of tokens
	char *ptr;
	char *pch;
	pid_t pid;
	int status, i, arg_no;

	signal(SIGINT, sig_handler);

	fprintf(stdout, ">> "); /* print prompt (printf requires %% to print %) */
	fflush(stdout);
	while (fgets(buf, MAXLINE, stdin) != NULL)
	{
		if (strlen(buf) == 1 && buf[strlen(buf) - 1] == '\n')
			goto nextprompt; // "empty" command line

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */
		// parse command line into tokens (stored in buf)
		arg_no = 0;
		pch = strtok(buf, " ");
		while (pch != NULL && arg_no < MAXARGS)
		{
			arg[arg_no] = pch;
			arg_no++;
			pch = strtok(NULL, " ");
		}
		arg[arg_no] = (char *)NULL;

		if (arg[0] == NULL) // "blank" command line
			goto nextprompt;

		/* print tokens
		for (i = 0; i < arg_no; i++)
		  printf("arg[%d] = %s\n", i, arg[i]);
                */

		if (strcmp(arg[0], "exit") == 0)
		{
			printf("Executing built-in [exit]\n");
			exit(0);
		}
		else if (strcmp(arg[0], "pwd") == 0)
		{ // built-in command pwd
			printf("Executing built-in [pwd]\n");
			ptr = getcwd(NULL, 0);
			printf("%s\n", ptr);
			free(ptr);
		}
		else if (strcmp(arg[0], "where") == 0) // need to finish
		{
			struct pathelement *p, *tmp;
			char *cmd;

			printf("Executing built-in [where]\n");

			if (arg[1] == NULL)
			{ // "empty" which
				printf("which: Too few arguments.\n");
				goto nextprompt;
			}

			p = get_path();
			/***/
			tmp = p;
			while (tmp)
			{ // print list of paths
				printf("path [%s]\n", tmp->element);
				tmp = tmp->next;
			}
			/***/

			cmd = where(arg[1], p);
			if (cmd)
			{
				printf("%s\n", cmd);
				free(cmd);
			}
			else // argument not found
				printf("%s: Command not found\n", arg[1]);

			while (p)
			{ // free list of path values
				tmp = p;
				p = p->next;
				free(tmp->element);
				free(tmp);
			}
		}
		else if (strcmp(arg[0], "which") == 0)
		{ // built-in command which
			struct pathelement *p, *tmp;
			char *cmd;

			printf("Executing built-in [which]\n");

			if (arg[1] == NULL)
			{ // "empty" which
				printf("which: Too few arguments.\n");
				goto nextprompt;
			}

			p = get_path();
			/***/
			tmp = p;
			while (tmp)
			{ // print list of paths
				printf("path [%s]\n", tmp->element);
				tmp = tmp->next;
			}
			/***/

			cmd = which(arg[1], p);
			if (cmd)
			{
				printf("%s\n", cmd);
				free(cmd);
			}
			else // argument not found
				printf("%s: Command not found\n", arg[1]);

			while (p)
			{ // free list of path values
				tmp = p;
				p = p->next;
				free(tmp->element);
				free(tmp);
			}
		}
		else if (strcmp(arg[0], "list") == 0)
		{ // command to list files (might need to double check this)
			printf("Executing built-in [list]\n");
			DIR *directory;
			struct dirent *file;
			//directory = opendir(getcwd(NULL, 0));
			if (arg[1] == NULL)
			{
				directory = opendir(getcwd(NULL, 0));
				if (directory != NULL)
				{
					while (file = readdir(directory))
					{
						printf("%s\n", file->d_name);
					}
					printf("\n");
					closedir(directory);
				}
			}
			else
			{
				int i = 1;
				while (arg[i] != NULL)
				{
					directory = opendir(arg[i]);
					if (directory != NULL)
					{
						printf("%s:\n",arg[i]);
						while (file = readdir(directory))
						{
							printf("%s\n", file->d_name);
						}
						printf("\n");
						closedir(directory);
					}
					i+=1;
				}
			}
		}else if (strcmp(arg[0], "pid") == 0){


		}
		else if (strcmp(arg[0], "cd") == 0)
		{
			printf("Executing built-in [cd]\n");
			chdir(arg[1]);
		}
		else
		{ // external command
			if ((pid = fork()) < 0)
			{
				printf("fork error");
			}
			else if (pid == 0)
			{ /* child */
				// an array of aguments for execve()
				char *execargs[MAXARGS];
				glob_t paths;
				int csource, j;
				char **p;

				execargs[j] = malloc(strlen(arg[0]) + 1);
				strcpy(execargs[0], arg[0]); // copy command
				j = 1;
				for (i = 1; i < arg_no; i++) // check arguments
					if (strchr(arg[i], '*') != NULL)
					{ // wildcard!
						csource = glob(arg[i], 0, NULL, &paths);
						if (csource == 0)
						{
							for (p = paths.gl_pathv; *p != NULL; ++p)
							{
								execargs[j] = malloc(strlen(*p) + 1);
								strcpy(execargs[j], *p);
								j++;
							}

							globfree(&paths);
						}
					}
				execargs[j] = NULL;

				i = 0;
				for (i = 0; i < j; i++)
					printf("exec arg [%s]\n", execargs[i]);

				execve(execargs[0], execargs, NULL);
				printf("couldn't execute: %s", buf);
				exit(127);
			}

			/* parent */
			if ((pid = waitpid(pid, &status, 0)) < 0)
				printf("waitpid error");
			/**
                  if (WIFEXITED(status)) S&R p. 239 
                    printf("child terminates with (%d)\n", WEXITSTATUS(status));
**/
		}

	nextprompt:
		fprintf(stdout, ">> ");
		fflush(stdout);
	}
	exit(0);
}
