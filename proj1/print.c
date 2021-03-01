#include "mp3.h"

extern node_t *head;

void print(bool order)
{
  node_t *temp;
  int  i = 0;

  temp = head;
  
  if(order){
    while (temp != NULL) {
      printf("(%d)--%s--%s--%d--\n", ++i, temp->name, temp->title, temp->data);
      temp = temp->next;
    }
  }
  else{
    while (temp->next != NULL) { // find end of list
      temp = temp->next;
      i++;
    }
    while (temp != NULL) {
      printf("(%d)--%s--%s--%d--\n", --i, temp->name, temp->title, temp->data);
      temp = temp->prev;
    }
  }
}
    
