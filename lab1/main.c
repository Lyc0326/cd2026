#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    char c;
    struct Node* next;
} Node;

int main() {
    Node* head = NULL;
    Node* tail = NULL;
    
    // 0 代表沒出現過，1 代表已經在 Linked-list 裡面了
    int seen[256] = {0}; 

    FILE *fp = fopen(__FILE__, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file.\n");
        return 1;
    }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch >= 32 && ch <= 126) { 
            
            if (seen[ch] == 0) {
                Node* new_node = (Node*)malloc(sizeof(Node));
                new_node->c = (char)ch;
                new_node->next = NULL;

                if (head == NULL) {
                    head = new_node;
                    tail = new_node;
                } else {
                    tail->next = new_node;
                    tail = new_node;
                }
                
                // 標記為已出現，下次遇到就不會再加進去了
                seen[ch] = 1; 
            }
        }
    }
    fclose(fp);

    Node* current = head;
    while (current != NULL) {
        
        // 如果是最後一個字元，就不印後面的逗號
        if (current->next != NULL) {
            printf("%c, ", current->c);
        } else {
            printf("%c\n", current->c);
        }
        
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}