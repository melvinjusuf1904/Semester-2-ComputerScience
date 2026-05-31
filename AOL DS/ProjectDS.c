#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 300

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

/* ================= BST ================= */

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
/* ================= SEARCH SYSTEM ================= */

#define HASH_SIZE 101

typedef struct HashNode {
    Dictionary data;
    struct HashNode* next;
} HashNode;

HashNode* hashTable[HASH_SIZE];

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

    HashNode* result =
        searchHash(word);

    if(result != NULL) {
        printf("Found:\n");

        printf("%s -> %s\n",
               result->data.indo,
               result->data.eng);

    } else {
        printf("Word not found.\n");
        printf("Use menu 9 for prefix suggestions.\n");
    }
}
/* ================= MENU ================= */

void loadFile() {

}

void saveFile() {

}

void deleteWord() {
    printf("Delete feature not ready yet.\n");
}

void changeTranslateWord() {
    printf("Change feature not ready yet.\n");
}

void undo() {
    printf("Undo feature not ready yet.\n");
}

void searchHistory() {
    printf("History feature not ready yet.\n");
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

                dictArray[count++] = d;

                insertDCLL(d);

                root = insertBST(root, d);
                insertHash(d);
                insertTrie(d.indo);
                
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
