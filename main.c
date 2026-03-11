// HW#0
#define LINEARSEARCH 1    // Linear Search
#define HASH 2            // Hash Table (Direct Access)
#define BST 3             // Binary Search Tree
// #define AVL 4          // AVL (Balanced Binary Search Tree), partially implemented

#define YES 1
#define NO 0

#define EFFCHECK NO       // YES/NO, efficiency check: big-O (max depth), min/avg depth, etc.

#define SEARCH HASH
/*******************************************************************************
   Algorithm                            Time complexity comparison
1: linear search                         O(n)
2: hash table (direct access)            O(1)
3. BST (binary search tree)              O(n)
4: AVL (balanced binary search tree)     O(log n), partially implemented (rotations pending)

Description:
1. Linear search is O(n) time complexity
   Unlimited set of characters.
   The last added character in the linked-list is with O(n) time complexity.
   No additional memory overhead, simply a linked-list.
2. Direct access is O(1) time complexity
   Essentially a hash table with the key being the ASCII value of the character.
   *Crucially, it still maintains a linked-list structure via 'next' pointers to fulfill HW requirements.*
   Specific to a finite set of 256 ASCII characters, but can be extended to Unicode.
   In this implementation, only printable ASCII characters (32-126) are counted.
   Finite and few additional memory overhead for the map.
3. Binary search tree is O(n) time complexity.
   Unlimited set of characters.
   Worst case time complexity is O(n) when the tree is unbalanced.
   Additional memory overhead for tree structure.
4. AVL, a Balanced binary search tree, is O(log n) time complexity.
   Unlimited set of characters.
   Additional memory overhead for tree structure.
   Partially implemented: Node insertion and Balance Factor (BF) tracking are included, 
   but rebalancing rotations (LL, RR, LR, RL) are pending.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if EFFCHECK == YES
    #define INT_MAX 0x7fffffff  // initialized for min depth calculation
    double log2approx(int x) {
        int n = 0;
        int temp = x;
        while (temp > 1) {
            temp >>= 1;
            n++;
        }
        double x1 = (double)x / (1 << n);

        // Data points for approximating the fractional part of log2(x) using Lagrange interpolation
        // xx is the fractional value (1.0 to 2.0), y is the corresponding log2(xx)
        double xx[] = {1.0, 1.2, 1.4, 1.6, 1.8, 2.0};
        double y[] = {0, 0.26303, 0.48543, 0.67807, 0.84800, 1};
        int in = 6; // number of nodes for lagrange interpolation
        double result = 0;
        for (int i = 0; i < in; i++) { // Largrange interpolation
            double term = y[i];
            for (int j = 0; j < in; j++) {
                if (j != i) {
                    term *= (x1 - xx[j]) / (xx[i] - xx[j]);
                }
            }
            result += term;
        }

        return n + result;
    }
#endif

typedef struct Node Node;
typedef struct List List;

struct Node {
    char ch;
    int count;
    Node* next;

    #if SEARCH == BST || SEARCH == AVL
        Node* prev; // for doubly linked-list
        Node* left; // for [balanced] binary search tree
        Node* right;
    #endif

    #if SEARCH == AVL
        int BF; // balance factor, for AVL tree
        Node* parent; // parent node, for AVL tree
    #endif
};

struct List {
    Node* head;
    Node* tail;
    List* (*addch)(struct List* lst, char ch);

    #if SEARCH == HASH
        Node* hash[256];
    #endif

    #if SEARCH == AVL
        Node* root; // root node of the AVL tree
    #endif

    #if EFFCHECK == YES
        int n; // number of nodes created
        int depth; // maximum traversal depth
        int mindepth; // minimum traversal depth
        int searches; // number of searches, for average traversal depth calculation
        int totaldepth; // total traversal depth, for average traversal depth calculation
    #endif
};

#if SEARCH == LINEARSEARCH
List* addch(List* lst, char ch) {
    #if EFFCHECK == YES
        int trav = 0;
        lst->searches++;
    #endif

    if (lst->head == NULL) {
        lst->head = (Node*)malloc(sizeof(Node));
        lst->head->ch = ch;
        lst->head->count = 1;
        lst->head->next = NULL;
        lst->tail = lst->head;

        #if EFFCHECK == YES
            lst->n++; 
            trav++;
        #endif
    } else {
        Node* cur = lst->head;
        while (cur != NULL) {
            #if EFFCHECK == YES
                trav++;
            #endif

            if (cur->ch == ch) {
                cur->count++;

                #if EFFCHECK == YES 
                    lst->totaldepth += trav;
                    if (trav > lst->depth) {
                        lst->depth = trav;
                    }
                    if (trav < lst->mindepth) {
                        lst->mindepth = trav;
                    }
                #endif

                return lst;
            }
            cur = cur->next;
        }
        lst->tail->next = (Node*)malloc(sizeof(Node));
        lst->tail->next->ch = ch;
        lst->tail->next->count = 1;
        lst->tail->next->next = NULL;
        lst->tail = lst->tail->next;

        #if EFFCHECK == YES
            lst->n++; 
        #endif
    }

    #if EFFCHECK == YES 
        lst->totaldepth += trav;
        if (trav > lst->depth) {
            lst->depth = trav;
        }
        if (trav < lst->mindepth) {
            lst->mindepth = trav;
        }
    #endif

    return lst;
}
#elif SEARCH == HASH
List* addch(List* lst, char ch) {
    #if EFFCHECK == YES
        lst->searches++;
        int trav = 0;
    #endif

    if (lst->hash[(unsigned char)ch] == NULL) {
        lst->hash[(unsigned char)ch] = (Node*)malloc(sizeof(Node));
        lst->hash[(unsigned char)ch]->ch = ch;
        lst->hash[(unsigned char)ch]->count = 1;
        lst->hash[(unsigned char)ch]->next = NULL;

        #if EFFCHECK == YES
            lst->n++;
            trav++;
        #endif

        if (lst->head == NULL) {
            lst->head = lst->hash[(unsigned char)ch];
            lst->tail = lst->hash[(unsigned char)ch];
        } else {
            lst->tail->next = lst->hash[(unsigned char)ch];
            lst->tail = lst->hash[(unsigned char)ch];
        }
    } else {
        lst->hash[(unsigned char)ch]->count++;

        #if EFFCHECK == YES
            trav++;
        #endif
    }

    #if EFFCHECK == YES
        if (lst->searches > 0) {
            lst->totaldepth += trav;
            if (trav > lst->depth) {
                lst->depth = trav;
            }
        }
        if (lst->mindepth > trav) {
            lst->mindepth = trav;
        }
    #endif

    return lst;
}
#elif SEARCH == BST
List* addch(List* lst, char ch) {
    #if EFFCHECK == YES
        lst->searches++;
        int trav = 0;
    #endif

    if (lst->head == NULL) {
        lst->head = (Node*)malloc(sizeof(Node));
        lst->head->ch = ch;
        lst->head->count = 1;
        lst->head->left = NULL;
        lst->head->right = NULL;
        lst->head->next = NULL;
        lst->head->prev = NULL;
        lst->tail = lst->head;

        #if EFFCHECK == YES
            lst->n++;
            trav++;
        #endif
    } else {
        Node* cur = lst->head; 
        Node* parent = NULL;
        while (cur != NULL) {
            #if EFFCHECK == YES
                trav++;
            #endif

            if (cur->ch == ch) {
                cur->count++;

                #if EFFCHECK == YES 
                    lst->totaldepth += trav;
                    if (trav > lst->depth) {
                        lst->depth = trav;
                    }
                    if (trav < lst->mindepth) {
                        lst->mindepth = trav;
                    }
                #endif

                return lst;
            } else if (ch < cur->ch) {
                parent = cur;
                cur = cur->left;
            } else {
                parent = cur;
                cur = cur->right;
            }
        }
        Node* newnode = (Node*)malloc(sizeof(Node));
        newnode->ch = ch;
        newnode->count = 1;
        newnode->left = NULL;
        newnode->right = NULL;
        newnode->next = NULL;
        newnode->prev = lst->tail;
        if (ch < parent->ch) {
            parent->left = newnode;
        } else {
            parent->right = newnode;
        }
        lst->tail->next = newnode;
        lst->tail = newnode;

        #if EFFCHECK == YES
            lst->n++;
        #endif
    }

    #if EFFCHECK == YES 
        lst->totaldepth += trav;
        if (trav > lst->depth) {
            lst->depth = trav;
        }
        if (trav < lst->mindepth) {
            lst->mindepth = trav;
        }
    #endif

    return lst;
}
#elif SEARCH == AVL
List* addch(List* lst, char ch) {
    #if EFFCHECK == YES
        lst->searches++;
        int trav = 0;
    #endif

    if (lst->head == NULL) {
        lst->head = (Node*)malloc(sizeof(Node));
        lst->head->ch = ch;
        lst->head->count = 1;
        lst->head->left = NULL;  
        lst->head->right = NULL; 
        lst->head->next = NULL;  
        lst->head->prev = NULL;  
        lst->head->BF = 0;       
        lst->head->parent = NULL; 
        lst->root = lst->head;   
        lst->tail = lst->head;   

        #if EFFCHECK == YES
            lst->n++;
            trav++;
        #endif
    } else {
        Node* cur = lst->root; 
        Node* parent = NULL; 
        while (cur != NULL) {
            #if EFFCHECK == YES
                trav++;
            #endif

            if (cur->ch == ch) {
                cur->count++;

                #if EFFCHECK == YES 
                    lst->totaldepth += trav;
                    if (trav > lst->depth) {
                        lst->depth = trav;
                    }
                    if (trav < lst->mindepth) {
                        lst->mindepth = trav;
                    }
                #endif

                return lst;
            } else if (ch < cur->ch) {
                parent = cur;
                cur = cur->left;
            } else {
                parent = cur;
                cur = cur->right;
            }
        } 

        Node* newnode = (Node*)malloc(sizeof(Node));
        newnode->parent = parent; 
        newnode->ch = ch; 
        newnode->count = 1; 
        newnode->left = NULL; 
        newnode->right = NULL; 
        newnode->next = NULL; 
        newnode->prev = lst->tail; 
        newnode->BF = 0; 
        lst->tail = newnode; 

        #if EFFCHECK == YES
            lst->n++; 
            lst->totaldepth += trav;
            if (trav > lst->depth) {
                lst->depth = trav;
            }
            if (trav < lst->mindepth) {
                lst->mindepth = trav;
            }
        #endif

        if (ch < parent->ch) {
            parent->left = newnode;
            parent->BF++;
        } else {
            parent->right = newnode;
            parent->BF--;
        }
    }
    return lst;
}
#endif

List* new_list() {
    List* lst = (List*)malloc(sizeof(List));
    lst->head = NULL;
    lst->tail = NULL;
    lst->addch = addch;

    #if SEARCH == HASH
        for (int i = 0; i < 256; i++) {
            lst->hash[i] = NULL;
        }
    #endif

    #if EFFCHECK == YES
        lst->n = 0;
        lst->depth = 0;
        lst->mindepth = INT_MAX;
        lst->searches = 0;
        lst->totaldepth = 0;
    #endif
    
    return lst;
}

void free_list(List* lst) {
    Node* cur = lst->head;
    while (cur != NULL) {
        Node* tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    #if SEARCH == HASH 
        for (int i = 0; i < 256; i++) {
            lst->hash[i] = NULL;
        }
    #endif

    free(lst);
}

int main() {
    #if EFFCHECK == YES
        int totalprintablechars = 0;
    #endif

    // [HW Requirement] Read its own source code dynamically
    FILE* file = fopen(__FILE__, "r");
    if (file == NULL) {
        printf("File not found.\n");
        return 1;
    }
    List* lst = new_list();
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch >= 32 && ch <= 126) { 
            #if EFFCHECK == YES
                totalprintablechars++;
            #endif
            lst = lst->addch(lst, (char)ch);
        }
    }
    fclose(file);

    Node* cur = lst->head;
    while (cur != NULL) {
        if (cur->ch >= 32 && cur->ch <= 126) {
            printf("%c:%d\n", cur->ch, cur->count);
        }
        cur = cur->next;
    }

    #if EFFCHECK == YES
        printf("Statistics:\n");
        printf("  Number of nodes created: %d\n", lst->n);
        printf("  Number of searches: %d\n", lst->searches);
        printf("  Total printable characters: %d\n", totalprintablechars);
        printf("  Aggregated Traversal Depth: %d\n", lst->totaldepth);
        printf("  Minimum traversal depth: %d\n", lst->mindepth);
        printf("  Average traversal depth: %.2f\n", (float)lst->totaldepth / lst->searches);
        printf("  Maximum traversal depth: %d\n", lst->depth);
        #if SEARCH == AVL
            printf("Minimum possible height of the AVL tree: %d\n", (int)log2approx(lst->n + 1));
            printf("Maximum possible height of the AVL tree: %d\n", (int)(1.44*log2approx(lst->n + 2)-0.328));
        #endif
    #endif

    free_list(lst);
    return 0;
}