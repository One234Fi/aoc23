#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "./day9/test"

char* readLine(FILE* file, bool* eof) {
    char buf[1024] = {0};
    int index = 0;
    char c;
    while ((c = getc(file)) != '\n' && c != EOF) {
        buf[index] = c;
        index++;
    }

    buf[index] = '\0';
    index++;

    char* string = malloc(sizeof(char) * index);
    memcpy(string, buf, sizeof(char) * index);

    if (c == EOF) {
        *eof = true;
    }

    return string;
}

typedef struct arr {
    int64_t* data;
    uint32_t len;
} arr;

arr* init_arr(int64_t* data, uint32_t size) {
    arr* a = malloc(sizeof(arr));
    a->data = malloc(sizeof(uint64_t) * size);

    a->len = size;
    if (data) {
        memcpy(a->data, data, sizeof(int64_t) * size);
    } else {
        memset(a->data, 0, sizeof(int64_t) * size);
    }

    return a;
}

void free_arr(arr* a) {
    free(a->data);
    free(a);
}

void print_arr(arr* a) {
    for (int i = 0; i < a->len; i++) {
        printf("%ld, ", a->data[i]);
    }
    printf("\n");
}

arr* parseLine(char* line) {
    int index = 0;
    int64_t buf[512];
    char* ptr = line;

    while (*ptr) {
        if (isdigit(*ptr) || (*ptr=='-' && isdigit(*(ptr+1)))) {
            buf[index] = strtol(ptr, &ptr, 10);
            index++;
        } else {
            ptr++;
        }
    }

    arr* data = init_arr(buf, index);
    return data;
}

bool is_zeroed(arr* arr) {
    for (int i = 0; i < arr->len; i++) {
        if (arr->data[i] != 0) {
            return false;
        }
    }
    
    return true;
}

int64_t get_next_val(arr a) {
    if (is_zeroed(&a)) {
        return 0;
    }

    int64_t data[a.len-1];
    memset(data, 0, sizeof(int) * (a.len-1));
    for (int i = 0; i < a.len-1; i++) {
        data[i] = a.data[i+1] - a.data[i];
    }
    arr next = {
        .data = data,
        .len = (a.len-1),
    };

    return a.data[a.len-1] + get_next_val(next);
}

int64_t get_prev_val(arr a) {
    if (is_zeroed(&a)) {
        return 0;
    }

    int64_t data[a.len-1];
    memset(data, 0, sizeof(int) * (a.len-1));
    for (int i = 0; i < a.len-1; i++) {
        data[i] = a.data[i+1] - a.data[i];
    }
    arr next = {
        .data = data,
        .len = (a.len-1),
    };

    return a.data[0] - get_prev_val(next);
}

int main(int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }

    FILE* input = fopen(path, "r");
    if (input) {
        bool eof = false;
        int64_t sum = 0;
        int64_t sum2 = 0;
        uint32_t amount = 0;
        while (!eof) {
            amount++;
            char* line = readLine(input, &eof);
            if (!eof) {
                arr* a = parseLine(line);

                int64_t add = get_next_val(*a);
                int64_t add2 = get_prev_val(*a);
                sum += add;
                sum2 += add2;

                free_arr(a);
            }
            free(line);
        }
        fprintf(stdout, "Part 1: The sum of the forward predictions is %ld\n", sum);
        fprintf(stdout, "Part 2: The sum of the backward predictions is %ld\n", sum2);
    } else {
        fprintf(stderr, "Couldn't open file\n");
    }
    fclose(input);
}
