#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <glob.h>
#include <sys/wait.h>
#include <libgen.h>
#include "sh.h"

char prompt[64] = {'>', '>'};
void sig_handler(int sig)
{
    fprintf(stdout, "\n%s ", prompt);
    fflush(stdout);
}

int main(int argc, char **argv, char **envp)
{
    char buf[MAXLINE];
    char *arg[MAXARGS]; // an array of tokens
    char *ptr;
    char *tmpDir;
    char *pch;
    pid_t pid;
    int status, i, arg_no;
    char *eofStatus;

    signal(SIGINT, sig_handler);
    signal(SIGTSTP, SIG_IGN); // for CTRL-Z
    signal(SIGTERM, SIG_IGN);

    fprintf(stdout, "%s ", prompt); /* print prompt (printf requires %% to print %) */
    fflush(stdout);
    while ((eofStatus = fgets(buf, MAXLINE, stdin)) != NULL)
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

        if (strcmp(arg[0], "exit") == 0) // built in exit command
        {
            printf("Executing built-in [exit]\n");
            exit(0);
        }
        else if (strcmp(arg[0], "pwd") == 0) // built in pwd command to get the current working directory
        { // built-in command pwd
            printf("Executing built-in [pwd]\n");
            ptr = getcwd(NULL, 0);
            printf("%s\n", ptr);
            free(ptr);
        }
        else if (strcmp(arg[0], "where") == 0) // build in where command
        {
            struct pathelement *p, *tmp;
            char *cmd;

            printf("Executing built-in [where]\n");

            if (arg[1] == NULL)
            { // "empty" where
                printf("where: Too few arguments.\n");
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
            if (cmd) // prints command
            {
                printf("%s\n", cmd);
                free(cmd);
            }
            else // argument not found
            {
                printf("%s: Command not found\n", arg[1]);
                fflush(stdout);
            }

            while (p)
            { // free list of path values
                tmp = p;
                p = p->next;
                free(tmp->element);
                free(tmp);
            }
        }
        else if (strcmp(arg[0], "which") == 0) // built in which command
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
        { // command to list files full working
            printf("Executing built-in [list]\n");
            DIR *directory;
            struct dirent *file;
            if (arg[1] == NULL) // if no arguments are given
            {
                directory = opendir(getcwd(NULL, 0));
                if (directory != NULL) // if a valid directory is found
                {
                    while (file = readdir(directory))
                    {
                        printf("%s\n", file->d_name);
                    }
                    printf("\n");
                    //free(directory);
                    //free(file);
                    closedir(directory);
                }
            }
            else // if a directory is given as an argument
            {
                int i = 1;
                while (arg[i] != NULL)
                {
                    directory = opendir(arg[i]);
                    if (directory != NULL) // if a valid directory is found
                    {
                        printf("%s:\n", arg[i]);
                        while (file = readdir(directory))
                        {
                            printf("%s\n", file->d_name);
                        }
                        printf("\n");
                        //free(directory);
                        //free(file);
                        closedir(directory);
                    }
                    i += 1;
                }
            }
        }
        else if (strcmp(arg[0], "cd") == 0) // built in cd command to navigate through folders
        {
            int suc = 0;
            printf("Executing built-in [cd]\n");

            if (arg[1] == NULL) // if no argumnets are given
            {
                tmpDir = getcwd(NULL, 0);
                suc = chdir(getenv("HOME"));
            }
            else if (strcmp(arg[1], "-") == 0) // case  of "cd -" to take you back to previous directory
            {
                if (tmpDir == NULL)  // to check if tmpDir is set
                {
                    printf("No previous directory");
                    fflush(stdout);
                }
                else // set source to previous directory stored in tmpDir
                {
                    suc = chdir(tmpDir);
                }
            }
            else if (arg[1] != NULL && arg[2] == NULL) // case when given a folder to move into 
            {
                tmpDir = getcwd(NULL, 0);
                suc = chdir(arg[1]);
            }
            else // case for when you gove too many arguments
            {
                printf("Too many arguements");
            }
            // check if success found dir
            if (suc != 0) // if no valid folder is found
            {
                printf("Path [%s] not found\n", arg[1]);
            }
        }
        else if (strcmp(arg[0], "pid") == 0) // built in command to get all processes running in the shell
        {
            printf("Executing built-in [pid]\n");
            printf("shell pid: %u\n", getpid());
        }
        else if (strcmp(arg[0], "kill") == 0) // built in command to kill processes
        {
            printf("Executing built-in [kill]\n");
            printf("shell pid: %u\n", getpid());
            char temp[3];
            i = 0;
            if (arg[1] != NULL && arg[1][0] == '-' && arg[2] != NULL) // if passed a "-" in the arguments
            {

                while (arg[1][i] != '\0')
                {
                    temp[i] = arg[1][i + 1];
                    i++;
                }
                temp[i] = '\0';

                kill(atoi(arg[2]), atoi(temp));
            }

            else if (arg[1] != NULL && arg[2] == NULL) // kill a specific process in the firs argument
            {
                kill(atoi(arg[1]), 15);
            }
        } //TODO
        else if (strcmp(arg[0], "prompt") == 0) // built in command for prompt to change the prompt characters
        {
            printf("Executing built-in [prompt]\n");
            if (arg[1] != NULL) // if an argument is passed
            {
                strcpy(prompt, arg[1]);
            }
            else // if no argument is given
            {
                printf("input prompt prefix: ");
                fgets(prompt, 64, stdin);
                prompt[strcspn(prompt, "\n")] = 0;
            }
        }
        else if (strcmp(arg[0], "printenv") == 0) // built in command to print all path environment variables
        {
            printf("Executing built-in [printenv]\n");
            int len = 0;
            if (arg[1] == NULL) // if no argument is given
            {
                while (__environ[len] != NULL)
                {
                    printf("%s\n", __environ[len]);
                    len++;
                }
            }
            else if (arg[1] != NULL && arg[2] == NULL) // if one arguemnt is given
            {
                if (getenv(arg[1]) == NULL)
                {
                    continue;
                }
                else
                {
                    printf("%s\n", getenv(arg[1]));
                }
            }
            else // if too many arguments are given
            {
                fprintf(stderr, "\nprintenv: Too many arguments.\n");
            }
        }
        else if (strcmp(arg[0], "setenv") == 0) // built in command to set/change environment variables
        {

            printf("Executing built-in [setenv]\n");
            if (arg[1] == NULL) // if no argument was given
            {
                i = 0;
                while (__environ[i] != NULL)
                {
                    printf("%s\n", __environ[i]);
                    i += 1;
                }
            }
            else if (arg[1] != NULL && arg[2] == NULL) // one argument
            {
                setenv(arg[1], "", 1);
            }
            else if (arg[1] != NULL && arg[2] != NULL) // two arguments
                setenv(arg[1], arg[2], 1);

            else // too many arguments given
            {
                printf("setenv: Too many arguments\n");
            }
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
                fflush(stdout);
                char *execargs[MAXARGS];
                glob_t paths;
                struct pathelement *path;
                int csource, j = 0;
                char **p;
                char *temp;
                path = get_path();
                execargs[j] = malloc(strlen(arg[0]) + 1);


                switch (arg[0][0]) // switch statements to check if "/" is being pass or "."/".." is being passed
                {
                case '/':
                    strcpy(execargs[0], arg[0]);
                    break;

                case '.': //2 dots
                    if (arg[0][1] == '.') // check for the first dot
                    {
                        arg[0] += 2;
                        strcpy(execargs[0], strcat(dirname(getcwd(NULL, 0)), arg[0]));
                        break;
                    }
                    else // check for the second dot
                    {
                        arg[0]++;
                        strcpy(execargs[0], strcat(getcwd(NULL, 0), arg[0]));
                        break;
                    }
                default: // default case if absolute path isn't being passed
                    strcpy(execargs[0], which(arg[0], path));
                    break;
                }

                j = 1;
                for (i = 1; i < arg_no; i++) // check arguments for wildcards
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
                    else if (arg[i][0] == '-')
                        execargs[j++] = arg[i];

                    else if (arg[i] != NULL)
                        execargs[j++] = arg[i];

                execargs[j] = NULL;
                i = 0;
                for (i = 0; i < j; i++)
                    printf("exec arg [%s]\n", execargs[i]);

                execve(execargs[0], execargs, NULL);
                printf("%s: Command not found\n", buf);
                fflush(stdout);
                exit(127);
            }

            /* parent */
            if ((pid = waitpid(pid, &status, 0)) < 0)
            {
                printf("waitpid error");
            }

            if (WIFEXITED(status) && WIFEXITED(status) == 0)
            {
                printf("child terminates with (%d)\n", WEXITSTATUS(status));
            }
        }

    nextprompt:
        fprintf(stdout, "%s ", prompt);
        fflush(stdout);
    }
    if (eofStatus == NULL) // check if CTRL-D is pressed
    {
        printf("\nType exit instead\n");
        fflush(stdout);
        clearerr(stdin);
        goto nextprompt;
    }
    exit(0);
}