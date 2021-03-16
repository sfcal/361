#include "get_path.h"

char * where(char *command, struct pathelement *p)
{
  char cmd[64], *ch;
  int found=0;
  while (p) {         // WHERE
        sprintf(cmd, "%s/%s", p->element, command);
        if (access(cmd, X_OK) == 0){
            found=1;
            printf("[%s]\n", cmd);
        }
        p = p->next;
    }
    if (found) {
        ch = malloc(strlen(cmd)+1);
        strcpy(ch, cmd);
        return ch;
    }
    else {
        return (char *) NULL;
    }
}
