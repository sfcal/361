#include "mp3.h"

extern node_t *head;

void print()
{
  node_t *temp;
  int  i = 0;

  temp = head;

  while (temp != NULL) {
    printf("(%d)--%s--%s--%d--\n", ++i, temp->name, temp->title, temp->data);
    temp = temp->next;
  }
}
