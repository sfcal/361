#include "mp3.h"

extern node_t *head;

void delete(char * name){

node_t *temp;

temp=head;

while(temp->name != name && temp->next!=NULL){ // this goes to the next mp3
    temp=temp->next;
}
printf("temp name is %s",temp->name); // debug

}
