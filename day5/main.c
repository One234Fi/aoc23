#include <ctype.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#define INPUT "./day5/input"

typedef struct mapping {
    uint32_t dst_start;
    uint32_t src_start;
    uint32_t range_len;
} mapping;

mapping create_mapping(const uint32_t dst, const uint32_t src, const uint32_t range) {
    mapping m = {
        .dst_start = dst,
        .src_start = src,
        .range_len = range,
    };

    return m;
}

mapping* init_mapping(const uint32_t dst, const uint32_t src, const uint32_t range) {
    mapping* m = malloc(sizeof(mapping));

    m->dst_start = dst;
    m->src_start = src;
    m->range_len = range;

    return m;
}

//98 - 98 + 50 = 50
//99 - 99 + 50 = 51
//
//this should work because with valid inputs, val will always be >= src_start
uint32_t map_val(const mapping* m, const uint32_t val) {
    uint32_t mapped_val = val;
    mapped_val -= m->src_start;
    mapped_val += m->dst_start;

    return mapped_val;
}

// if num >= srcstart && num < srcstart + range
// 40 >= 15 && 40 < 15 + 37 (52) true
bool is_mappable(const mapping* m, const uint32_t val) {
    return (val >= m->src_start) && (val < (m->src_start + m->range_len));
}

//TODO: learn how to use void* for generics

typedef struct map {
    mapping* data;
    uint32_t len;
    uint32_t cap;
} map;

map* init_map(const uint32_t size) {
    map* m = malloc(sizeof(map));
    m->data = malloc(sizeof(mapping) * size);
    m->len = 0;
    m->cap = size;

    return m;
}

void free_map(map* m) {
    m->len = 0;
    m->cap = 0;
    free(m->data);
    free(m);
}

void increase_map(map** m) {
    map* temp = *m;
    *m = init_map((*m)->cap*2);
    
    memcpy((*m)->data, temp->data, sizeof(mapping) * temp->len);
    (*m)->len = temp->len;

    free_map(temp);
}

void add_mapping(map** m, const mapping* val) {
    if ((*m)->len >= (*m)->cap) {
        increase_map(m);
    }

    (*m)->data[(*m)->len].dst_start = val->dst_start;
    (*m)->data[(*m)->len].src_start = val->src_start;
    (*m)->data[(*m)->len].range_len = val->range_len;
    (*m)->len++;
}

uint32_t apply_map(map* m, const uint32_t val) {
    for (uint32_t i = 0; i < m->len; i++) {
        if (is_mappable(&m->data[i], val)) {
            return map_val(&m->data[i], val);
        }
    }

    return val;
}

//allocs to heap
char* read_line(FILE* input) {
    char chunk[1024] = {0};
    uint32_t index = 0;

    char c = getc(input);
    while (c != '\n' && c != EOF) {
        chunk[index] = c;
        index+=1;
        c = getc(input);
    }

    chunk[index] = '\0';
    index+=1;

    char* string = malloc(sizeof(char) * index);
    memcpy(string, chunk, sizeof(char) * index);
    return string;
}

void skip_line(FILE* input) {
    char c = '0';
    while ((c = getc(input)) != '\n' && c != EOF) {
    }
}

//allocs to heap
uint32_t* get_seeds(FILE* input, uint32_t* seed_count) {
    char* seed_str = read_line(input);
    char* ptr = seed_str;
    uint32_t chunk[512] = {0};
    uint32_t index = 0;

//////////////////////////////////////////////////////////////////////////////////////
//https://stackoverflow.com/questions/13399594/how-to-extract-numbers-from-string-in-c
    while (*ptr) {
        if (isdigit(*ptr)) {
            uint32_t val = strtol(ptr, &ptr, 10);
            chunk[index] = val;
            index++;
        } else {
            ptr++;
        }
    }
/////////////////////////////////////////////////////////////////////////////////////

    uint32_t* seeds = malloc(sizeof(uint32_t) * index);
    memcpy(seeds, chunk, sizeof(uint32_t) * index);
    *seed_count = index;

    free(seed_str);
    return seeds;
}

map* parse_map(FILE* input) {
    map* m = init_map(10);

    char* line = read_line(input);
    while (line[0] != '\n' && line[0] != '\0') {

        uint32_t dst = 0, src = 0, range = 0;
        char* ptr = line;
        dst = strtol(ptr, &ptr, 10);
        src = strtol(ptr, &ptr, 10);
        range = strtol(ptr, &ptr, 10);
        
        mapping* val = init_mapping(dst, src, range);
        add_mapping(&m, val);

        free(val);
        free(line);
        line = read_line(input);
    }
    free(line);

    return m;
}

void print_mapping(mapping* m) {
    printf("dst: %d, src: %d, range: %d\n", m->dst_start, m->src_start, m->range_len);
}

void print_map(map* m) {
    for (int i = 0; i < m->len; i++) {
        print_mapping(&m->data[i]);
    }
}

void part1(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        uint32_t seed_count = 0;
        uint32_t* seeds = NULL;
        seeds = get_seeds(input, &seed_count);

        skip_line(input); //blank line
        
        map* pipeline[7] = {0};
        for (int i = 0; i < 7; i++) {
            skip_line(input); //header
            pipeline[i] = parse_map(input); //parse map consumes next blank line
            //print_map(pipeline[i]);
        }

        for (int i = 0; i < seed_count; i++) {
            for (int j = 0; j < 7; j++) {
                //printf("changing seeds[%d] from %d to ", i, seeds[i]);
                seeds[i] = apply_map(pipeline[j], seeds[i]);
                //printf("%d\n", seeds[i]);
            }
        }

        uint32_t min = UINT32_MAX;
        for (int i = 0; i < seed_count; i++) {
            if (seeds[i] < min) {
                min = seeds[i];
            }
        }

        fprintf(stdout, "PART 1: The lowest location number is %d\n", min);

        free(seeds);
        for (int i = 0; i < 7; i++) {
            free_map(pipeline[i]);
        }
    } else {
        fprintf(stderr, "PART 1: Couldn't open file\n");
    }
    fclose(input);
}

struct func_args {
    map** pipeline;
    uint32_t start;
    uint32_t amount;
};

uint32_t shared_min = UINT32_MAX;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* min_mapping_from_range(void* arguments) {
    struct func_args* args = (struct func_args*) arguments;

    uint32_t min = UINT32_MAX;
    for (int i = 0; i < args->amount; i++) {
        uint32_t num = args->start + i;
        for (int j = 0; j < 7; j++) {
            num = apply_map(args->pipeline[j], num);
        }
        if (num < min) {
            min = num;
        }
    }
    
    pthread_mutex_lock(&mutex);
    if (min < shared_min) {
        shared_min = min;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void part2(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        uint32_t seed_count = 0;
        uint32_t* seed_ranges = NULL;
        seed_ranges = get_seeds(input, &seed_count);

        skip_line(input); //blank line
        
        map* pipeline[7] = {0};
        for (int i = 0; i < 7; i++) {
            skip_line(input); //header
            pipeline[i] = parse_map(input); //parse map consumes next blank line
            //print_map(pipeline[i]);
        }

        //brute force time
        uint32_t num_threads = seed_count / 2;
        pthread_t thread_id[num_threads];
        memset(thread_id, 0, sizeof(pthread_t) * num_threads);
        struct func_args* thread_args = malloc(sizeof(struct func_args) * num_threads);
        uint32_t index = 0;

        for (int i = 0; i < num_threads; i++) {
            thread_args[i].pipeline = pipeline;
            thread_args[i].start = seed_ranges[index];
            index++;
            thread_args[i].amount = seed_ranges[index];
            index++;

            pthread_create(&thread_id[i], NULL, min_mapping_from_range, 
                    (void*)&thread_args[i]);

            printf("dispatching thread %d\n", i);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(thread_id[i], NULL);
            printf("recieved thread %d\n", i);
        }

        //pthread_mutex_lock(&mutex);
        fprintf(stdout, "PART 2: The lowest location number is %d\n", shared_min);
        //pthread_mutex_unlock(&mutex);

        free(thread_args);
        free(seed_ranges);
        for (int i = 0; i < 7; i++) {
            free_map(pipeline[i]);
        }
    } else {
        fprintf(stderr, "PART 2: Couldn't open file\n");
    }
    fclose(input);
}

int main(int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }

    part1(path);
    part2(path);
}
