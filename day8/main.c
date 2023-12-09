#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "./day8/test"

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

typedef struct fork {
    char id[3];
    char left[3];
    char right[3];
} fork;

//return whether two fork ids are the same
bool forks_equal(char* fork1, char* fork2) {
    for (uint32_t i = 0; i < 3; i++) {
        if (fork1[i] != fork2[i]) {
            return false;
        }
    }

    return true;
}

typedef struct entry {
    bool has_val;
    fork val;
} entry;

typedef struct hashmap {
    entry* data;
    uint32_t size;
    uint32_t cap;
} hashmap;

uint32_t hash(uint32_t val, uint32_t seed) {
    return val % seed;
}

void insert(hashmap* map, fork f);

hashmap* init_hashmap(uint32_t size, hashmap* old_map) {
    hashmap* h = malloc(sizeof(hashmap));
    h->data = malloc(sizeof(entry) * size);
    memset(h->data, 0, sizeof(entry) * size);
    h->cap = size;
    h->size = 0;

    if (old_map) {
        // this doesn't work correctly
        for (uint32_t i = 0; i < old_map->size; i++) {
            if (old_map->data[i].has_val) {
                insert(h, old_map->data[i].val);
            }
        }
    }
    return h;
}

hashmap* new_map() {
    return init_hashmap(1024, NULL);
}

void free_hashmap(hashmap* map) {
    free(map->data);
    free(map);
}

void resize_map(hashmap** map) {
    hashmap* temp = NULL;
    temp = init_hashmap((*map)->cap * 2, *map);
    free_hashmap(*map);
    (*map) = temp;
}

uint32_t get_key(hashmap* map, char* id) {
    uint32_t n = 0; 
    for (uint32_t i = 0; i < 3; i++) {
        n += id[i];
    }
    uint32_t key = hash(n, map->cap);
    return key;
}

void insert(hashmap* map, fork f) {
    if (map->size == map->cap) {
        resize_map(&map);
    }

    uint32_t key = get_key(map, f.id);
    while(map->data[key].has_val) {
        key++;
    }

    map->data[key].val = f;
    map->data[key].has_val = true;
}

bool has_value(hashmap* map, char* id) {
    uint32_t key = get_key(map, id);
    uint32_t stop_inf_loop = key;

    while (map->data[key].has_val) {
        if (forks_equal(map->data[key].val.id, id))  {
            return true;
        }
        key += 1 % map->cap;
        if (key == stop_inf_loop) {
            return false;
        }
    }

    //val wasn't found
    return false;
}

fork get_value(hashmap* map, char* id) {
    uint32_t key = get_key(map, id);
    uint32_t stop_inf_loop = key;
    fork f = {{0}, {0}, {0}};

    while (map->data[key].has_val) {
        if (forks_equal(map->data[key].val.id, id))  {
            f = map->data[key].val;
            break;
        }
        key += 1 % map->cap;
        if (key == stop_inf_loop) {
            break;
        }
    }

    return f;
}

void print_fork(fork* f) {
    printf("id: ");
    for (uint32_t i = 0; i < 3; i++) {
        printf("%c", f->id[i]);
    }
    printf("\nl: ");
    for (uint32_t i = 0; i < 3; i++) {
        printf("%c", f->left[i]);
    }
    printf("\nr: ");
    for (uint32_t i = 0; i < 3; i++) {
        printf("%c", f->right[i]);
    }
    printf("\n");
}

fork parse_fork(char* line) {
    fork f = {
        .id = {line[0], line[1], line[2]}, 
        .left = {line[7], line[8], line[9]}, 
        .right = {line[12], line[13], line[14]}};

    //print_fork(&f);
    return f;
}

void part1(const char* path) { 
    FILE* input = fopen(path, "r");

    if (input) {
        bool eof = false;
        char* lines[1024] = {0};
        uint32_t numLines = 0;

        //get directions
        char* directions = readLine(input, NULL);

        //discard empty line
        char* blank = readLine(input, NULL);
        free(blank);

        hashmap* map = new_map();

        //get paths
        while (!eof) {
            lines[numLines] = readLine(input, &eof);
            
            if (!eof) {
                insert(map, parse_fork(lines[numLines]));
                numLines++;
            }
        }

        uint32_t sum = 0;
        uint32_t direction_index = 0;
        char begin[3] = {'A', 'A', 'A'};
        char dest[3] = {'Z', 'Z', 'Z'};
        char empty[3] = {0, 0, 0};

        fork current = get_value(map, begin);
        while (!forks_equal(current.id, dest)) {
            //printf("dir: %d \n", directions[direction_index]);
            //print_fork(&current);
            switch (directions[direction_index]) {
                case 'L':
                    current = get_value(map, current.left);
                    break;

                case 'R':
                    current = get_value(map, current.right);
                    break;

                default:
                    fprintf(stderr, "PART 1: something went wrong\n");
            }
            if (forks_equal(current.id, empty)) {
                fprintf(stderr, "PART 1: recieved an empty value\n");
                break;
            }
            sum++;

            if (directions[++direction_index] == '\0') {
                direction_index = 0;
            }
        }

        printf("PART 1: It took %d steps to reach ZZZ\n", sum);

        free_hashmap(map);
        free(directions);
        for (uint32_t i = 0; i < numLines; i++) {
            free(lines[i]);
        }
        free(lines[numLines]);
    } else {
        fprintf(stderr, "PART 1: Couldn't open file\n");
    }

    fclose(input);
}

bool at_end(fork state) {
    return state.id[2] == 'Z';
}

//least common multiple
uint64_t LCM(uint32_t* nums, uint32_t count) {
    uint64_t copy[count];
    for (uint32_t i = 0; i < count; i++) {
        copy[i] = nums[i];
    }

    bool finished = false;
    while (!finished) {
        bool nums_equal = true;
        for (uint32_t i = 1; i < count; i++) {
            if (copy[0] != copy[i]) {
                nums_equal = false;
            }
        }
        if (nums_equal) {
            finished = true;
            break;
        }

        uint64_t min = copy[0];
        uint32_t index = 0;
        for (uint32_t i = 1; i < count; i++) {
            if (copy[i] < min) {
                min = copy[i];
                index = i;
            }
        }

        copy[index] += nums[index];
    }

    return copy[0];
}

void part2(const char* path) { 
    FILE* input = fopen(path, "r");

    if (input) {
        bool eof = false;
        char* lines[1024] = {0};
        uint32_t numLines = 0;

        //get directions
        char* directions = readLine(input, NULL);

        //discard empty line
        char* blank = readLine(input, NULL);
        free(blank);

        hashmap* map = new_map();

        fork start_forks[254] = {0};
        uint32_t fork_count = 0;

        while (!eof) {
            lines[numLines] = readLine(input, &eof);

            if (!eof) {
                fork f = parse_fork(lines[numLines]);
                if (f.id[2] == 'A') {
                    start_forks[fork_count] = f;
                    fork_count++;
                }
                insert(map, f);
                numLines++;
            }
        }

        uint32_t sum[fork_count];;
        memset(sum, 0, sizeof(uint32_t) * fork_count);
        uint32_t direction_index = 0;
        char empty[3] = {0, 0, 0};

        printf("starting loop with %d forks\n", fork_count);
        for (uint32_t i = 0; i < fork_count; i++) {
            while (!at_end(start_forks[i])) {
                //printf("index: %d \n", i);
                //print_fork(&start_forks[i]);
                switch (directions[direction_index]) {
                    case 'L':
                        start_forks[i] = get_value(map, start_forks[i].left);
                        break;

                    case 'R':
                        start_forks[i] = get_value(map, start_forks[i].right);
                        break;

                    default:
                        fprintf(stderr, "PART 2: something went wrong\n");
                }
                if (forks_equal(start_forks[i].id, empty)) {
                    fprintf(stderr, "PART 2: recieved an empty value\n");
                    break;
                }
                sum[i]++;

                if (directions[++direction_index] == '\0') {
                    direction_index = 0;
                    //printf("reset directions... sum is %d\n", sum);
                }
            }
            printf("PART 2: It took %d steps to reach the end\n", sum[i]);
        }
        printf("PART 2: The final answer is %lu\n", LCM(sum, fork_count));

        free_hashmap(map);
        free(directions);
        for (uint32_t i = 0; i < numLines; i++) {
            free(lines[i]);
        }
        free(lines[numLines]);
    } else {
        fprintf(stderr, "PART 2: Couldn't open file\n");
    }

fclose(input);
}

int main (int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }

    part1(path);
    part2(path);
}
