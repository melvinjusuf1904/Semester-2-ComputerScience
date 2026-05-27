#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* ================= MENU ================= */

void menu() {
    printf("  ╔════════════════════════════════╗\n");
    printf("\n === FINAL DICTIONARY SYSTEM ===\n");
    printf("  ╠════════════════════════════════╣\n");
    printf("  ║  1. Add Word                   ║\n");
    printf("  ║  2. Delete Word                ║\n"
    printf("  ║  3. Change Translate Words     ║\n");
    printf("  ║  4. Search Word                ║\n");
    printf("  ║  5. Display All (Array)        ║\n");
    printf("  ║  6. Display Sorted (BST)       ║\n");
    printf("  ║  7. Undo Last Operation        ║\n");
    printf("  ║  8. Word Search History        ║\n");
    printf("  ║  0. Save & Exit                ║\n");
    printf("  ╚════════════════════════════════╝\n");
    printf("Choose: ");
}

/* ================= MAIN ================= */

int main() {
    int choice;
    Dictionary d;

    do {
        menu();
        scanf("%d", &choice);

        switch(choice) {

            case 1:
                printf("Indonesia: ");
                scanf("%s", d.indo);

                printf("English: ");
                scanf("%s", d.eng);

                dictArray[count++] = d;

                insertDCLL(d);

                root = insertBST(root, d);

                printf("Word added.\n");
                break;
            case 2:
                for(int i = 0; i < count; i++) {
                    printf("%s -> %s\n",
                           dictArray[i].indo,
                           dictArray[i].eng);
                }
                break;
            case 3:
                inorder(root);
                break;
            case 4:
                printf("Exit.\n");
                break;
            default:
                printf("Invalid.\n");
        }
    } while(choice != 4);
    return 0;
}
