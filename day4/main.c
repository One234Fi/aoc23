#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INPUT "./day4/input"

//high quality hash function
int hash(int data, int seed) {
    return data % seed;
}

typedef struct bucket {
    int data;
    struct bucket* next;
} bucket;

bucket* init_bucket(int val) {
    bucket* b = malloc(sizeof(bucket));
    b->data = val;
    b->next = NULL;
    return b;
}

void free_bucket(bucket* b) {
    bucket* ptr = NULL;
    while (b != NULL) {
        ptr = b;
        b = b->next;
        ptr->next = NULL;
        free(ptr);
    }
}

void append(bucket* b, int val) {
    bucket* new_bucket = init_bucket(val);

    //printf("appending %d, (%d)\n", val, new_bucket->data);

    if (b->next == NULL) {
        b->next = new_bucket;
        return;
    }

    while (b->next != NULL) {
        b = b->next;
    }
    b->next = new_bucket;
}

bool check_bucket(bucket* b, int val) {
    //printf("looking for %d\n", val);

    //dummy node
    if (b->next == NULL) {
        //printf("empty list\n");
        return false;
    }

    bucket* ptr = b;
    while (ptr != NULL) {
        //printf("check %d\n", ptr->data);
        if (ptr->data == val) {
            //printf("FOUND %d\n", ptr->data);
            return true;
        }
        ptr = ptr->next;
    }

    return false;
}

void print_bucket(bucket* b) {
    bucket* temp = b;
    while (temp != NULL) {
        printf("%d, ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

// could probably just get away with direct entry into a large 
// sparsely-populated array because of small sample-size, but this is more fun
typedef struct set {
    bucket* buckets;
    uint32_t capacity;
} set;

set* init_set(uint32_t size) {
    set* s = malloc(sizeof(set));
    s->buckets = malloc(sizeof(bucket) * size);
    s->capacity = size;

    for (int i = 0; i < s->capacity; i++) {
        s->buckets[i] = *init_bucket(0);
    }

    return s;
}

void put(set* s, int val) {
    int index = hash(val, s->capacity);
    append(&s->buckets[index], val);
}

bool contains(set* s, int val) {
    int index = hash(val, s->capacity);
    return check_bucket(&s->buckets[index], val);
}

void print_set(set* s) {
    for (int i = 0; i < s->capacity; i++) {
        print_bucket(&s->buckets[i]);
    }
}

void free_set(set* s) {
    for (int i = 0; i < s->capacity; i++) {
        free_bucket(s->buckets[i].next);
    }

    //free heads
    free(s->buckets);
    s->capacity = 0;
    free(s);
}

void load_winning_nums(FILE* input, set* s) {
    char c = '.';

    //skip "Card %: "
    while ((c = getc(input)) != ':' && c != EOF) {
    }

    //parse winning nums
    while ((c = getc(input)) != '|' && c != EOF) {
        if (isblank(c)) {
            continue;
        } else if (isdigit(c)) {
            char digits[10] = {'.'};
            int index = 0;
            while(isdigit(c)) {
                digits[index] = c;
                index+=1;
                c = getc(input);
            }

            int num = strtol(digits, NULL, 10);
            //printf("%d\n", num);
            put(s, num);
        }
    }
}


bool get_score(FILE* input, set* s, int* out) {
    int matches = 0;
    char c = 0;

    while (c != '\n' && c != EOF) {
        c = getc(input);
        if (isblank(c)) {
            continue;
        } else if (isdigit(c)) {
            char digits[10] = {'.'};
            int index = 0;
            while(isdigit(c) && c != '\n') {
                digits[index] = c;
                index+=1;
                c = getc(input);
            }

            int num = 0;
            num = strtol(digits, NULL, 10);
            if (contains(s, num)) {
                matches += 1;
            }
        }
    }

    if (matches != 0) {
        int score = 1;
        for (int i = 1; i < matches; i++) {
            score *= 2;
        }
        *out = score;
    } else {
        *out = 0;
    }
    
    return c != EOF;
}

// return false when EOF reached, send points to out
bool parseLine(FILE* input, int* out) {
    set* winning_nums = init_set(10);

    load_winning_nums(input, winning_nums);
    //print_set(winning_nums);

    bool is_eof = get_score(input, winning_nums, out);
    free_set(winning_nums);

    return is_eof;
}

void part1(char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        int sum = 0;
        int temp = 0;

        while (parseLine(input, &temp)) {
            //printf("plus %d points\n", temp);
            sum += temp;
        }

        fprintf(stdout, "PART 1: the sum of the scratchcards is %d\n", sum);
    } else {
        fprintf(stderr, "Couldn't open input file\n");
    }
    fclose(input);
}

typedef struct card {
    set* winning_nums;
    char* line;
    int amount;
} card;

//assume line is malloc'd
card* init_card(set* s, char* line) {
    card* c = malloc(sizeof(card));
    c->winning_nums = s;
    c->line = line;
    c->amount = 1;
    return c;
}

void free_card(card* c) {
    free_set(c->winning_nums);
    free(c->line);
    free(c);
}

//this is kinda messy
void get_winning_nums(card* card) {
    int index = 0;
    char c = '.';

    //skip "Card %: "
    while (c != ':') {
        c = card->line[index];
        index++;
    }

    while (c != '|') {
        c = card->line[index];
        if (isblank(c)) {
            index++;
            continue;
        } else if (isdigit(c)) {
            char digits [10] = {'.'};
            int digit_index = 0;
            while(isdigit(c)) {
                digits[digit_index] = c;
                digit_index++;
                index++;
                c = card->line[index];
            }

            int num = strtol(&digits[0], NULL, 10);
            put(card->winning_nums, num);
        }
    }
}

int calc_wins(card* card) {
    //print_set(card->winning_nums);

    int matches = 0;

    int index = 0;
    char c = '.';

    while (c != '|') {
        c = card->line[index];
        index++;
    }

    while (c != '\0') {
        c = card->line[index];
        if (isblank(c)) {
            index++;
            continue;
        } else if (isdigit(c)) {
            char digits[10] = {'.'};
            int digits_index = 0;
            while(isdigit(c) && c != '\0') {
                digits[digits_index] = c;
                digits_index += 1;
                index += 1;
                c = card->line[index];
            }

            int num = 0;
            num = strtol(digits, NULL, 10);
            if (contains(card->winning_nums, num)) {
                matches += 1;
            }
        }
    }
    
    return matches;
}

int count_wins_and_copy_forward(card** cards, int index) {
    int wins = calc_wins(cards[index]);

    //printf("index %d won %d\n", index+1, wins);
    for (int i = 1; i <= wins; i++) {
        cards[index + i]->amount += cards[index]->amount;
    }

    return cards[index]->amount;
}

void part2(char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        int cardNum = 0;
        card* cards[512];
        char c = 0;
        while ((c = getc(input)) != EOF) {
            char buffer[254] = {0};
            int buflen = 0;
            while ((c = getc(input)) != '\n' && c != EOF) {
                buffer[buflen] = c;
                buflen +=1;
            }
            buffer[buflen] = '\0';
            buflen+=1;
            char* string = malloc(sizeof(char) * buflen);
            memcpy(string, buffer, sizeof(char) * buflen);

            set* s = init_set(10);
            cards[cardNum] = init_card(s, string);
            get_winning_nums(cards[cardNum]);
            cardNum++;
        }
        int sum = 0;

        for (int i = 0; i < cardNum; i++) {
            sum += count_wins_and_copy_forward(cards, i);
        }

        fprintf(stdout, "PART 2: the total number of scratchcards is %d\n", sum);

        for (int i = 0; i < cardNum; i++) {
            free_card(cards[i]);
        }
    } else {
        fprintf(stderr, "Couldn't open input file\n");
    }
    fclose(input);
}

int main(int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }
    
    //part1(path);
    part2(path);
}
