#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 300
#define STACK_SIZE 100
#define QSIZE 100
#define TABLE_SIZE 101
#define FILE_NAME "/Users/melvinjusuf/Documents/AOL DS/dictionary.txt"

/* =====================================================
   1️⃣ ARRAY & POINTER (MAIN STORAGE)
===================================================== */

typedef struct {
    char indo[50];
    char eng[50];
} Dictionary;

Dictionary dictArray[MAX];
int count = 0;

/* =====================================================
   2️⃣ DOUBLY CIRCULAR LINKED LIST
===================================================== */

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

/* =====================================================
   3️⃣ STACK (UNDO)
===================================================== */

Dictionary stack[STACK_SIZE];
int top = -1;

void push(Dictionary d) {
    if(top < STACK_SIZE-1)
        stack[++top] = d;
}

Dictionary pop() {
    return stack[top--];
}

/* =====================================================
   4️⃣ QUEUE (SEARCH HISTORY)
===================================================== */

Dictionary queue[QSIZE];
int front = 0, rear = -1;

void enqueue(Dictionary d) {
    if(rear < QSIZE-1)
        queue[++rear] = d;
}

void showHistory() {
    printf("\nSearch History:\n");
    for(int i = front; i <= rear; i++)
        printf("%s -> %s\n", queue[i].indo, queue[i].eng);
}

/* =====================================================
   5️⃣ BINARY SEARCH TREE (SORTED)
===================================================== */

typedef struct TreeNode {
    Dictionary data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* root = NULL;

TreeNode* insertBST(TreeNode* node, Dictionary d) {
    if(node == NULL) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = d;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if(strcmp(d.indo, node->data.indo) < 0)
        node->left = insertBST(node->left, d);
    else
        node->right = insertBST(node->right, d);

    return node;
}

void inorder(TreeNode* node) {
    if(node != NULL) {
        inorder(node->left);
        printf("%s -> %s\n", node->data.indo, node->data.eng);
        inorder(node->right);
    }
}

/* =====================================================
   6️⃣ HASH TABLE (FAST SEARCH)
===================================================== */

typedef struct HashNode {
    Dictionary data;
    struct HashNode* next;
} HashNode;

HashNode* hashTable[TABLE_SIZE];

int hashFunction(char key[]) {
    int sum = 0;
    for(int i = 0; key[i] != '\0'; i++)
        sum += key[i];
    return sum % TABLE_SIZE;
}

void insertHash(Dictionary d) {
    int index = hashFunction(d.indo);

    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->data = d;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

HashNode* searchHash(char key[]) {
    int index = hashFunction(key);
    HashNode* temp = hashTable[index];

    while(temp != NULL) {
        if(strcmp(temp->data.indo, key) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

/* =====================================================
   FILE PROCESSING
===================================================== */

void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "w");
    if(fp == NULL) return;

    for(int i = 0; i < count; i++)
        fprintf(fp, "%s %s\n",
                dictArray[i].indo,
                dictArray[i].eng);

    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "r");
    if(fp == NULL) return;

    Dictionary d;

    while(fscanf(fp, "%s %s", d.indo, d.eng) != EOF) {
        dictArray[count++] = d;
        insertDCLL(d);
        insertHash(d);
        root = insertBST(root, d);
    }

    fclose(fp);
}

/* =====================================================
   CRUD OPERATIONS
===================================================== */

void addWord() {
    Dictionary d;

    printf("Indonesia: ");
    scanf("%s", d.indo);
    printf("English: ");
    scanf("%s", d.eng);

    dictArray[count++] = d;

    insertDCLL(d);
    insertHash(d);
    root = insertBST(root, d);

    saveToFile();

    printf("Word added.\n");
}

void searchWord() {
    char key[50];
    printf("Search word: ");
    scanf("%s", key);

    HashNode* result = searchHash(key);

    if(result != NULL) {
        printf("Found: %s -> %s\n",
               result->data.indo,
               result->data.eng);

        enqueue(result->data);
    } else {
        printf("Not found.\n");
    }
}

void updateWord() {
    char key[50];
    printf("Update word: ");
    scanf("%s", key);

    HashNode* result = searchHash(key);

    if(result != NULL) {
        push(result->data);

        printf("New meaning: ");
        scanf("%s", result->data.eng);

        for(int i = 0; i < count; i++)
            if(strcmp(dictArray[i].indo, key) == 0)
                strcpy(dictArray[i].eng, result->data.eng);

        saveToFile();
        printf("Updated.\n");
    }
}

void deleteWord() {
    char key[50];
    printf("Delete word: ");
    scanf("%s", key);

    for(int i = 0; i < count; i++) {
        if(strcmp(dictArray[i].indo, key) == 0) {

            push(dictArray[i]);

            for(int j = i; j < count-1; j++)
                dictArray[j] = dictArray[j+1];

            count--;
            saveToFile();
            printf("Deleted.\n");
            return;
        }
    }

    printf("Not found.\n");
}

void undo() {
    if(top >= 0) {
        Dictionary d = pop();
        dictArray[count++] = d;
        insertDCLL(d);
        insertHash(d);
        root = insertBST(root, d);
        saveToFile();
        printf("Undo success.\n");
    }
}

/* =====================================================
   MENU
===================================================== */

void menu() {
    printf("\n===== FINAL DICTIONARY SYSTEM =====\n");
    printf("1. Add Word\n");
    printf("2. Search Word\n");
    printf("3. Update Word\n");
    printf("4. Delete Word\n");
    printf("5. Display All (Array)\n");
    printf("6. Display Sorted (BST)\n");
    printf("7. Search History (Queue)\n");
    printf("8. Undo\n");
    printf("9. Exit\n");
    printf("Choose: ");
}

int main() {

    loadFromFile();

    int choice;

    do {
        menu();
        scanf("%d", &choice);

        switch(choice) {
            case 1: addWord(); break;
            case 2: searchWord(); break;
            case 3: updateWord(); break;
            case 4: deleteWord(); break;
            case 5:
                for(int i = 0; i < count; i++)
                    printf("%s -> %s\n",
                           dictArray[i].indo,
                           dictArray[i].eng);
                break;
            case 6: inorder(root); break;
            case 7: showHistory(); break;
            case 8: undo(); break;
            case 9: printf("Exit.\n"); break;
            default: printf("Invalid.\n");
        }

    } while(choice != 9);

    return 0;
}