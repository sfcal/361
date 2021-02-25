#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct node
{
  char *name;    
  int   data;
  struct node *next;
  struct node *prev;
} node_t; 
