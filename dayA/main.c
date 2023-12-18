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

matrix* create_mat(uint32_t xsize, uint32_t ysize) {
    matrix* m = malloc(sizeof(matrix));
    m->data = malloc(sizeof(char*) * ysize);
    memset(m->data, 0, sizeof(char*) * ysize);
    m->ysize = ysize;
    m->xsize = xsize;
    return m;
}

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

    matrix* m = create_mat(xsize, ysize);
    memcpy(m->data, lines, sizeof(char*) * ysize);

    return m;
}

void populate_default_mat(matrix* m) {
    char* lines[1024] = {0};
    for (uint32_t i = 0; i < m->ysize; i++) {
        lines[i] = malloc(sizeof(char) * m->xsize);
        memset(lines[i], '.', sizeof(char) * m->xsize);
    }
    memcpy(m->data, lines, sizeof(char*) * m->ysize);
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
    //print_pos(&no);
    //print_pos(&so);
    //print_pos(&ea);
    //print_pos(&we);
    if (connects_down(n(m, start))) {
        return no;
    }
    if (connects_up(s(m, start))) {
        return so;
    }
    if (connects_left(w(m, start))) {
        return ea;
    }
    if (connects_right(e(m, start))) {
        return we;
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
    if (num_steps) {
        *num_steps += 1;
    }
    return next;
}

void mark_pos(matrix* m, pos p, char c) {
    m->data[p.y][p.x] = c;
}

void print_mat(matrix* m) {
    for (uint32_t y = 0; y < m->ysize; y++) {
        for (uint32_t x = 0; x < m->xsize; x++) {
            printf("%c", m->data[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

bool visited(uint32_t x, uint32_t y, matrix* v) {
    return v->data[y][x] != '.';
}

bool is_wall(uint32_t x, uint32_t y, matrix* m) {
    char c = m->data[y][x];
    return c == 'S' 
        || c == '|' || c == '-' 
        || c == 'L' || c == 'J'
        || c == '7' || c == 'F';
}

void flood_fill(uint32_t x, uint32_t y, char c, matrix* m, matrix* v) {
    if (x < 0 || x >= m->xsize || y < 0 || y >= m->ysize
            || is_wall(x, y, m) 
            || visited(x, y, v)) {
        return;
    }
    //set visited
    mark_pos(v, (pos){.x=x,.y=y}, c);
    //fill
    mark_pos(m, (pos){.x=x,.y=y}, c);

    flood_fill(x, y+1, c, m, v);
    flood_fill(x, y-1, c, m, v);
    flood_fill(x-1, y, c, m, v);
    flood_fill(x+1, y, c, m, v);
    //flood_fill(x+1, y+1, c, m, v);
    //flood_fill(x-1, y-1, c, m, v);
    //flood_fill(x-1, y+1, c, m, v);
    //flood_fill(x+1, y-1, c, m, v);
}

void fill_borders(matrix* m) {
    matrix* visited = create_mat(m->xsize, m->ysize);
    populate_default_mat(visited);

    pos start_pos = find_start(m);
    pos curr = find_loop(m, NULL, start_pos);
    while (curr.curr != 'S') {
        curr = step(m, NULL, curr, &start_pos);
        
        int xmod = curr.x - start_pos.x;
        int ymod = curr.y - start_pos.y;

        flood_fill(curr.x - (1 * ymod), curr.y, '$', m, visited);
        flood_fill(curr.x, curr.y + (1 * xmod), '$', m, visited);

        flood_fill(curr.x + (1 * ymod), curr.y, '#', m, visited);
        flood_fill(curr.x, curr.y - (1 * xmod), '#', m, visited);

    }

    free_matrix(visited);
}

uint32_t count_enclosed(matrix* m) {
    char enclosed = '.';
    for (uint32_t y = 0; y < m->ysize; y++) {
        if (!is_wall(0, y, m)) {
            if (m->data[y][0] == '$') {
                enclosed = '#';
            } else {
                enclosed = '$';
            }
            break;
        }
    }
    printf("the enclosed character is %c\n", enclosed);

    uint32_t sum = 0;
    for (uint32_t y = 0; y < m->ysize; y++) {
        for (uint32_t x = 0; x < m->xsize; x++) {
            if (m->data[y][x] == enclosed) {
                sum++;
            }
        }
    }
    return sum;
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
        matrix* loop = create_mat(m->xsize, m->ysize);
        populate_default_mat(loop);

        pos start_pos = find_start(m);
        mark_pos(loop, start_pos, 'S');
        pos curr = find_loop(m, &num_steps, start_pos);
        mark_pos(loop, curr, curr.curr);
        while (curr.curr != 'S') {
            curr = step(m, &num_steps, curr, &start_pos);
            mark_pos(loop, curr, curr.curr);
            //print_pos(&curr);
        }
        printf("The farthest point in the loop is %d steps away\n", (num_steps+1)/2);

        print_mat(loop);
        fill_borders(loop);
        print_mat(loop);
        printf("The number of enclosed tiles is %d \n", count_enclosed(loop));

        free_matrix(loop);
        free_matrix(m);
    } else {
        fprintf(stderr, "Couldn't open file\n");
    }
    fclose(f);
}
