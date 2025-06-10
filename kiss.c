//elliktronic(or elrt)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#define KISS_LINE_MAX 256
#define INCLUDE_DIRECTIVE "#include"
#define MAX_LABELS 100
#define MAX_INCLUDE_DEPTH 10
#define MEMORY_SIZE 1024

typedef struct {
char name[KISS_LINE_MAX];
long file_pos;
int line_num;
} Label;

int run_file(const char *filename, int *x, int *y, int depth, int *memory);

static char *ltrim(char *s) {
while (isspace((unsigned char)*s)) s++;
return s;
}

static char *rtrim(char *s) {
char *back = s + strlen(s);
if (back > s) back--;
while (back > s && isspace((unsigned char)*back)) back--;
if ( !isspace((unsigned char)*back)) back++;
*back = '\0';
return s;
}

static int safe_strtol(const char *str, long *val) {
char *end;
errno = 0;
*val = strtol(str, &end, 10);
return (errno == 0 && end != str && *end == '\0');
}

int main(int argc, char *argv[]) {
if (argc != 2) {
fprintf(stderr, "Usage: %s <source.kiss>\n", argv[0]);
return 1;
}

int x = 0;

int y = 0;
int memory[MEMORY_SIZE] = {0};
srand(time(NULL));
return run_file(argv[1], &x, &y, 0, memory) ? 1 : 0;
}
int run_file(const char *filename, int *x, int *y, int depth, int *memory) {
if (depth > MAX_INCLUDE_DEPTH) {
fprintf(stderr, "Error: Include depth exceeded (max %d)\n", MAX_INCLUDE_DEPTH);
return 1;
}
FILE *fp = fopen(filename, "r");
if (!fp) {
    fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
    return 1;
}
Label labels[MAX_LABELS];
int label_count = 0;
char line[KISS_LINE_MAX];
int line_num = 0;
while (fgets(line, sizeof(line), fp)) {
    line_num++;
    char *comment_start = strchr(line, '#');
    if (comment_start) *comment_start = '\0';
    char *ptr = ltrim(line);
    rtrim(ptr);
    if (*ptr == '\0') continue;
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
        strncpy(labels[label_count].name, label_name, sizeof(labels[0].name) - 1);
        labels[label_count].name[sizeof(labels[0].name) - 1] = '\0';
        long file_pos = ftell(fp);
        if (file_pos == -1) {
            fprintf(stderr, "%s:%d: Failed to get file position\n", filename, line_num);
            fclose(fp);
            return 1;
        }
        labels[label_count].file_pos = file_pos;
        labels[label_count].line_num = line_num;
        label_count++;
    }
}
rewind(fp);
line_num = 0;
while (fgets(line, sizeof(line), fp)) {
    line_num++;
    char *comment_start = strchr(line, '#');
    if (comment_start) *comment_start = '\0';
    char *ptr = ltrim(line);
    rtrim(ptr);
    if (*ptr == '\0' || *ptr == ':') continue;
    if (strncmp(ptr, INCLUDE_DIRECTIVE, strlen(INCLUDE_DIRECTIVE)) == 0) {
        char include_file[KISS_LINE_MAX];
        char *start = ptr + strlen(INCLUDE_DIRECTIVE);
        while (*start && *start != '"') start++;
        if (*start != '"' || sscanf(start, "\"%255[^\"]\"", include_file) != 1) {
            fprintf(stderr, "%s:%d: Invalid include directive\n", filename, line_num);
            fclose(fp);
            return 1;
        }
        if (run_file(include_file, x, y, depth + 1, memory) != 0) {
            fclose(fp);
            return 1;
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
                    fprintf(stderr, "%s:%d: Invalid char literal\n", filename, line_num);
                    fclose(fp);
                    return 1;
                }
                *x = (int)*(p+1);
            } else {
                long val;
                if (!safe_strtol(p, &val)) {
                    fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                    fclose(fp);
                    return 1;
                }
                *x = (int)val;
            }
            break;
        }
        case 'I':
        case 'D':
        case 'X':
        case 'M': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (cmd == 'I') *x += val;
            else if (cmd == 'D') *x -= val;
            else if (cmd == 'X') *x ^= val;
            else *x *= val;
            break;
        }
        case 'P': printf("%d\n", *x); break;
        case 'C': *x = 0; break;
        case 'S': {
            int t = *x;
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x = (int)val;
            *y = t;
            break;
        }
        case 'N': *x = -(*x); break;
        case 'B': *x = ~(*x); break;
        case 'Y': *y = *x; break;
        case 'V': *x = *y; break;
        case 'E': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (*x != val) {
                if (!fgets(line, sizeof(line), fp)) break;
                line_num++;
            }
            break;
        }
        case 'L': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (*x >= val) {
                if (!fgets(line, sizeof(line), fp)) break;
                line_num++;
            }
            break;
        }
        case 'G': {
            char target[KISS_LINE_MAX];
            char *start = ptr + 1;
            while (*start && isspace((unsigned char)*start)) start++;
            char *end = start;
            while (*end && !isspace((unsigned char)*end)) end++;
            if (end == start) break;
            strncpy(target, start, end - start);
            target[end - start] = '\0';
            int found = 0;
            for (int i = 0; i < label_count; i++) {
                if (strcmp(labels[i].name, target) == 0) {
                    if (fseek(fp, labels[i].file_pos, SEEK_SET) != 0) {
                        fprintf(stderr, "%s:%d: Seek failed\n", filename, line_num);
                        fclose(fp);
                        return 1;
                    }
                    line_num = labels[i].line_num - 1;
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
        case 'R': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (val == 0) {
                fprintf(stderr, "%s:%d: Division by zero\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x %= val;
            break;
        }
        case 'Q': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (val == 0) {
                fprintf(stderr, "%s:%d: Division by zero\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x /= val;
            break;
        }
        case '^': {
            *x = *x * *x;
            break;
        }
        case 't': {
            if (*x < 0) {
                fprintf(stderr, "%s:%d: Square root of negative number\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x = (int)sqrt((double)*x);
            break;
        }
        case '>': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid memory address\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (val < 0 || val >= MEMORY_SIZE) {
                fprintf(stderr, "%s:%d: Memory address out of bounds\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            memory[(int)val] = *x;
            break;
        }
        case '<': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid memory address\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (val < 0 || val >= MEMORY_SIZE) {
                fprintf(stderr, "%s:%d: Memory address out of bounds\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x = memory[(int)val];
            break;
        }
        case '?': {
            long val;
            if (!safe_strtol(ptr + 1, &val)) {
                fprintf(stderr, "%s:%d: Invalid number for random range\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            if (val <= 0) {
                fprintf(stderr, "%s:%d: Random range must be positive\n", filename, line_num);
                fclose(fp);
                return 1;
            }
            *x = rand() % (int)val;
            break;
        }
        default:
            fprintf(stderr, "%s:%d: Unknown command '%c'\n", filename, line_num, cmd);
            fclose(fp);
            return 1;
    }
}
fclose(fp);
return 0;
}
