#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define INPUT "./day3/input"

//Note to self. Don't code late at night or this file happens

//Load data into a 2D array,
//  iterate
//      if number:
//          if checkbounds
//              parseNumber and add to sum
//          else
//              continue


//quick and dirty vector and matrix (vector of vectors) impls
struct vector {
    char* data;
    uint32_t len;
    uint32_t cap;
};

void init_vec(struct vector* vec) {
    vec->data = malloc(256);
    vec->len = 0;
    vec->cap = 256;
}

void free_vec(struct vector* vec) {
    free(vec->data);
}

void increase_cap(struct vector* vec) {
    struct vector new_vec = {
        .data = malloc(2 * vec->cap),
        .len = vec->len,
        .cap = 2 * vec->cap,
    };

    for (int i = 0; i < vec->cap; i++) {
        new_vec.data[i] = vec->data[i];
    }

    free_vec(vec);
    *vec = new_vec;
}

void push_vec(struct vector* vec, char value) {
    if (vec->len >= vec->cap) {
        increase_cap(vec);
    }

    vec->data[vec->len] = value;
    vec->len += 1;
}

// copy paste moment
// I should probably try to learn how to macro better for this type of stuff
struct matrix {
    struct vector* vecs;
    uint32_t len;
    uint32_t cap;
};

void init_mat(struct matrix* mat) {
    mat->vecs = malloc(sizeof(struct matrix) * 256);
    mat->len = 0;
    mat->cap = 256;
}

void free_mat(struct matrix* mat) {
    free(mat->vecs);
}

void increase_cap_m(struct matrix* mat) {
    struct matrix new_mat = {
        .vecs = malloc(2 * mat->cap),
        .len = mat->len,
        .cap = 2 * mat->cap,
    };

    for (int i = 0; i < mat->cap; i++) {
        new_mat.vecs[i] = mat->vecs[i];
    }

    free_mat(mat);
    *mat = new_mat;
}

void push_mat(struct matrix* mat) {
    if (mat->len >= mat->cap) {
        increase_cap_m(mat);
    }
    struct vector vec;
    init_vec(&vec);

    mat->vecs[mat->len] = vec;
    mat->len += 1;
}

char get(struct matrix* mat, uint32_t x, uint32_t y) {
    if (y < 0 || y > mat->len) {
        fprintf(stderr, "invalid y index: %d\n", y);
        return '.';
    }
    if (x < 0 || x > mat->vecs[y].len) {
        fprintf(stderr, "invalid x index: %d\n", x);
        return '.';
    }
    return mat->vecs[y].data[x];
}
// end vector and matrix


//I don't like nesting if statements like this but idk how not to for this problem
bool checkBorders(struct matrix* mat, int x, int y) {
    bool top = y > 0;
    //for some reason, there's like 4 extra blank lines at the end of mat,
    //and I can't find out what I did wrong to cause it. But this fixes the
    //logical error for calculation purposes
    bool bottom = y < mat->len - 3;
    bool left = x > 0;
    bool right = x < mat->vecs[y].len - 1;

    if (top) {
        char c = mat->vecs[y-1].data[x];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (bottom) {
        char c = mat->vecs[y+1].data[x];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (left) {
        char c = mat->vecs[y].data[x-1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (top && left) {
        char c = mat->vecs[y-1].data[x-1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (top && right) {
        char c = mat->vecs[y-1].data[x+1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (bottom && left) {
        char c = mat->vecs[y+1].data[x-1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (bottom && right) {
        char c = mat->vecs[y+1].data[x+1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        }
    }
    if (right) {
        //right is a special case
        char c = mat->vecs[y].data[x+1];
        if (c != '.' && c != '\n' && !isdigit(c)) {
            return true;
        } else if (isdigit(c)) {
            return checkBorders(mat, x+1, y);
        }
    }

    return false;
}

int parseNum(struct matrix* mat, int x, int y) {
    return strtol(&mat->vecs[y].data[x], NULL, 10);
}

char lu(struct matrix* mat, int x, int y) {
    return get(mat, x-1, y-1);
}
char u(struct matrix* mat, int x, int y) {
    return get(mat, x, y-1);
}
char ru(struct matrix* mat, int x, int y) {
    return get(mat, x+1, y-1);
}
char l(struct matrix* mat, int x, int y) {
    return get(mat, x-1, y);
}
char r(struct matrix* mat, int x, int y) {
    return get(mat, x+1, y);
}
char ld(struct matrix* mat, int x, int y) {
    return get(mat, x-1, y+1);
}
char d(struct matrix* mat, int x, int y) {
    return get(mat, x, y+1);
}
char rd(struct matrix* mat, int x, int y) {
    return get(mat, x+1, y+1);
}


void printHits(bool* hits) {
    for (int i = 0; i < 9; i++) {
        fprintf(stdout, "%d, ", hits[i]);
        if ((i+1) % 3 == 0) {
            fprintf(stdout, "\n");
        }
    }
}

void reduce(bool* hits) {
    if (hits[0] && hits[1] && hits[2]) {
        hits[0] = false;
        hits[1] = false;
    } else if (hits[1]) {
        if (hits[0]) {
            hits[0] = false;
        }
        if (hits[2]) {
            hits[1] = false;
        }
    }
    
    if (hits[6] && hits[7] && hits[8]) {
        hits[6] = false;
        hits[7] = false;
    } else if (hits[7]) {
        if (hits[6]) {
            hits[6] = false;
        }
        if (hits[8]) {
            hits[7] = false;
        }
    }
}

int parseNumBacktrack(struct matrix* mat, int x, int y) {
    int localx = x;
    while (localx > 0 && isdigit(mat->vecs[y].data[localx-1])) {
        localx -= 1;
    }
    
    char temp[mat->vecs[y].len];
    memcpy(temp, mat->vecs[y].data, sizeof(char) * mat->vecs[y].len);

    //for (int i = 0; i < mat->vecs[y].len; i++) {
    //    fprintf(stdout, "%c", temp[i]);
    //}
    //fprintf(stdout, "\n");
    
    int output = strtol(&temp[localx], NULL, 10);
    //fprintf(stdout, "found %d\n", output);
    return output;
}

int calcGearRatios(struct matrix* mat, int x, int y, bool* hits) {
    reduce(hits);
    int nums[2] = {0};
    int index = 0;

    for (int i = 2; i >= 0; i--) {
        if (hits[i]) {
            nums[index] = parseNumBacktrack(mat, x + i - 1, y - 1);
            index += 1;
        }
    }

    for (int i = 5; i >= 3; i--) {
        if (hits[i]) {
            nums[index] = parseNumBacktrack(mat, x + i - 4, y);
            index += 1;
        }
    }

    for (int i = 8; i >= 6; i--) {
        if (hits[i]) {
            nums[index] = parseNumBacktrack(mat, x + i - 7, y + 1);
            index += 1;
        }
    }

    return nums[0] * nums[1];
}

//if true, calculate gear ratio and sends it to param out
bool isGear(struct matrix* mat, int x, int y, int* out) {
    int adjNums = 0;
    // 0, 1, 2, 
    // 3, 4, 5,
    // 6, 7, 8
    bool hits[9] = {0};

    if (isdigit(lu(mat, x, y)) ) {
        hits[0] = true;
    }
    if (isdigit(u(mat, x, y)) ) {
        hits[1] = true;
    }
    if (isdigit(ru(mat, x, y)) ) {
        hits[2] = true;
    }
    if (isdigit(l(mat, x, y)) ) {
        hits[3] = true;
        adjNums += 1;
    }
    if (isdigit(r(mat, x, y)) ) {
        hits[5] = true;
        adjNums += 1;
    }
    if (isdigit(ld(mat, x, y)) ) {
        hits[6] = true;
    }
    if (isdigit(d(mat, x, y)) ) {
        hits[7] = true;
    }
    if (isdigit(rd(mat, x, y)) ) {
        hits[8] = true;
    }

    //this looks stupid because it is but I'm pretty sure it works
    if (hits[0] && hits[1] && hits[2]) {
        adjNums += 1;
    } else  {
        if (hits[1]) {
            adjNums +=1;
        } else {
            if (hits[0]) {
                adjNums +=1;
            }

            if (hits[2]) {
                adjNums +=1;
            }
        }
    }

    if (hits[6] && hits[7] && hits[8]) {
        adjNums += 1;
    } else  {
        if (hits[7]) {
            adjNums +=1;
        } else {
            if (hits[6]) {
                adjNums +=1;
            }

            if (hits[8]) {
                adjNums +=1;
            }
        }
    }

    if (adjNums == 2) {
        *out = calcGearRatios(mat, x, y, hits);
        //fprintf(stdout, "Out: %d\n", *out);
        return true;
    }
    return false;
}

void part1(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        struct matrix mat;
        init_mat(&mat);
        push_mat(&mat);

        char c;
        uint32_t line = 0;
        int sum = 0;
        while ((c = getc(input)) != EOF) {
            push_vec(&mat.vecs[line], c);
            if (c == '\n') {
                line += 1;
                push_mat(&mat);
            }
        }

        for (int i = 0; i < mat.len; i++) {
            for (int j = 0; j < mat.vecs[i].len; j++) {
                if (isdigit(mat.vecs[i].data[j]) && checkBorders(&mat, j, i)) {
                    int num = parseNum(&mat, j, i);
                    //fprintf(stdout, "PART 1: Found num: %d\n", num);
                    sum += num;

                    //skip digits
                    while (num > 0) {
                        j += 1;
                        num /= 10;
                    }
                }
                //fprintf(stdout, "%c", mat.vecs[i].data[j]);
            }
            //fprintf(stdout, "%d, %d\n", mat.len, i);
        }

        fprintf(stdout, "PART 1: The sum of the part numbers is %d\n", sum);
    } else {
        fprintf(stderr, "PART 2: Couldn't open the input file\n");
    }
}

void part2(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        struct matrix mat;
        init_mat(&mat);
        push_mat(&mat);

        char c;
        uint32_t line = 0;
        int sum = 0;
        while ((c = getc(input)) != EOF) {
            push_vec(&mat.vecs[line], c);
            if (c == '\n') {
                line += 1;
                push_mat(&mat);
            }
        }

        int number = 0;
        for (int i = 0; i < mat.len; i++) {
            for (int j = 0; j < mat.vecs[i].len; j++) {
                if (mat.vecs[i].data[j] == '*') {
                    if (isGear(&mat, j, i, &number)) {
                        sum += number;
                        //fprintf(stdout, "PART 2: Found a gear with ratio: %d\n", number);
                    }
                }
                //fprintf(stdout, "%c", mat.vecs[i].data[j]);
            }
        }

        fprintf(stdout, "PART 2: The sum of the gear ratios is %d\n", sum);
    } else {
        fprintf(stderr, "PART 2: Couldn't open the input file\n");
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
