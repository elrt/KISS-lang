/*
  * KISS Language Interpreter (Keep It Simple Syntax)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_MAX 256
#define INCLUDE_DIRECTIVE "#include"

int run_file(const char *filename, int *x);
 
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.kiss>\n", argv[0]);
        return 1;
    }

    int x = 0;
    int result = run_file(argv[1], &x);
    if (result != 0) {
        fprintf(stderr, "Error: Failed to run the program.\n");
       return 1;
    }

    return 0;
}
static char *ltrim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    return s;
}

int run_file(const char *filename, int *x) {
   FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return 1;
    }

    char line[LINE_MAX];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        char *ptr = ltrim(line);

        if (*ptr == '\0' || *ptr == '\n')
            continue;

        if (*ptr == '#') {
            if (strncmp(ptr, INCLUDE_DIRECTIVE, strlen(INCLUDE_DIRECTIVE)) == 0) {
                char include_file[LINE_MAX];
                if (sscanf(ptr + strlen(INCLUDE_DIRECTIVE), " \"%255[^\"]\"", include_file) != 1) {
                    fprintf(stderr, "%s:%d: Invalid include directive syntax.\n", filename, line_num);
                    fclose(fp);
                     return 1;
                }
                if (run_file(include_file, x) != 0) {
                    fclose(fp);
                    return 1;
                }
                continue;
            }
 
            continue;
        }  
        if (strncmp(ptr, "CP", 2) == 0) {
            putchar((char)(*x));
            fflush(stdout);
            continue;
        }
        char cmd;
        int num_args = 0;
        num_args = sscanf(ptr, " %c", &cmd);
        if (num_args < 1) {
            continue;
        }

        switch (cmd) {
            case 'A': {
                char *p = ptr + 1;
                p = ltrim(p);
                if (*p == '\'') {
                    if (*(p+2) != '\'') {
                        fprintf(stderr, "%s:%d: Invalid char literal syntax.\n", filename, line_num);
                        fclose(fp);
                        return 1;
                    }
                    *x = (int)*(p+1);
                } else {
                    int val;
                    if (sscanf(p, "%d", &val) != 1) {
                        fprintf(stderr, "%s:%d: Invalid number for assignment.\n", filename, line_num);
                        fclose(fp);
                        return 1;
                    }
                    *x = val;
                }
                break;
            }
            case 'I': {
                int val;
                if (sscanf(ptr + 1, "%d", &val) != 1) {
                    fprintf(stderr, "%s:%d: Invalid number for increment.\n", filename, line_num);
                    fclose(fp);
                    return 1;
                }
                *x += val;
                break;
            }
            case 'D': {
                int val;
                if (sscanf(ptr + 1, "%d", &val) != 1) {
                    fprintf(stderr, "%s:%d: Invalid number for decrement.\n", filename, line_num);
                    fclose(fp);
                    return 1;
                }
                *x -= val;
                break;
            }
            case 'P': {
                printf("%d\n", *x);
                break;
            }

            default:
                fprintf(stderr, "%s:%d: Unknown command '%c'.\n", filename, line_num, cmd);
                fclose(fp);
                return 1;
        }
    }

    fclose(fp);
    return 0;
}


