#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "./day6/test"

//allocs to heap
char* readLine(FILE* file) {
    char buf[254] = {0};
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
    return string;
}

char* readLineIgnoreSpaces(FILE* file) {
    char buf[254] = {0};
    int index = 0;
    char c;
    while ((c = getc(file)) != '\n' && c != EOF) {
        if (!isblank(c)) {
            buf[index] = c;
            index++;
        }
    }

    buf[index] = '\0';
    index++;

    char* string = malloc(sizeof(char) * index);
    memcpy(string, buf, sizeof(char) * index);
    return string;
}

//I guess technically not a vec because it won't grow
typedef struct arr {
    uint64_t* data;
    uint32_t len;
} arr;

arr* init_arr(uint32_t size) {
    arr* a = malloc(sizeof(arr));
    a->data = malloc(sizeof(uint64_t) * size);
    a->len = size;
    return a;
}

void free_arr(arr* a) {
    free(a->data);
    free(a);
}

//allocs to heap
arr* parseInts(char* string) {
    char* ptr = string;
    uint64_t buf[16] = {0};
    uint64_t index = 0;
    while (*ptr) {
        if (isdigit(*ptr)) {
            buf[index] = strtoul(ptr, &ptr, 10);
            index++;
        } else {
            ptr++;
        }
    }
    arr* a = init_arr(index);
    memcpy(a->data, buf, sizeof(uint64_t) * index);
    return a;
}

uint64_t get_distance(uint64_t time, uint64_t hold) {
    return (time - hold) * hold;
}

uint64_t calc_margin(uint64_t time, uint64_t record) {
    uint64_t sum = 0;
    //start at max distance and go down
    uint64_t time_index = time/2;
    uint64_t distance = get_distance(time, time_index);
    
    while (distance > record) {
        sum++;
        time_index--;
        distance = get_distance(time, time_index);
    }

    sum *= 2;
    if (time % 2 == 0) {
        sum -=1;
    }
    return sum;
}

void part1(char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        char* line1 = readLine(input);
        char* line2 = readLine(input);
        arr* times = parseInts(line1);
        arr* distances = parseInts(line2);


        uint32_t sum = 1;
        //uint32_t margins[times->len];
        for (uint32_t i = 0; i < times->len; i++) {
            //margins[i] = calc_margin(times->data[i], distances->data[i]);
            sum *= calc_margin(times->data[i], distances->data[i]);
        }

        fprintf(stdout, "Part 1: The product of the margins is %d\n", sum);

        free(line1);
        free(line2);
        free_arr(times);
        free_arr(distances);
    } else {
        fprintf(stderr, "Part 1: Couldn't open file\n");
    }
}

void part2(char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        char* line1 = readLineIgnoreSpaces(input);
        char* line2 = readLineIgnoreSpaces(input);
        arr* times = parseInts(line1);
        arr* distances = parseInts(line2);

        uint64_t time = times->data[0];
        uint64_t distance = distances->data[0];
        printf("time: %lu\nrecord %lu\n", time, distance);
        uint64_t margin = calc_margin(time, distance);

        fprintf(stdout, "Part 2: The margin is %lu\n", margin);

        free(line1);
        free(line2);
        free_arr(times);
        free_arr(distances);
    } else {
        fprintf(stderr, "Part 2: Couldn't open file\n");
    }
}

int main(int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }

    part1(path);
    part2(path);
}
