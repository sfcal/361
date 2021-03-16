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
//	switch(){
//
//	}

}

int main(int argc, char **argv, char **envp) {
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
    while ((eofStatus = fgets(buf, MAXLINE, stdin)) != NULL) {
        if (strlen(buf) == 1 && buf[strlen(buf) - 1] == '\n')
            goto nextprompt; // "empty" command line

        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */
        // parse command line into tokens (stored in buf)
        arg_no = 0;
        pch = strtok(buf, " ");
        while (pch != NULL && arg_no < MAXARGS) {
            arg[arg_no] = pch;
            arg_no++;
            pch = strtok(NULL, " ");
        }
        arg[arg_no] = (char *) NULL;

        if (arg[0] == NULL) // "blank" command line
            goto nextprompt;

        /* print tokens
        for (i = 0; i < arg_no; i++)
          printf("arg[%d] = %s\n", i, arg[i]);
                */

        if (strcmp(arg[0], "exit") == 0) {
            printf("Executing built-in [exit]\n");
            exit(0);
        }
        else if (strcmp(arg[0], "pwd") == 0) { // built-in command pwd
            printf("Executing built-in [pwd]\n");
            ptr = getcwd(NULL, 0);
            printf("%s\n", ptr);
            free(ptr);
        }
        else if (strcmp(arg[0], "where") == 0) // TODO
        {
            struct pathelement *p, *tmp;
            char *cmd;

            printf("Executing built-in [where]\n");

            if (arg[1] == NULL) { // "empty" which
                printf("where: Too few arguments.\n");
                goto nextprompt;
            }

            p = get_path();
            /***/
            tmp = p;
            while (tmp) { // print list of paths
                printf("path [%s]\n", tmp->element);
                tmp = tmp->next;
            }
            /***/

            cmd = where(arg[1], p);
            if (cmd) {
                printf("%s\n", cmd);
                free(cmd);
            } else // argument not found
            {
                printf("%s: Command not found\n", arg[1]);
                fflush(stdout);
            }

            while (p) { // free list of path values
                tmp = p;
                p = p->next;
                free(tmp->element);
                free(tmp);
            }
        }
        else if (strcmp(arg[0], "which") == 0) { // built-in command which
            struct pathelement *p, *tmp;
            char *cmd;

            printf("Executing built-in [which]\n");

            if (arg[1] == NULL) { // "empty" which
                printf("which: Too few arguments.\n");
                goto nextprompt;
            }

            p = get_path();
            /***/
            tmp = p;
            while (tmp) { // print list of paths
                printf("path [%s]\n", tmp->element);
                tmp = tmp->next;
            }
            /***/

            cmd = which(arg[1], p);
            if (cmd) {
                printf("%s\n", cmd);
                free(cmd);
            } else // argument not found
                printf("%s: Command not found\n", arg[1]);

            while (p) { // free list of path values
                tmp = p;
                p = p->next;
                free(tmp->element);
                free(tmp);
            }
        }
        else if (strcmp(arg[0], "list") == 0) { // command to list files full working
            printf("Executing built-in [list]\n");
            DIR *directory;
            struct dirent *file;
            //directory = opendir(getcwd(NULL, 0));
            if (arg[1] == NULL) {
                directory = opendir(getcwd(NULL, 0));
                if (directory != NULL) {
                    while (file = readdir(directory)) {
                        printf("%s\n", file->d_name);
                    }
                    printf("\n");
                    //free(directory);
                    //free(file);
                    closedir(directory);
                }
            } else {
                int i = 1;
                while (arg[i] != NULL) {
                    directory = opendir(arg[i]);
                    if (directory != NULL) {
                        printf("%s:\n", arg[i]);
                        while (file = readdir(directory)) {
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
        else if (strcmp(arg[0], "cd") == 0) {
            int suc = 0;
            printf("Executing built-in [cd]\n");

            if (arg[1] == NULL) {
                tmpDir = getcwd(NULL, 0);
                suc = chdir(getenv("HOME"));

            } else if (strcmp(arg[1], "-") == 0) {
                if (tmpDir == NULL) {
                    printf("No previous directory");
                    fflush(stdout);
                } else {
                    suc = chdir(tmpDir);
                }

            } else if (arg[1] != NULL && arg[2] == NULL) {
                tmpDir = getcwd(NULL, 0);
                suc = chdir(arg[1]);
            } else {
                printf("Too many arguements");
            }
            // check if success found dir
            if(suc != 0){
                printf("Path [%s] not found\n", arg[1]);
            }
        }
        else if (strcmp(arg[0], "pid") == 0) {
            printf("Executing built-in [pid]\n");
            printf("shell pid: %u\n", getpid());
        }
        else if (strcmp(arg[0], "kill") == 0) { // need to finish
            printf("Executing built-in [kill]\n");
            printf("shell pid: %u\n", getpid());
            char temp[3];
            i = 0;
            if(arg[1]!=NULL && arg[1][0] == '-' && arg[2]!=NULL){

                while (arg[1][i] != '\0') {
                    temp[i] = arg[1][i+1];
                    i++;
                }
                temp[i]='\0';

                kill(atoi(arg[2]), atoi(temp));
            }

            else if(arg[1]!=NULL && arg[2]==NULL){
                kill(atoi(arg[1]), 15);
            }
        } //TODO
        else if (strcmp(arg[0], "prompt") == 0){
            printf("Executing built-in [prompt]\n");
            if (arg[1] != NULL) {
                strcpy(prompt, arg[1]);
            } else {
                printf("input prompt prefix: ");
                fgets(prompt, 64, stdin);
                prompt[strcspn(prompt, "\n")] = 0;
            }
        }
        else if (strcmp(arg[0], "printenv") == 0) {
            printf("Executing built-in [printenv]\n");
            int len = 0;
            if (arg[1] == NULL) {
                while (__environ[len] != NULL) {
                    printf("%s\n", __environ[len]);
                    len++;
                }
            } else if (arg[1] != NULL && arg[2] == NULL) {
                if (getenv(arg[1]) == NULL) {
                    continue;
                } else {
                    printf("%s\n", getenv(arg[1]));
                }
            } else {
                fprintf(stderr, "\nprintenv: Too many arguments.\n");
            }
        }
        else if (strcmp(arg[0], "setenv") == 0) {

            printf("Executing built-in [setenv]\n");
            if (arg[1] == NULL) // if no argument was given
            {
                i = 0;
                while (__environ[i] != NULL) {
                    printf("%s\n", __environ[i]);
                    i += 1;
                }
            } else if (arg[1] != NULL && arg[2] == NULL) // one argument
            {
                setenv(arg[1], "", 1);

            } else if (arg[1] != NULL && arg[2] != NULL) // two arguments
                setenv(arg[1], arg[2], 1);

            else {
                printf("setenv: Too many arguments\n");
            }

        }
        else { // external command
            if ((pid = fork()) < 0) {
                printf("fork error");
            } else if (pid == 0) { /* child */
                // an array of aguments for execve()
                //printf("here1\n");
                fflush(stdout);
                char *execargs[MAXARGS];
                glob_t paths;
                struct pathelement *path;
                int csource, j = 0;
                char **p;
                char *temp;
                path = get_path();
                execargs[j] = malloc(strlen(arg[0]) + 1);

                //if(which(arg[0], path)==NULL)
                //   printf("%s: command not found\n", arg[0]);
                //  fflush(stdout);

                //printf("1.5");
                //fflush(stdout);
                switch (arg[0][0]) {
                    case '/' :
                        strcpy(execargs[0], arg[0]);
                        break;

                    case '.' : //2 dots
                        if (arg[0][1] == '.') {
                            arg[0] += 2;
                            strcpy(execargs[0], strcat(dirname(getcwd(NULL, 0)), arg[0]));
                            break;
                        } else {
                            arg[0]++;
                            strcpy(execargs[0], strcat(getcwd(NULL, 0), arg[0]));
                            break;
                        }
                    default :
                        // printf("1.75");
                        //fflush(stdout);
                        strcpy(execargs[0], which(arg[0], path));
                        break;
                }

                j = 1;
                //printf("here2");
                //fflush(stdout);
                for (i = 1; i < arg_no; i++) // check arguments
                    if (strchr(arg[i], '*') != NULL) { // wildcard!
                        csource = glob(arg[i], 0, NULL, &paths);
                        if (csource == 0) {
                            for (p = paths.gl_pathv; *p != NULL; ++p) {
                                execargs[j] = malloc(strlen(*p) + 1);
                                strcpy(execargs[j], *p);
                                j++;
                            }

                            globfree(&paths);
                        }
                    } else if (arg[i][0] == '-')
                        execargs[j++] = arg[i];

                    else if (arg[i] != NULL)
                        execargs[j++] = arg[i];

                execargs[j] = NULL;
                //printf("here3");
                //fflush(stdout);
                i = 0;
                for (i = 0; i < j; i++)
                    printf("exec arg [%s]\n", execargs[i]);

                execve(execargs[0], execargs, NULL);
                printf("%s: Command not found\n", buf);
                fflush(stdout);
                exit(127);
            }

            /* parent */
            if ((pid = waitpid(pid, &status, 0)) < 0) {
                printf("waitpid error");
            }

            if (WIFEXITED(status) && WIFEXITED(status) == 0) {
                printf("child terminates with (%d)\n", WEXITSTATUS(status));
            }
        }

        nextprompt:
            fprintf(stdout, "%s ", prompt);
            fflush(stdout);
    }
    if(eofStatus == NULL){
        printf("\nType exit instead\n");
        fflush(stdout);
        clearerr(stdin);
        goto nextprompt;
    }
        exit(0);
}