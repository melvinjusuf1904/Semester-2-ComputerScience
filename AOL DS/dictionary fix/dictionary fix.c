#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #define CLEAR_SCREEN() system("cls")
#else
    #define CLEAR_SCREEN() system("clear")
#endif

#define MAX 320
#define HASH_SIZE 101
#define MAX_HISTORY 20
#define MAX_SUGGEST 10
#define TRIE_ALPHA 27
#define FILENAME "dictionary.txt"

/* ================= STRUCTS ================= */
typedef struct {
    char indo[100];
    char eng[100];
} Dictionary;

typedef struct Node { //DCLL
    Dictionary data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct TreeNode { //BST
    Dictionary data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct HashNode { //Hash
    Dictionary data;
    struct HashNode* next;
} HashNode;
 
typedef struct StackNode { //Stack
    int operation;
    Dictionary oldData;
    Dictionary newData;
    struct StackNode* next;
} StackNode;
 
typedef struct QueueNode { //Queue
    char keyword[100];
    char translation[100];
    struct QueueNode* next;
} QueueNode;
 
typedef struct TrieNode { //Trie
    struct TrieNode* child[TRIE_ALPHA];
    int endWord;
    char fullWord[100];
} TrieNode;


/* ================= GLOBAL VARIABLES ================= */
Dictionary dictArray[MAX];
int count = 0;
 
Node* head = NULL;
TreeNode* root = NULL;
HashNode* hashTable[HASH_SIZE];
 
#define OP_ADD 1
#define OP_DELETE 2
#define OP_CHANGE 3
 
StackNode* top = NULL;
 
QueueNode* front = NULL;
QueueNode* rear  = NULL;
int qSize = 0;
 
TrieNode* rootTrie = NULL;
char suggestList[MAX_SUGGEST][100];
int suggestCount = 0;
 
char heap[10][100];
int heapSize = 0;
int loadedWords = 0;
 
 /* ================= HELPERS ================= */
void trimNewline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}
 
void clearInputBuffer(void) { //kosongkan sisa input buffer
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}
 
void toLowerStr(const char *src, char *dst, int size) { //salin string ke lowercase
    int i;
    for(i = 0; src[i] && i < size - 1; i++) dst[i] = tolower((unsigned char)src[i]);
    dst[i] = '\0';
}
 
int trieIndex(char c) { //dapatkan index karakter untuk Trie (a-z=0..25, '-'=26, lainnya=-1)
    if(c >= 'a' && c <= 'z') return c - 'a';
    if(c >= 'A' && c <= 'Z') return c - 'A';
    if(c == '-') return 26;
    return -1;
}
 
 void pauseScreen(void) { //tekan enter untuk lanjut
    printf("\n  Press enter to back...");
    clearInputBuffer();
}


/* ================= DCLL ================= */
void insertDCLL(Dictionary d) {  //sisipkan di akhir DCLL
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
    if(head == NULL) return;
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
            if(curr == head) head = curr->next;
            free(curr);
            return;
        }
        curr = curr->next;
    } while(curr != head);
}


/* ================= BST ================= */
int bstCmp(const char *a, const char *b) { //perbandingan case-insensitive untuk BST
    char la[100], lb[100];
    toLowerStr(a, la, sizeof(la));
    toLowerStr(b, lb, sizeof(lb));
    return strcmp(la, lb);
}

TreeNode* minValueNode(TreeNode* node) { //cari node terkecil (paling kiri)
    TreeNode* curr = node;
    while(curr && curr->left != NULL) curr = curr->left;
    return curr;
}

TreeNode* insertBST(TreeNode* node, Dictionary d) { //sisipkan ke BST (urut A-Z, case-insensitive)
    if(node == NULL) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = d;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }
    int cmp = bstCmp(d.indo, node->data.indo);
    if (cmp < 0) node->left = insertBST(node->left, d);
    else if (cmp > 0) node->right = insertBST(node->right, d);
    return node;
}

TreeNode *deleteBST(TreeNode *node, char word[]) {
    if (!node) return NULL;
    int cmp = bstCmp(word, node->data.indo);

    if (cmp < 0) node->left = deleteBST(node->left, word);
    else if (cmp > 0) node->right = deleteBST(node->right, word);
    else {
        if (!node->left) {
            TreeNode *tmp = node->right;
            free(node);
            return tmp;
		}
        if (!node->right) {
            TreeNode *tmp = node->left;
            free(node);
            return tmp;
        }
        TreeNode *tmp = minValueNode(node->right);
        
        node->data = tmp->data;
        node->right = deleteBST(node->right, tmp->data.indo);
    }
    return node;
}

void inorder(TreeNode* node) { //tampilkan BST secara in-order (terurut A-Z)
    if(node != NULL) {
        inorder(node->left);
        printf("  %-22s -> %s\n", node->data.indo, node->data.eng);
        inorder(node->right);
    }
}


/* ================= HASH ================= */
int hashFunc(const char *word) {
    unsigned int sum = 0;
    for(int i = 0; word[i] != '\0'; i++) sum += (unsigned char) tolower((unsigned char)word[i]);
    return (int) (sum % HASH_SIZE);
}

void insertHash(Dictionary d) {
    int index = hashFunc(d.indo);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));

    newNode->data = d;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

HashNode* searchHash(char word[]) {
    int index = hashFunc(word);
    HashNode* temp = hashTable[index];

    while(temp != NULL) {
        if(bstCmp(temp->data.indo, word) == 0) return temp;
        temp = temp->next;
    } return NULL;
}

void deleteHash(char word[]) {
    int index = hashFunc(word);
    HashNode* curr = hashTable[index];
    HashNode* prev = NULL;
    
    while(curr != NULL) {
        if(bstCmp(curr->data.indo, word) == 0) {
            if(prev == NULL) hashTable[index] = curr->next;
            else prev->next = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
 
void updateHash(char word[], char newTranslation[]) {
    HashNode* node = searchHash(word);
    if(node != NULL) strcpy(node->data.eng, newTranslation);
}


/* ================= STACK ================= */
void pushStack(int operation, Dictionary oldData, Dictionary newData) { //push operasi ke stack
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));

    newNode->operation = operation;
    newNode->oldData = oldData;
    newNode->newData = newData;
    newNode->next = top;
    top = newNode;
}

int popStack(int* operation, Dictionary* oldData, Dictionary* newData) { //pop operasi dari stack; return 0 jika kosong
    if(top == NULL) return 0;
    StackNode* temp = top;

    *operation = temp->operation;
    *oldData = temp->oldData;
    *newData = temp->newData;
    top = top->next;
    
    free(temp);
    return 1;
}

int isStackEmpty(void) { //cek apa stack kosong
    return (top == NULL);
}

/* ================= QUEUE  ================= */
void enqueueHistory(char *keyword, char *translation) { //enqueue kata yang dicari; otomatis hapus terlama jika penuh
    //hapus elemen terlama jika antrian sudah penuh
    if (qSize >= MAX_HISTORY) {
        QueueNode *tmp = front;
        front = front->next;
        if (!front) rear = NULL;
        free(tmp);
        qSize--;
    }
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    strncpy(newNode->keyword, keyword, 99);
    newNode->keyword[99] = '\0';
    strncpy(newNode->translation, translation, 99);
    newNode->translation[99] = '\0';
    newNode->next = NULL;

    if (!rear) front = rear = newNode;
    else {
        rear->next = newNode;
        rear = newNode;
    }
    qSize++;
}

void displayHistory(void) {
    if (!front){
        printf("  [Still no search history exist]\n");
        return;
    }
    printf("\n  Search History (oldest -> newest):\n");
    printf("  %-5s %-25s %s\n", "No.", "Searched Word", "Translation");
    printf("  %-5s %-25s %s\n", "---", "-------------------------", "----------------");
    int i = 1;
    QueueNode *cur = front;
    while (cur) {
        printf("  %-5d %-25s %s\n", i++, cur->keyword, cur->translation);
        cur = cur->next;
    } printf("\n  Total: %d history (maks. %d)\n", qSize, MAX_HISTORY);
}

void searchHistory(void) {
    displayHistory();
}


/* ================= TRIE ================= */
TrieNode* createTrieNode() {
    TrieNode* newNode = (TrieNode*)malloc(sizeof(TrieNode));
    newNode->endWord = 0;
    newNode->fullWord[0] = '\0';
    for(int i = 0; i < TRIE_ALPHA; i++) newNode->child[i] = NULL;
    return newNode;
}

void insertTrie(const char *word) {
    if (!rootTrie) rootTrie = createTrieNode();
    TrieNode *cur = rootTrie;
    
    for (int i = 0; word[i]; i++) {
        int idx = trieIndex(word[i]);
        
        if (idx < 0) continue;
        if (!cur->child[idx]) cur->child[idx] = createTrieNode();
        cur = cur->child[idx];
    }
    cur->endWord = 1;
    strncpy(cur->fullWord, word, 99);
    cur->fullWord[99] = '\0';
}

int deleteTrieHelper(TrieNode* curr, char word[], int depth) {
    if(curr == NULL) return 0;
    if(word[depth] == '\0') {
        curr->endWord = 0;
        return 1;
    }
    int index = tolower(word[depth]) - 'a';

    if(index < 0 || index > 25) return 0;
    return deleteTrieHelper(curr->child[index], word, depth + 1);
}

void deleteTrie(char word[]) {
    deleteTrieHelper(rootTrie, word, 0);
}

TrieNode *searchPrefix(const char *prefix) { //cari node akhir dari prefix (case-insensitive) 
    if (!rootTrie) return NULL;
    TrieNode *cur = rootTrie;
    for (int i = 0; prefix[i]; i++) {
        int idx = trieIndex(tolower((unsigned char)prefix[i]));
        if (idx < 0 || !cur->child[idx]) return NULL;
        cur = cur->child[idx];
    }
    return cur;
}

void collectWords(TrieNode *node) {
    if(node->endWord && suggestCount < MAX_SUGGEST)
        strcpy(suggestList[suggestCount++], node->fullWord);
    for(int i = 0; i < TRIE_ALPHA; i++)
        if(node->child[i]) collectWords(node->child[i]);
}
 
 
/* ================= HEAP ================= */
void insertHeap(char word[]) { //insert suggestion
    if(heapSize >= 10) return;
    strcpy(heap[heapSize], word);
    heapSize++;
}

void traversalTrie(TrieNode* curr, char word[], int level) {
    if(curr->endWord) {
        word[level] = '\0';
        insertHeap(word);
    }

    for(int i = 0; i < 26; i++) {
        if(curr->child[i] != NULL) {
            word[level] = i + 'a';

            traversalTrie(curr->child[i], word, level + 1);
        }
    }}

void prefixSearch() {
    char prefix[100];
    printf("Input Prefix: ");
    scanf("%s", prefix);
    clearInputBuffer();
    
    TrieNode* curr = rootTrie;
    for(int i = 0; prefix[i] != '\0'; i++) {

    int index = tolower((unsigned char)prefix[i]) - 'a';
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
    traversalTrie(curr, temp, strlen(prefix));

    printf("Suggestions:\n");
    for(int i = 0; i < heapSize; i++) printf("- %s\n", heap[i]);
}


/* ================= FILE ================= */
void loadFile(){ //muat kata dari file dictionary.txt saat program mulai 
    FILE* fp = fopen(FILENAME,"r");
    if(fp == NULL) return;
    Dictionary d;
    loadedWords = 0;
    
    while(
        fscanf(fp, "%99[^|]|%99[^\n]\n", d.indo, d.eng) == 2){
        if(count >= MAX){
        printf("Dictionary is full.\n");
        break;
        }
        dictArray[count++] = d;
        
        insertDCLL(d);
        root = insertBST(root,d);
        insertHash(d);
        insertTrie(d.indo);
        loadedWords++;
    }
    fclose(fp);
}

void saveFile(){ //simpan semua kata ke file dictionary.txt
    FILE* fp = fopen(FILENAME,"w");
    if(fp == NULL){
        printf("Cannot save file.\n");
        return;
    }
    for(int i = 0; i < count; i++) fprintf(fp, "%s|%s\n", dictArray[i].indo, dictArray[i].eng);
    fclose(fp);
    printf("  Succesfully saved %d words to '%s'.\n", count, FILENAME);
}


/* ================= CRUD ================= */
void searchWord(void) {
    char indo[100];
    printf("Word to Search: ");
    fgets(indo, sizeof(indo), stdin);
    trimNewline(indo);

    if (strlen(indo) == 0) {
        printf("Input should not be empty.\n");
        return;
    }

    suggestCount = 0; //cari rekomendasi dari Trie
    TrieNode *prefixNode = searchPrefix(indo);
    if (prefixNode) collectWords(prefixNode);

    if (suggestCount > 0) { //jika ada rekomendasi, tampilkan dan tanya pilihan
        printf("\nWord recommended:\n");
        for (int i = 0; i < suggestCount; i++) printf("  [%d] %s\n", i + 1, suggestList[i]);

        HashNode *directHit = searchHash(indo);
        if (!directHit) {
            printf("Input number: ");
            char pickBuf[16];

            fgets(pickBuf, sizeof(pickBuf), stdin);
            trimNewline(pickBuf);

            int pick = atoi(pickBuf);
            if (pick >= 1 && pick <= suggestCount) {
                strncpy(indo, suggestList[pick - 1], sizeof(indo) - 1);
                indo[sizeof(indo) - 1] = '\0';
            }
            else if (pick != 0) {
                printf("Invalid.\n");
                return;
            }
        }
    }
    HashNode *found = searchHash(indo); //cari di Hash Table
    if(found) enqueueHistory(indo, found->data.eng);
    else enqueueHistory(indo, "-"); //simpan ke history
    
    if (found) {
        printf(" Successfully founded!\n");
        printf("+----------------------------------+\n");
        printf("Indonesia : %-22s\n", found->data.indo);
        printf("Inggris   : %-22s\n", found->data.eng);
        printf("+----------------------------------+\n");
    } else printf("Word '%s' not found in dictionary.\n", indo);
}

void addWord(void) {
    if (count >= MAX) {
        printf("Dictionary Full.\n");
        return;
    }
    Dictionary d;

    while (1) { //input Indonesia
        printf("Indonesia: ");
        fgets(d.indo, sizeof(d.indo), stdin);
        trimNewline(d.indo);
        d.indo[0] = toupper((unsigned char)d.indo[0]);

        if (strlen(d.indo) == 0) {
            printf("Word cannot be empty.\n");
            continue;
        }
        if (searchHash(d.indo) != NULL) {
            printf("Word already exists.\n");
            continue;
        } break;
    }
    
    while (1) { //input English
        printf("English: ");
        fgets(d.eng, sizeof(d.eng), stdin);
        trimNewline(d.eng);
        d.eng[0] = toupper((unsigned char)d.eng[0]);

        if (strlen(d.eng) == 0) {
            printf("Translation cannot be empty. Try again.\n");
            continue;
        }
        break;
    }

    dictArray[count++] = d;
    insertDCLL(d);

    root = insertBST(root, d);
    insertHash(d);
    insertTrie(d.indo);

    Dictionary empty = {"", ""};
    pushStack(OP_ADD, empty, d);
    printf("Word '%s -> %s' successfully added.\n", d.indo, d.eng);
}

void deleteWord(){
    char word[100];
    printf("Delete Word: ");
    fgets(word, sizeof(word), stdin);
    trimNewline(word);
    if(strlen(word) == 0){
        printf("Input cannot be empty.\n");
        return;
    }

    for(int i = 0; i < count; i++){
        if(bstCmp(dictArray[i].indo, word) == 0){
            Dictionary deletedData = dictArray[i];
            Dictionary empty = {"",""};
            pushStack(OP_DELETE, deletedData, empty);

    for(int j = i; j < count - 1; j++) dictArray[j] = dictArray[j+1];
        count--;
        deleteHash(word);
        deleteDCLL(word);
        root = deleteBST(root, word);

    deleteTrie(word);
            printf("Word deleted successfully.\n");
            return;
        }
    }
    printf("Word not found.\n");
}

void changeTranslateWord(){
    char word[100];
    printf("Word: ");
    fgets(word, sizeof(word), stdin);
    trimNewline(word);

    if(strlen(word) == 0){
        printf("Input cannot be empty.\n");
        return;
    }
    
    for(int i = 0; i < count; i++){
        if(bstCmp(dictArray[i].indo, word) == 0){
            Dictionary oldData = dictArray[i];
            printf("Current Translation: %s\n", oldData.eng);
            printf("New Translation: ");
            fgets(dictArray[i].eng, sizeof(dictArray[i].eng), stdin);
            trimNewline(dictArray[i].eng);

            if(strlen(dictArray[i].eng) == 0){
                printf("Translation cannot be empty.\n");
                return;
            }

            if(bstCmp(oldData.eng, dictArray[i].eng) == 0){
                printf("Translation is the same as before.\n");
                return;
            }
            updateHash(word, dictArray[i].eng);     

            deleteDCLL(word);
            insertDCLL(dictArray[i]);

            root = deleteBST(root, word);
            root = insertBST(root, dictArray[i]);
            
            Dictionary newData = dictArray[i];
            pushStack(OP_CHANGE, oldData, newData);
            printf("Translation updated successfully.\n");
            return;
        }
    } printf("Word not found.\n");
}

void undo() {
    int operation;
    Dictionary oldData, newData;
    if(!popStack(&operation, &oldData, &newData)) {
        printf("Nothing to undo.\n");
        return;
    }
    if(operation == OP_ADD) {
        for(int i = 0; i < count; i++) {
            if(bstCmp(dictArray[i].indo, newData.indo) == 0) {
                for(int j = i; j < count - 1; j++) dictArray[j] = dictArray[j + 1];
                count--;
                break;
            }
        }
        deleteHash(newData.indo);
        deleteDCLL(newData.indo);
        root = deleteBST(root, newData.indo); 
        deleteTrie(newData.indo);
        printf("Undo Add:\n");
        printf("'%s -> %s' has been removed from dictionary.\n", newData.indo, newData.eng);
    }
    else if(operation == OP_DELETE) {
        if(count >= MAX) { printf("Dictionary Full. Cannot undo.\n"); return; }
        dictArray[count++] = oldData;
        insertDCLL(oldData);
        root = insertBST(root, oldData);
        insertHash(oldData);
        insertTrie(oldData.indo);
        printf("Undo Delete:\n");
        printf("'%s -> %s' has been restored.\n", oldData.indo, oldData.eng);
    }
    else if(operation == OP_CHANGE) {
        for(int i = 0; i < count; i++) {
            if(bstCmp(dictArray[i].indo, oldData.indo) == 0) {
                dictArray[i] = oldData;
                updateHash(oldData.indo, oldData.eng);
                deleteDCLL(oldData.indo);
                insertDCLL(oldData);
                root = deleteBST(root, oldData.indo);
                root = insertBST(root, oldData);
                printf("Undo Delete:\n");
                printf("'%s -> %s' has been restored.\n", oldData.indo, oldData.eng);
                break;
            }}}
}


void displayAll(void) {
    if (count == 0) {
        printf("  [Dictionary still empty]\n");
        return;
    }
    printf("\n  %-5s %-22s %s\n", "No.", "Indonesia", "Inggris");
    printf("  %-5s %-22s %s\n",   "---", "---------", "-------");
    for (int i = 0; i < count; i++) printf("  %-5d %-22s %s\n", i + 1, dictArray[i].indo, dictArray[i].eng);
    printf("\n  Total: %d words\n", count);
}

void displaySorted(void) {
    if (!root) {
        printf("  [Kamus still empty]\n");
        return;
    }
    printf("\n  %-22s -> %s\n", "Indonesia", "Inggris");
    printf("  %-22s    %s\n",   "---------", "-------");
    inorder(root);
    printf("\n  Total: %d words (sorted A-Z)\n", count);
}

void menu() {
	printf("Successfully loaded %d words from '%s'\n\n",loadedWords, FILENAME);
    printf("  +----------------------------------+\n");
    printf("\n ==== FINAL DICTIONARY SYSTEM ====\n");
    printf("\n¦      INDONESIAN - ENGLISH        ¦ \n");
    printf("  ¦----------------------------------¦\n");
    printf("  ¦  1. Add Word                     ¦\n");
    printf("  ¦  2. Delete Word                  ¦\n");
    printf("  ¦  3. Change Translate Word        ¦\n");
    printf("  ¦  4. Search Word (Hash + Trie)    ¦\n");
    printf("  ¦  5. Display All (Array)          ¦\n");
    printf("  ¦  6. Display Sorted (BST)         ¦\n");
    printf("  ¦  7. Undo Last Operation (Stack)  ¦\n");
    printf("  ¦  8. Word Search History          ¦\n");
    printf("  ¦  9. Prefix Search                ¦\n");
    printf("  ¦  0. Save & Exit                  ¦\n");
    printf("  +----------------------------------+\n");
    printf("Choose: ");
}

/* ================= MAIN ================= */
int main() {
    int choice;
    rootTrie = createTrieNode();
   
    for(int i = 0; i < HASH_SIZE; i++) hashTable[i] = NULL;
    loadFile();
    do {
    	CLEAR_SCREEN();
        menu();
        if(scanf("%d", &choice) != 1){
            printf("Invalid input.\n");
            while(getchar() != '\n');
            getchar();
            continue;
        }
        getchar();
        printf("\n");
        
        switch(choice) {
            case 1: //add word
                CLEAR_SCREEN();
                printf("=== ADD WORD ===\n");
				addWord();
                break;
            case 2: //delete word
                CLEAR_SCREEN();
                printf("=== DELETE WORD ===\n");
                deleteWord();
                break;
            case 3: //change translate word
                CLEAR_SCREEN();
                printf("=== CHANGE TRANSLATION WORD ===\n");
                changeTranslateWord();
                break;
            case 4: //search word
                CLEAR_SCREEN();
                printf("=== SEARCH WORD (HASH + TRIE) ===\n");
                searchWord();
                break;
            case 5: //display all (array)
                CLEAR_SCREEN();
                printf("=== DISPLAY ALL WORDS (ARRAY) ===\n");
                displayAll();
                break;
            case 6: //display sorted BST
                CLEAR_SCREEN();
                printf("=== SORTED DICTIONARY (BST) ===\n");
                displaySorted();
                break;
            case 7: //undo last operation
                CLEAR_SCREEN();
                printf("  === UNDO LAST OPEARTION ===\n");
                undo();
                break;

            case 8: //show search history
                CLEAR_SCREEN();
                printf("  === SEARCH HISTORY ===\n");
                searchHistory();
                break;

            case 9: //prefix search
                CLEAR_SCREEN();
                printf("  === PREFIX SEARCH ===\n");
                prefixSearch();
                break;
            
            case 0: //save and exit
                CLEAR_SCREEN();
                printf("  === SAVE & EXIT ===\n");
                saveFile();
                printf("Thank you for using the Dictionary System.\n");
                break;

            default: printf("  [!] Invalid choice. Please input 0-9.\n");
        } if (choice != 0) pauseScreen();
    } while(choice != 0);
    return 0;
}

