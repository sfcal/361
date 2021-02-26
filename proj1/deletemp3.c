#include "mp3.h"

extern node_t *head;

void delete(char * name){

node_t *temp = head;

while(temp!=NULL){ //we always need to traverse full list

if(temp->name == name){ //if head deleted
	head = temp->next;
	free(temp);
	continue;
}

while(temp->name != name && temp!=NULL){ // this goes to the next mp3
    temp->prev=temp;
    temp=temp->next;
}
if(temp->name == name){
    temp=temp->next;
    free(temp);
    continue;
}
return;
printf("temp name is %s",temp->name); // debug

}
}
