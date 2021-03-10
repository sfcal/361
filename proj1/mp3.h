#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
 
typedef struct node
{
  char *name;    
  char *title;    
  int   data;
  struct node *next;
  struct node *prev;
} node_t; 
