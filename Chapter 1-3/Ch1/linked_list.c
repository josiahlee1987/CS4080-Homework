#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Doubly-Linked List
typedef struct Node
{
  struct Node* prev;
  struct Node* next;
  char* string;
} Node;

//Insert
void insert(Node** list, Node* prev, const char* string) {
  Node* node = malloc(sizeof(Node));
  node->string = malloc(strlen(string) + 1);
  strcpy(node->string, string);
  if (prev == NULL) {
    if (*list != NULL) (*list)->prev = node;
    node->prev = NULL;
    node->next = *list;
    *list = node;
  } else {
    node->next = prev->next;
    if (node->next != NULL) node->next->prev = node;
    prev->next = node;
    node->prev = prev;
  }
}

// Search
Node* find(Node* list, const char* string) {
  while (list != NULL) {
    if (strcmp(string, list->string) == 0) {
      return list;
    }
    list = list->next;
  }
  return NULL;
}

// Delete
void delete(Node** list, Node* node) {
  if (node->prev != NULL) node->prev->next = node->next;
  if (node->next != NULL) node->next->prev = node->prev;

  // update head node to next if it is being deleted
  if (*list == node) *list = node->next;
  
  free(node->string);
  free(node);
}

// Testing
void dump(Node* list) {
  printf("Order: ");
  while (list != NULL) {
    printf("%s [%p] -> ", list->string, list);
    list = list->next;
  }
  printf("\n\n");
}

int main() {
  Node* list = NULL;
  insert(&list, NULL, "4th");
  insert(&list, NULL, "1st");
  insert(&list, find(list, "1st"), "2nd");
  insert(&list, find(list, "2nd"), "3rd");
  insert(&list, NULL, "actual 1st");
  insert(&list, find(list, "actual 1st"), "actual 2nd");
  
  dump(list);

  printf("DELETE extra nodes\n");
  delete(&list, find(list, "actual 1st"));
  delete(&list, find(list, "actual 2nd"));
  dump(list);

  printf("DELETE 3rd\n");
  delete(&list, find(list, "3rd"));
  dump(list);

  printf("DELETE 1st\n");
  delete(&list, find(list, "1st"));
  dump(list);

  return 0;
}