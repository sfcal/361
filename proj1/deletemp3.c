#include "mp3.h"

extern node_t *head;

void delete (char *Name)
{
    node_t *temp;
    if (head == NULL)
    {
        printf("\nlist is empty\n");
    }
    else
    {
        temp = head;
        while (temp != NULL)
        {

            if (strcmp(temp->name, Name) == 0)
            {
                if (temp == head && temp->next == NULL)
                { // if only mp3 in the list
                    int i = 0;

                    while (head != NULL)
                    {
                        temp = head;
                        head = head->next; // point to next MP3 record
                        free(temp->name);  // first free name inside MP3 record
                        free(temp);        // then free MP3 record
                        i++;
                    }
                }
                else if (temp == head && temp->next != NULL)
                { // if deleting first mp3 in the list
                    head = temp->next;
                    head->prev = NULL;
                    free(temp->name);
                    free(temp);
                }
                else if (temp->next != NULL) // case for if you try to delete not at the end of the list
                {
                    node_t *beforeMp3 = temp->prev; // get previous node
                    node_t *afterMp3 = temp->next;
                    beforeMp3->next = afterMp3;
                    afterMp3->prev = beforeMp3;
                    free(temp->name);
                    free(temp);
                }
                else // case if you delete a node at the end of the list
                {
                    node_t *beforeMp3 = temp->prev; // get previous node
                    beforeMp3->next = NULL;
                    free(temp->name);
                    free(temp);
                }
            }
            if (temp->next != NULL)
            {
                temp = temp->next;
            }
            else
            { // at the end of the list
                break;
            }
        }
    }
}
