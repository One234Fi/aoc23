#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT "./dayA/test2"

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

typedef struct matrix {
    char** data;
    uint32_t xsize;
    uint32_t ysize;
} matrix;

matrix* load_file(FILE* f) {
    bool eof = false;
    uint32_t xsize = 0, ysize = 0;
    char* lines[1024] = {0};
    while (!eof) {
        char* line = readLine(f, &eof);
        if (!eof) {
            lines[ysize] = line;
            ysize++;
        } else {
            //discard eof line
            free(line);
        }
    }
    char* c = lines[0];
    while (*c != '\0') {
        c++;
        xsize++;
    }

    matrix* m = malloc(sizeof(matrix));
    m->data = malloc(sizeof(char*) * ysize);
    m->ysize = ysize;
    m->xsize = xsize;
    memcpy(m->data, lines, sizeof(char*) * ysize);

    return m;
}

void free_matrix(matrix* m) {
    for (uint32_t i = 0; i < m->ysize; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

typedef struct pos {
    uint32_t x;
    uint32_t y;
    char curr;
} pos;

pos find_start(matrix* m) {
    for (uint32_t i = 0; i < m->ysize; i++) {
        for (uint32_t j = 0; j < m->xsize; j++) {
            if (m->data[i][j] == 'S') {
                return (pos) {
                    .x = j,
                    .y = i,
                    .curr = 'S',
                };
            }
        }
    }
    fprintf(stderr, "Couldn't find \'S\'\n");
    exit(EXIT_FAILURE);
}

bool check_bounds(matrix* m, pos curr) {
    return curr.x > m->xsize || curr.y > m->ysize || curr.x < 0 || curr.y < 0;
}

pos n(matrix* m, pos curr) {
    if (curr.y == 0) {
        return (pos) {};
    }
    pos p = {
        .curr = m->data[curr.y-1][curr.x],
        .x = curr.x,
        .y = curr.y-1,
    };
    return p;
}
pos s(matrix* m, pos curr) {
    if (curr.y == m->ysize-1) {
        return (pos) {};
    }
    pos p = {
        .curr = m->data[curr.y+1][curr.x],
        .x = curr.x,
        .y = curr.y+1,
    };
    return p;
}
pos e(matrix* m, pos curr) {
    if (curr.x == m->xsize-1) {
        return (pos) {};
    }
    pos p = {
        .curr = m->data[curr.y][curr.x+1],
        .x = curr.x+1,
        .y = curr.y,
    };
    return p;
}
pos w(matrix* m, pos curr) {
    if (curr.x == 0) {
        return (pos) {};
    }
    pos p = {
        .curr = m->data[curr.y][curr.x-1],
        .x = curr.x-1,
        .y = curr.y,
    };
    return p;
}

bool connects_right(pos p) {
    char c = p.curr;
    return c == '-' || c == 'L' || c == 'F' || c == 'S';
}
bool connects_left(pos p) {
    char c = p.curr;
    return c == '-' || c == 'J' || c == '7' || c == 'S';
}
bool connects_up(pos p) {
    char c = p.curr;
    return c == '|' || c == 'L' || c == 'J' || c == 'S';
}
bool connects_down(pos p) {
    char c = p.curr;
    return c == '|' || c == '7' || c == 'F' || c == 'S';
}

bool connected(pos src, pos dst) {
    int haxis = src.y - dst.y;
    int vaxis = src.x - dst.x;

    //check adjacency
    if (abs(haxis) > 1 || abs(vaxis) > 1) {
        return false;
    }

    // s d
    if (haxis == 0 && vaxis == 1) {
        return connects_right(src) && connects_left(dst);
    }
    // d s
    if (haxis == 0 && vaxis == -1) {
        return connects_right(dst) && connects_left(src);
    }
    // s
    // d
    if (haxis == 1 && vaxis == 0) {
        return connects_up(dst) && connects_down(src);
    }
    // d
    // s
    if (haxis == -1 && vaxis == 0) {
        return connects_up(src) && connects_down(dst);
    }

    return false;
}

void print_pos(pos* p) {
    printf("x: %d, y: %d\nc: %c\n", p->x, p->y, p->curr);
}

pos find_loop(matrix* m, uint32_t* num_steps, pos start) {
    pos no = n(m, start);
    pos so = s(m, start);
    pos ea = e(m, start);
    pos we = w(m, start);
    print_pos(&no);
    print_pos(&so);
    print_pos(&ea);
    print_pos(&we);
    if (connects_down(n(m, start))) {
        return n(m, start);
    }
    if (connects_up(s(m, start))) {
        return s(m, start);
    }
    if (connects_left(w(m, start))) {
        return w(m, start);
    }
    if (connects_right(e(m, start))) {
        return e(m, start);
    }
    fprintf(stderr, "couldn't find a loop\n");
    return (pos){};
}

bool pos_equal(pos p1, pos p2) {
    return p1.x == p2.x && p1.y == p2.y && p1.curr == p2.curr;
}

pos step(matrix* m, uint32_t* num_steps, const pos curr, pos* prev) {
    pos next = {
        .curr = '.',
        .x = 0,
        .y = 0,
    };

    switch (curr.curr) {
        case '|':
            if (connects_down(n(m, curr)) && !pos_equal(n(m, curr), *prev)) {
                next = n(m, curr);
            } else {
                next = s(m, curr);
            }
            break;
        case '-':
            if (connects_left(e(m, curr)) && !pos_equal(e(m, curr), *prev)) {
                next = e(m, curr);
            } else {
                next = w(m, curr);
            }
            break;
        case 'L':
            if (connects_down(n(m, curr)) && !pos_equal(n(m, curr), *prev)) {
                next = n(m, curr);
            } else {
                next = e(m, curr);
            }
            break;
        case 'J':
            if (connects_down(n(m, curr)) && !pos_equal(n(m, curr), *prev)) {
                next = n(m, curr);
            } else {
                next = w(m, curr);
            }
            break;
        case '7':
            if (connects_up(s(m, curr)) && !pos_equal(s(m, curr), *prev)) {
                next = s(m, curr);
            } else {
                next = w(m, curr);
            }
            break;
        case 'F':
            if (connects_up(s(m, curr)) && !pos_equal(s(m, curr), *prev)) {
                next = s(m, curr);
            } else {
                next = e(m, curr);
            }
            break;

        case 'S':
            printf("done\n");
            break;


        case '.':
        default:
            fprintf(stderr, "tried to step from an invalid char\n");
    };

    *prev = curr;
    *num_steps += 1;
    return next;
}


int main(int argc, char** argv) {
    char* path = INPUT;
    if (argc > 1) {
        path = argv[1];
    }

    FILE* f = fopen(path, "r");
    if (f) {
        uint32_t num_steps = 0;
        matrix* m = load_file(f);
        pos start_pos = find_start(m);
        pos curr = find_loop(m, &num_steps, start_pos);
        while (curr.curr != 'S') {
            curr = step(m, &num_steps, curr, &start_pos);
            //print_pos(&curr);
        }

        printf("The farthest point in the loop is %d steps away\n", (num_steps+1)/2);

    } else {
        fprintf(stderr, "Couldn't open file\n");
    }
    fclose(f);
}
