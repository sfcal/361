#include "get_path.h"

char * where(char *command, struct pathelement *p)
{
  char cmd[6000], *ch;
  int  found;

  found = 0;

  while (p) {
      //strncat(cmd, p->element, strlen(p->element));      
      
    //sprintf(cmd, "%s/%s", p->element, command);
    // printf("--- %s\n",p->element); 
    if (access(cmd, X_OK) == 0) {
      found = 1;
      strncat(cmd, p->element, strlen(p->element)); 
      p=p->next;
      continue;
    }
    p = p->next;
  }
  if (found) {
    ch = malloc(strlen(cmd)+1);
    strcpy(ch, cmd);
    return ch;
  }
  else
    return (char *) NULL;
}
