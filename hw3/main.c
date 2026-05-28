/*
 * HW#3 - Recursive-Descent Parser
 * Grammar:
 *   S  -> E S'
 *   S' -> + S | epsilon
 *   E  -> number | ( S )
 *
 */

#include <stdio.h>
#include <string.h>

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

static FILE *src = NULL;
static char lexeme[256];
static int token;
static int parse_error = 0;

/* ---------- HW#2 Scanner ---------- */
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

static int check_keyword(const char *s) {
    if (strcmp(s, "int") == 0)   return TYPE_TOKEN;
    if (strcmp(s, "main") == 0)  return MAIN_TOKEN;
    if (strcmp(s, "if") == 0)    return IF_TOKEN;
    if (strcmp(s, "else") == 0)  return ELSE_TOKEN;
    if (strcmp(s, "while") == 0) return WHILE_TOKEN;
    return ID_TOKEN;
}

static int get_next_token(void) {
    int c;
    int len = 0;

    while ((c = fgetc(src)) != EOF) {
        if (!ch_space(c)) break;
    }

    if (c == EOF) {
        strcpy(lexeme, "EOF");
        return END_TOKEN;
    }

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

/* ---------- Parser ---------- */
static void print_indent(int level) {
    int i;
    for (i = 0; i < level; i++) printf("  ");
}

static void advance(void) {
    token = get_next_token();
}

static void report_error(const char *msg) {
    if (!parse_error) {
        printf("Parse error: %s near '%s'\n", msg, lexeme);
    }
    parse_error = 1;
}

static void parse_S(int level);
static void parse_S_prime(int level);
static void parse_E(int level);

static void parse_S(int level) {
    if (parse_error) return;

    switch (token) {
        case LITERAL_TOKEN:
        case LEFTPAREN_TOKEN:
            print_indent(level);
            printf("S -> E S'\n");
            parse_E(level + 1);
            parse_S_prime(level + 1);
            return;
        default:
            report_error("S should start with a number or '('");
            return;
    }
}

static void parse_S_prime(int level) {
    if (parse_error) return;

    switch (token) {
        case PLUS_TOKEN:
            print_indent(level);
            printf("S' -> + S\n");
            print_indent(level + 1);
            printf("+\n");
            advance();
            parse_S(level + 1);
            return;
        case REFTPAREN_TOKEN:
        case END_TOKEN:
            print_indent(level);
            printf("S' -> epsilon\n");
            return;
        default:
            report_error("S' should be '+', ')' or EOF");
            return;
    }
}

static void parse_E(int level) {
    if (parse_error) return;

    switch (token) {
        case LITERAL_TOKEN:
            print_indent(level);
            printf("E -> num\n");
            print_indent(level + 1);
            printf("%s\n", lexeme);
            advance();
            return;

        case LEFTPAREN_TOKEN:
            print_indent(level);
            printf("E -> ( S )\n");
            print_indent(level + 1);
            printf("(\n");
            advance();
            parse_S(level + 1);
            if (parse_error) return;
            if (token != REFTPAREN_TOKEN) {
                report_error("missing ')'");
                return;
            }
            print_indent(level + 1);
            printf(")\n");
            advance();
            return;

        default:
            report_error("E should be a number or '('");
            return;
    }
}

int main(int argc, char *argv[]) {
    const char *filename = "sample.txt";

    if (argc > 1) {
        filename = argv[1];
    }

    src = fopen(filename, "r");
    if (src == NULL) {
        printf("File not found: %s\n", filename);
        return 1;
    }

    advance();

    if (token == ERROR_TOKEN) {
        printf("Lexical error: %s\n", lexeme);
        fclose(src);
        return 1;
    }

    printf("Parse Tree:\n");
    parse_S(0);

    if (!parse_error && token != END_TOKEN) {
        report_error("extra token after a complete expression");
    }

    fclose(src);

    if (parse_error) {
        printf("Invalid input.\n");
        return 1;
    }

    printf("Accepted.\n");
    return 0;
}
