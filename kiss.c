//By elrt(github) and elliktronic(discord)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
#define LINE_MAX 256
#define INCLUDE_DIRECTIVE "#include"
#define MAX_LABELS 100

typedef struct {
    char name[LINE_MAX];
    long file_pos;
    int line_num;
}  Label;

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

static char *rtrim(char *s) {
    char *back = s + strlen(s);
    while (back > s && isspace((unsigned char)*(back-1))) back--;
    *back = '\0';
    return s;
}

int run_file(const char *filename, int *x) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return 1;
    }

    // First pass: collect all labels
    Label labels[MAX_LABELS];
    int label_count = 0;
    char line[LINE_MAX];
    int line_num = 0;
    long file_pos = 0;

    while (fgets(line, sizeof(line), fp)) {
       line_num++;
        char *ptr = ltrim(line);
        rtrim(ptr);

        if (*ptr == ':') {
            if (label_count >= MAX_LABELS) {
                fprintf(stderr, "Error: Too many labels (max %d)\n", MAX_LABELS);
                fclose(fp);
                return 1;
            }

            char *label_name = ptr + 1;
            while (*label_name && isspace((unsigned char)*label_name)) label_name++;
            rtrim(label_name);

            if (*label_name == '\0') {
                fprintf(stderr, "%s:%d: Empty label name\n", filename, line_num);
                fclose(fp);
                return 1;
            }

            strncpy(labels[label_count].name, label_name, LINE_MAX-1);
            labels[label_count].file_pos = file_pos;
            labels[label_count].line_num = line_num;
            label_count++;
        }

        file_pos = ftell(fp);
    }

    // Second pass: execute the program
    rewind(fp);
    line_num = 0;
    int y = 0;

    while (fgets(line, sizeof(line), fp)) {
       line_num++;
       char *ptr = ltrim(line);
       rtrim(ptr);

       if (*ptr == '\0') continue;


       if (*ptr == ':') continue;

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
            }
            continue;
        }

        if (strncmp(ptr, "CP", 2) == 0) {
            putchar(*x);
            fflush(stdout);
            continue;
        }

        if (strncmp(ptr, "CI", 2) == 0) {
           *x = getchar();
            continue;
        }

        char cmd;
        if (sscanf(ptr, " %c", &cmd) != 1) continue;

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
            case 'I': *x += atoi(ptr + 1); break;
            case 'D': *x -= atoi(ptr + 1); break;
            case 'P': printf("%d\n", *x); break;
            case 'C': *x = 0; break;
            case 'X': *x ^= atoi(ptr + 1); break;
            case 'M': *x *= atoi(ptr + 1); break;
            case 'S': { int t = *x; *x = atoi(ptr + 1); y = t; break; }
            case 'N': *x = -(*x); break;
            case 'B': *x = ~(*x); break;
            case 'Y': y = *x; break;
            case 'V': *x = y; break;
            case 'E': {
                int val = atoi(ptr + 1);
                if (*x != val) {
                    fgets(line, sizeof(line), fp);
                    line_num++;
                }
                break;
            }
            case 'L': {
                int val = atoi(ptr + 1);
                if (*x >= val) {
                    fgets(line, sizeof(line), fp);
                    line_num++;
                }
                break;
            }
            case 'G': {
                char target[LINE_MAX];
                if (sscanf(ptr + 1, "%255s", target) != 1) break;
                
                int found = 0;
                for (int i = 0; i < label_count; i++) {
                    if (strcmp(labels[i].name, target) == 0) {
                        fseek(fp, labels[i].file_pos, SEEK_SET);
                        line_num = labels[i].line_num - 1; // -1 because we'll increment in next iteration
                        found = 1;
                        break;
                    }
                }
                
                if (!found) {
                    fprintf(stderr, "%s:%d: Label '%s' not found\n", filename, line_num, target);
                    fclose(fp);
                    return 1;
                }
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
