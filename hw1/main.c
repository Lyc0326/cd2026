// HW#1 - Crap-C Scanner (Lexer)
// Based on HW#0

#define LINEARSEARCH 1
#define HASH 2
#define BST 3
// #define AVL 4

#define YES 1
#define NO 0

#define EFFCHECK NO
#define SEARCH HASH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if EFFCHECK == YES
    #define INT_MAX 0x7fffffff
    double log2approx(int x) {
        int n = 0;
        int temp = x;
        while (temp > 1) {
            temp >>= 1;
            n++;
        }
        double x1 = (double)x / (1 << n);

        double xx[] = {1.0, 1.2, 1.4, 1.6, 1.8, 2.0};
        double y[] = {0, 0.26303, 0.48543, 0.67807, 0.84800, 1};
        int in = 6;
        double result = 0;
        int i, j;

        for (i = 0; i < in; i++) {
            double term = y[i];
            for (j = 0; j < in; j++) {
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
        Node* prev;
        Node* left;
        Node* right;
    #endif

    #if SEARCH == AVL
        int BF;
        Node* parent;
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
        Node* root;
    #endif

    #if EFFCHECK == YES
        int n;
        int depth;
        int mindepth;
        int searches;
        int totaldepth;
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
                    if (trav > lst->depth) lst->depth = trav;
                    if (trav < lst->mindepth) lst->mindepth = trav;
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
        if (trav > lst->depth) lst->depth = trav;
        if (trav < lst->mindepth) lst->mindepth = trav;
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
            if (trav > lst->depth) lst->depth = trav;
        }
        if (lst->mindepth > trav) lst->mindepth = trav;
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
                    if (trav > lst->depth) lst->depth = trav;
                    if (trav < lst->mindepth) lst->mindepth = trav;
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

        {
            Node* newnode = (Node*)malloc(sizeof(Node));
            newnode->ch = ch;
            newnode->count = 1;
            newnode->left = NULL;
            newnode->right = NULL;
            newnode->next = NULL;
            newnode->prev = lst->tail;

            if (ch < parent->ch) parent->left = newnode;
            else parent->right = newnode;

            lst->tail->next = newnode;
            lst->tail = newnode;

            #if EFFCHECK == YES
                lst->n++;
            #endif
        }
    }

    #if EFFCHECK == YES
        lst->totaldepth += trav;
        if (trav > lst->depth) lst->depth = trav;
        if (trav < lst->mindepth) lst->mindepth = trav;
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
                    if (trav > lst->depth) lst->depth = trav;
                    if (trav < lst->mindepth) lst->mindepth = trav;
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

        {
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
                if (trav > lst->depth) lst->depth = trav;
                if (trav < lst->mindepth) lst->mindepth = trav;
            #endif

            if (ch < parent->ch) {
                parent->left = newnode;
                parent->BF++;
            } else {
                parent->right = newnode;
                parent->BF--;
            }
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
        {
            int i;
            for (i = 0; i < 256; i++) lst->hash[i] = NULL;
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
        {
            int i;
            for (i = 0; i < 256; i++) lst->hash[i] = NULL;
        }
    #endif

    free(lst);
}

/* ========================= */
/* HW#1 Scanner starts here  */
/* ========================= */

#define TYPE_TOKEN          1
#define MAIN_TOKEN          2
#define IF_TOKEN            3
#define ELSE_TOKEN          4
#define WHILE_TOKEN         5
#define ID_TOKEN            6
#define LITERAL_TOKEN       7
#define ASSIGN_TOKEN        8
#define EQUAL_TOKEN         9
#define GREATEREQUAL_TOKEN  10
#define LESSEQUAL_TOKEN     11
#define GREATER_TOKEN       12
#define LESS_TOKEN          13
#define PLUS_TOKEN          14
#define MINUS_TOKEN         15
#define LEFTPAREN_TOKEN     16
#define REFTPAREN_TOKEN     17
#define LEFTBRACE_TOKEN     18
#define REFTBRACE_TOKEN     19
#define SEMICOLON_TOKEN     20
#define END_TOKEN           0
#define ERROR_TOKEN        -1

static char lexeme[256];
static FILE* src = NULL;

static int ch_alpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int ch_digit(int c) {
    return c >= '0' && c <= '9';
}

static int ch_alnum(int c) {
    return ch_alpha(c) || ch_digit(c);
}

static int ch_space(int c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

static const char* token_type_name(int token) {
    switch (token) {
        case TYPE_TOKEN:          return "TYPE_TOKEN";
        case MAIN_TOKEN:          return "MAIN_TOKEN";
        case IF_TOKEN:            return "IF_TOKEN";
        case ELSE_TOKEN:          return "ELSE_TOKEN";
        case WHILE_TOKEN:         return "WHILE_TOKEN";
        case ID_TOKEN:            return "ID_TOKEN";
        case LITERAL_TOKEN:       return "LITERAL_TOKEN";
        case ASSIGN_TOKEN:        return "ASSIGN_TOKEN";
        case EQUAL_TOKEN:         return "EQUAL_TOKEN";
        case GREATEREQUAL_TOKEN:  return "GREATEREQUAL_TOKEN";
        case LESSEQUAL_TOKEN:     return "LESSEQUAL_TOKEN";
        case GREATER_TOKEN:       return "GREATER_TOKEN";
        case LESS_TOKEN:          return "LESS_TOKEN";
        case PLUS_TOKEN:          return "PLUS_TOKEN";
        case MINUS_TOKEN:         return "MINUS_TOKEN";
        case LEFTPAREN_TOKEN:     return "LEFTPAREN_TOKEN";
        case REFTPAREN_TOKEN:     return "REFTPAREN_TOKEN";
        case LEFTBRACE_TOKEN:     return "LEFTBRACE_TOKEN";
        case REFTBRACE_TOKEN:     return "REFTBRACE_TOKEN";
        case SEMICOLON_TOKEN:     return "SEMICOLON_TOKEN";
        default:                  return "ERROR_TOKEN";
    }
}

static int check_keyword(const char* s) {
    if (strcmp(s, "int") == 0)   return TYPE_TOKEN;
    if (strcmp(s, "main") == 0)  return MAIN_TOKEN;
    if (strcmp(s, "if") == 0)    return IF_TOKEN;
    if (strcmp(s, "else") == 0)  return ELSE_TOKEN;
    if (strcmp(s, "while") == 0) return WHILE_TOKEN;
    return ID_TOKEN;
}

static int get_next_token() {
    int c;
    int len = 0;

    while ((c = fgetc(src)) != EOF) {
        if (!ch_space(c)) break;
    }

    if (c == EOF) return END_TOKEN;

    lexeme[0] = (char)c;
    lexeme[1] = '\0';

    if (ch_alpha(c) || c == '_') {
        len = 1;
        while ((c = fgetc(src)) != EOF && (ch_alnum(c) || c == '_')) {
            if (len < 255) lexeme[len++] = (char)c;
        }
        lexeme[len] = '\0';
        if (c != EOF) ungetc(c, src);
        return check_keyword(lexeme);
    }

    if (ch_digit(c)) {
        len = 1;
        while ((c = fgetc(src)) != EOF && ch_digit(c)) {
            if (len < 255) lexeme[len++] = (char)c;
        }
        lexeme[len] = '\0';
        if (c != EOF) ungetc(c, src);
        return LITERAL_TOKEN;
    }

    if (c == '=') {
        c = fgetc(src);
        if (c == '=') {
            strcpy(lexeme, "==");
            return EQUAL_TOKEN;
        }
        if (c != EOF) ungetc(c, src);
        strcpy(lexeme, "=");
        return ASSIGN_TOKEN;
    }

    if (c == '>') {
        c = fgetc(src);
        if (c == '=') {
            strcpy(lexeme, ">=");
            return GREATEREQUAL_TOKEN;
        }
        if (c != EOF) ungetc(c, src);
        strcpy(lexeme, ">");
        return GREATER_TOKEN;
    }

    if (c == '<') {
        c = fgetc(src);
        if (c == '=') {
            strcpy(lexeme, "<=");
            return LESSEQUAL_TOKEN;
        }
        if (c != EOF) ungetc(c, src);
        strcpy(lexeme, "<");
        return LESS_TOKEN;
    }

    if (c == '+') {
        strcpy(lexeme, "+");
        return PLUS_TOKEN;
    }
    if (c == '-') {
        strcpy(lexeme, "-");
        return MINUS_TOKEN;
    }
    if (c == '(') {
        strcpy(lexeme, "(");
        return LEFTPAREN_TOKEN;
    }
    if (c == ')') {
        strcpy(lexeme, ")");
        return REFTPAREN_TOKEN;
    }
    if (c == '{') {
        strcpy(lexeme, "{");
        return LEFTBRACE_TOKEN;
    }
    if (c == '}') {
        strcpy(lexeme, "}");
        return REFTBRACE_TOKEN;
    }
    if (c == ';') {
        strcpy(lexeme, ";");
        return SEMICOLON_TOKEN;
    }

    return ERROR_TOKEN;
}

int main() {
    const char* filename = "sample.c";
    int token;
    int i;
    List* lst;

    src = fopen(filename, "r");
    if (src == NULL) {
        printf("File not found.\n");
        return 1;
    }

    lst = new_list();

    while ((token = get_next_token()) != END_TOKEN) {
        if (token == ERROR_TOKEN) {
            printf("Lexical error: %s\n", lexeme);
            fclose(src);
            free_list(lst);
            return 1;
        }

        for (i = 0; lexeme[i] != '\0'; i++) {
            if (lexeme[i] >= 32 && lexeme[i] <= 126) {
                lst = lst->addch(lst, lexeme[i]);
            }
        }

        printf("%s: %s\n", lexeme, token_type_name(token));
    }

    fclose(src);
    free_list(lst);
    return 0;
}