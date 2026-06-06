#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 300
#define FILENAME "dictionary.txt"

typedef struct {
    char indo[100];
    char eng[100];
} Dictionary;

/* ================= ARRAY ================= */

Dictionary dictArray[MAX];
int count = 0;

/* ================= DCLL ================= */

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

void deleteDCLL(char word[]) {

    if(head == NULL)
        return;

    Node* curr = head;

    do {

        if(strcmp(curr->data.indo, word) == 0) {

            if(curr->next == curr) {
                free(curr);
                head = NULL;
                return;
            }

            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;

            if(curr == head)
                head = curr->next;

            free(curr);
            return;
        }

        curr = curr->next;

    } while(curr != head);
}

/* ================= BST ================= */

typedef struct TreeNode {
    Dictionary data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* root = NULL;

TreeNode* minValueNode(TreeNode* node) {

    TreeNode* curr = node;

    while(curr && curr->left != NULL)
        curr = curr->left;

    return curr;
}

TreeNode* deleteBST(
    TreeNode* root,
    char word[]
) {

    if(root == NULL)
        return root;

    if(strcmp(word, root->data.indo) < 0)
        root->left =
            deleteBST(root->left, word);

    else if(strcmp(word, root->data.indo) > 0)
        root->right =
            deleteBST(root->right, word);

    else {

        if(root->left == NULL) {
            TreeNode* temp =
                root->right;
            free(root);
            return temp;
        }
        else if(root->right == NULL) {
            TreeNode* temp =
                root->left;
            free(root);
            return temp;
        }

        TreeNode* temp =
            minValueNode(root->right);
        root->data = temp->data;
        root->right =
            deleteBST(
                root->right,
                temp->data.indo
            );
    }

    return root;
}

TreeNode* insertBST(
    TreeNode* node,
    Dictionary d
) {

    if(node == NULL) {

        TreeNode* newNode =
            (TreeNode*)malloc(sizeof(TreeNode));

        newNode->data = d;
        newNode->left = NULL;
        newNode->right = NULL;

        return newNode;
    }

    if(strcmp(d.indo, node->data.indo) < 0)
        node->left =
            insertBST(node->left, d);

    else
        node->right =
            insertBST(node->right, d);

    return node;
}

void inorder(TreeNode* node) {
    if(node != NULL) {
        inorder(node->left);
        printf("%s -> %s\n", node->data.indo, node->data.eng);
        inorder(node->right);
    }
}
/* ================= SEARCH SYSTEM ================= */

#define HASH_SIZE 101

#define OP_ADD 1
#define OP_DELETE 2
#define OP_CHANGE 3

typedef struct HashNode {
    Dictionary data;
    struct HashNode* next;
} HashNode;

HashNode* hashTable[HASH_SIZE];

/* ================= STACK ================= */

typedef struct StackNode {
    int operation; // 1=ADD, 2=DELETE, 3=CHANGE
    Dictionary oldData;
    Dictionary newData;
    struct StackNode* next;
} StackNode;

StackNode* top = NULL;

/* ================= QUEUE ================= */

typedef struct QueueNode {
    char word[100];
    struct QueueNode* next;
} QueueNode;

QueueNode* front = NULL;
QueueNode* rear = NULL;
/* ================= HASH ================= */

int hashFunc(char word[]) {
    int sum = 0;

    for(int i = 0; word[i] != '\0'; i++) {
        sum += word[i];
    }

    return sum % HASH_SIZE;
}

void insertHash(Dictionary d) {
    int index = hashFunc(d.indo);

    HashNode* newNode =
        (HashNode*)malloc(sizeof(HashNode));

    newNode->data = d;
    newNode->next = hashTable[index];

    hashTable[index] = newNode;
}

HashNode* searchHash(char word[]) {
    int index = hashFunc(word);

    HashNode* temp = hashTable[index];

    while(temp != NULL) {
        if(strcmp(temp->data.indo, word) == 0) {
            return temp;
        }

        temp = temp->next;
    }

    return NULL;
}
/* ================= STACK FUNCTIONS ================= */

void pushStack(int operation,
               Dictionary oldData,
               Dictionary newData) {

    StackNode* newNode =
        (StackNode*)malloc(sizeof(StackNode));

    newNode->operation = operation;
    newNode->oldData = oldData;
    newNode->newData = newData;
    newNode->next = top;

    top = newNode;
}

int popStack(int* operation,
             Dictionary* oldData,
             Dictionary* newData) {

    if(top == NULL)
        return 0;

    StackNode* temp = top;

    *operation = temp->operation;
    *oldData = temp->oldData;
    *newData = temp->newData;

    top = top->next;

    free(temp);

    return 1;
}

/* ================= QUEUE FUNCTIONS ================= */

void enqueueHistory(char word[]) {

    QueueNode* newNode =
        (QueueNode*)malloc(sizeof(QueueNode));

    strcpy(newNode->word, word);
    newNode->next = NULL;

    if(rear == NULL) {
        front = rear = newNode;
    }
    else {
        rear->next = newNode;
        rear = newNode;
    }
}

void searchHistory() {

    QueueNode* temp = front;

    printf("=== Search History ===\n");

    while(temp != NULL) {
        printf("%s\n", temp->word);
        temp = temp->next;
    }
}
/* ================= TRIE ================= */

typedef struct TrieNode {
    struct TrieNode* child[26];
    int endWord;
} TrieNode;

TrieNode* rootTrie;

/* Create Trie */

TrieNode* createTrieNode() {
    TrieNode* newNode =
        (TrieNode*)malloc(sizeof(TrieNode));

    newNode->endWord = 0;

    for(int i = 0; i < 26; i++) {
        newNode->child[i] = NULL;
    }

    return newNode;
}

/* Insert Trie */

void insertTrie(char word[]) {
    TrieNode* curr = rootTrie;

    for(int i = 0; word[i] != '\0'; i++) {
        char c = tolower(word[i]);

        if(c < 'a' || c > 'z')
            continue;

        int index = c - 'a';

        if(curr->child[index] == NULL) {
            curr->child[index] =
                createTrieNode();
        }

        curr = curr->child[index];
    }

    curr->endWord = 1;
}

int deleteTrieHelper(
    TrieNode* curr,
    char word[],
    int depth
) {

    if(curr == NULL)
        return 0;

    if(word[depth] == '\0') {

        curr->endWord = 0;

        return 1;
    }

    int index =
        tolower(word[depth]) - 'a';

    if(index < 0 || index > 25)
        return 0;

    return deleteTrieHelper(
        curr->child[index],
        word,
        depth + 1
    );
}

void deleteTrie(char word[]) {

    deleteTrieHelper(
        rootTrie,
        word,
        0
    );
}

/* ================= HEAP ================= */

char heap[10][100];
int heapSize = 0;

/* Insert Suggestion */

void insertHeap(char word[]) {
    if(heapSize >= 10)
        return;

    strcpy(heap[heapSize], word);

    heapSize++;
}

/* Trie Traversal */

void traversalTrie(TrieNode* curr,
                   char word[],
                   int level) {

    if(curr->endWord) {
        word[level] = '\0';

        insertHeap(word);
    }

    for(int i = 0; i < 26; i++) {
        if(curr->child[i] != NULL) {
            word[level] = i + 'a';

            traversalTrie(
                curr->child[i],
                word,
                level + 1
            );
        }
    }
}

/* Prefix Search */

void prefixSearch() {
    char prefix[100];

    printf("Input Prefix: ");
    scanf("%s", prefix);

    TrieNode* curr = rootTrie;

    for(int i = 0; prefix[i] != '\0'; i++) {

    int index =
        tolower(prefix[i]) - 'a';

    if(index < 0 || index > 25){
        printf("Invalid prefix.\n");
        return;
    }

    if(curr->child[index] == NULL) {
            printf("No matching words.\n");
            return;
        }

        curr = curr->child[index];
    }

    heapSize = 0;

    char temp[100];

    strcpy(temp, prefix);

    traversalTrie(
        curr,
        temp,
        strlen(prefix)
    );

    printf("Suggestions:\n");

    for(int i = 0; i < heapSize; i++) {
        printf("- %s\n", heap[i]);
    }
}

/* ================= SEARCH WORD ================= */
void searchWord() {

    char word[100];

    printf("Search Word: ");
    scanf("%s", word);

    enqueueHistory(word);

    /* Indonesia -> English (Hash Table) */
    HashNode* result = searchHash(word);

    if(result != NULL){
        printf(
            "%s -> %s\n",
            result->data.indo,
            result->data.eng
        );
        return;
    }

    /* English -> Indonesia (Array) */
    for(int i = 0; i < count; i++) {

        if(strcmp(
            dictArray[i].eng,
            word
        ) == 0){

            printf(
                "%s -> %s\n",
                dictArray[i].eng,
                dictArray[i].indo
            );
            return;
        }
    }

    printf("Word not found.\n");
}
/* ================= MENU ================= */

void loadFile(){

    FILE* fp =
        fopen(FILENAME,"r");

    if(fp == NULL)
        return;

    Dictionary d;

    while(
        fscanf(
            fp,
            "%[^|]|%[^\n]\n",
            d.indo,
            d.eng
        ) == 2
    ){
        if(count >= MAX){
        printf("Dictionary Full.\n");
        break;
        }
        
        dictArray[count++] = d;
        insertDCLL(d);
        root =
            insertBST(root,d);
        insertHash(d);
        insertTrie(d.indo);
        
    }
    fclose(fp);
}

void saveFile(){
    FILE* fp =
        fopen(FILENAME,"w");
    if(fp == NULL){
        printf("Cannot save file.\n");
        return;
    }
    for(int i = 0; i < count; i++){
        fprintf(
            fp,
            "%s|%s\n",
            dictArray[i].indo,
            dictArray[i].eng
        );
    }
    fclose(fp);
}

void deleteHash(char word[]){
    int index = hashFunc(word);

    HashNode* curr = hashTable[index];
    HashNode* prev = NULL;

    while(curr != NULL){
        if(strcmp(curr->data.indo, word) == 0){
            if(prev == NULL)
                hashTable[index] = curr->next;
            else
                prev->next = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void deleteWord(){

    char word[100];

    printf("Delete Word: ");
    scanf("%s", word);
    for(int i = 0; i < count; i++){
        if(strcmp(dictArray[i].indo, word) == 0){
            Dictionary deletedData =
                dictArray[i];
            Dictionary empty = {"",""};
            pushStack(
                OP_DELETE,
                deletedData,
                empty
            );

            for(int j = i; j < count - 1; j++){
                dictArray[j] = dictArray[j+1];
            }
            count--;
            deleteHash(word);
            deleteDCLL(word);
            root =
            deleteBST(
                root,
                word
            );

deleteTrie(word);
            printf("Word deleted.\n");
            return;
        }
    }

    printf("Word not found.\n");
}

void updateHash(char word[],
                char newTranslation[])
{
    HashNode* node =
        searchHash(word);

    if(node != NULL){
        strcpy(
            node->data.eng,
            newTranslation
        );
    }
}

void changeTranslateWord(){

    char word[100];

    printf("Word: ");
    scanf("%s", word);

    for(int i = 0; i < count; i++){

        if(strcmp(dictArray[i].indo, word) == 0){

            Dictionary oldData =
                dictArray[i];

            printf("New Translation: ");
            scanf("%s", dictArray[i].eng);

            updateHash(
                word,
                dictArray[i].eng
            );     

            deleteDCLL(word);
            insertDCLL(dictArray[i]);

            root = deleteBST(root, word);
            root = insertBST(root, dictArray[i]);
            
            Dictionary newData =
                dictArray[i];

            pushStack(
                OP_CHANGE,
                oldData,
                newData
            );
            printf("Translation updated.\n");
            return;
        }
    }
    printf("Word not found.\n");
}

void undo(){

    int operation;

    Dictionary oldData;
    Dictionary newData;

    if(!popStack(
        &operation,
        &oldData,
        &newData))
    {
        printf("Nothing to undo.\n");
        return;
    }

    if(operation == OP_DELETE){

    if(count >= MAX){
        printf("Dictionary Full. Cannot undo.\n");
        return;
    }
        
    dictArray[count++] = oldData;
    insertDCLL(oldData);
    root =
        insertBST(
            root,
            oldData
        );
    insertHash(oldData);
    insertTrie(
        oldData.indo
    );
    printf("Delete undone.\n");
}

    else if(operation == OP_CHANGE){

        for(int i = 0; i < count; i++){
            if(strcmp(
                dictArray[i].indo,
                oldData.indo) == 0){
                dictArray[i] = oldData;
                break;
            }
        }
        updateHash(
            oldData.indo,
            oldData.eng
        );

        deleteDCLL(newData.indo);
        insertDCLL(oldData);

        root = deleteBST(
            root,
            newData.indo
        );

root = insertBST(
    root,
    oldData
);
        
        printf("Change undone.\n");
    }
    else if(operation == OP_ADD){

    for(int i = 0; i < count; i++){

        if(strcmp(
            dictArray[i].indo,
            newData.indo) == 0){

            for(int j = i;
                j < count - 1;
                j++){
                dictArray[j] =
                    dictArray[j+1];
            }
            count--;
            break;
        }
    }

    deleteHash(newData.indo);
    deleteDCLL(newData.indo);
    root =
    deleteBST(
        root,
        newData.indo
    );

    deleteTrie(
        newData.indo
    );    
        
    printf("Add undone.\n");
        }
    }

void menu() {
    printf("  ╔════════════════════════════════╗\n");
    printf("\n === FINAL DICTIONARY SYSTEM ===\n");
    printf("  ╠════════════════════════════════╣\n");
    printf("  ║  1. Add Word                   ║\n");
    printf("  ║  2. Delete Word                ║\n");
    printf("  ║  3. Change Translate Word      ║\n");
    printf("  ║  4. Search Word                ║\n");
    printf("  ║  5. Display All (Array)        ║\n");
    printf("  ║  6. Display Sorted (BST)       ║\n");
    printf("  ║  7. Undo Last Operation        ║\n");
    printf("  ║  8. Word Search History        ║\n");
    printf("  ║  9. Prefix Search              ║\n");
    printf("  ║  0. Save & Exit                ║\n");
    printf("  ╚════════════════════════════════╝\n");
    printf("Choose: ");
}

/* ================= MAIN ================= */

int main() {
    int choice;
    Dictionary d;
    rootTrie = createTrieNode();
   
    for(int i = 0; i < HASH_SIZE; i++) {
    hashTable[i] = NULL;
    }
    
    loadFile();
    do {
        menu();
        scanf("%d", &choice);
        printf("\n");
        
        switch(choice) {
            case 1:
                //add word
                printf("Indonesia: ");
                scanf("%s", d.indo);

                printf("English: ");
                scanf("%s", d.eng);

                if(searchHash(d.indo) != NULL){
                    printf("Word already exists.\n");
                    break;
                }

                if(count >= MAX){
                    printf("Dictionary Full.\n");
                    break;
                }
                
                dictArray[count++] = d;

                insertDCLL(d);

                root = insertBST(root, d);
                insertHash(d);
                insertTrie(d.indo);

                Dictionary empty = {"",""};

                pushStack(
                OP_ADD,
                empty,
                d
                );
                
                printf("Word added.\n");
                break;

            case 2:
                //delete word
                deleteWord();
                break;

            case 3:
                //change translate word
                changeTranslateWord();
                break;

            case 4:
                //search word
                searchWord();
                break;
            
            case 5:
                //display all (array)
                for(int i = 0; i < count; i++) {
                    printf("%s -> %s\n",
                           dictArray[i].indo,
                           dictArray[i].eng);
                }
                break;
            
            case 6:
                //display sorted BST
                printf("=== Sorted Dictionary (BST) ===\n");
                inorder(root);
                break;

               case 7:
                //undo
                undo();
                break;

               case 8:
                //show search history
                searchHistory();
                break;

                case 9:
                prefixSearch();
                break;
            
            case 0:
                //save and exit
                saveFile();
                printf("Exit.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
        printf("\n");
        
    } while(choice != 0);
    return 0;
}
