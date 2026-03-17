#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 300
#define STACK_SIZE 100
#define QSIZE 100
#define TABLE_SIZE 101
#define FILE_NAME "/Users/melvinjusuf/Documents/AOL DS/dictionary.txt"

typedef struct {
    char indo[50];
    char eng[50];
} Dictionary;

Dictionary dictArray[MAX];
int count = 0;

typedef struct Node {
    Dictionary data;
    struct Node* next;
    struct Node* prev;
} Node;

Node* head = NULL;

void insertDCLL(Dictionary d) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = d;

    if(head == NULL) {
        head = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else {
        Node* tail = head->prev;

        tail->next = newNode;
        newNode->prev = tail;

        newNode->next = head;
        head->prev = newNode;
    }
}