#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "./dayB/test"
#define SIZE 1024

void print_mat(char** c, uint32_t x_lim, uint32_t y_lim);

bool row_empty(char** c, uint32_t x_lim, uint32_t row) {
    for (uint32_t i = 0; i < x_lim; i++) {
        if (c[i][row] == '#') {
            return false;
        }
    }
    return true;
}

bool col_empty(char** c, uint32_t y_lim, uint32_t col) {
    for (uint32_t i = 0; i < y_lim; i++) {
        if (c[col][i] == '#') {
            return false;
        }
    }
    return true;
}

void insert_row(char** c, uint32_t* x_lim, uint32_t* y_lim, uint32_t index) {
    //shift rows down
    for (uint32_t i = *y_lim; i >= index; i--) {
        for (uint32_t j = 0; j < *x_lim; j++) {
            c[j][i+1] = c[j][i];
        }
    }

    //insert blank row
    //for (uint32_t i = 0; i < *x_lim; i++) {
    //    c[i][index] = '.';
    //}

    //print_mat(c, *x_lim, *y_lim);

    *y_lim += 1;
}

void insert_col(char** c, uint32_t* x_lim, uint32_t* y_lim, uint32_t index) {
    //shift columns right
    for (uint32_t i = *x_lim; i >= index; i--) {
        for (uint32_t j = 0; j < *y_lim; j++) {
            c[i+1][j] = c[i][j];
        }
    }

    //insert blank column
    //for (uint32_t i = 0; i < *y_lim; i++) {
    //    c[index][i] = '.';
    //}

    //print_mat(c, *x_lim, *y_lim);

    *x_lim += 1;
}

void print_mat(char** c, uint32_t x_lim, uint32_t y_lim) {
    for (uint32_t y = 0; y < y_lim; y++) {
        for (uint32_t x = 0; x < x_lim; x++) {
            printf("%c", c[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_bools(bool* bools, uint32_t num) {
    for (uint32_t i = 0; i < num; i++) {
        printf("%d, ", bools[i]);
    }
    printf("\n");
}

void expand(char** c, uint32_t* x, uint32_t* y, bool* rows, bool* cols) {
    for (uint32_t i = 0; i < *x; i++) {
        cols[i] = col_empty(c, *y, i);
    }
    for (uint32_t i = 0; i < *y; i++) {
        rows[i] = row_empty(c, *x, i);
    }
    //print_bools(rows, *y);
    //print_bools(cols, *x);

    return;

    //old part 1 code
    /*
    for (uint32_t i = SIZE-1; i > 0; i--) {
        if (rows[i]) {
            //printf("inserting row\n");
            insert_row(c, x, y, i);
        }
        if (cols[i]) {
            //printf("inserting column\n");
            insert_col(c, x, y, i);
        }
    }
    */
}

void find_galaxies(char** c, uint32_t x, uint32_t y, uint32_t* pos_count, 
        uint32_t** positions) {
    if (!positions) {
        uint32_t count = 0;
        for (uint32_t i = 0; i < y; i++) {
            for (uint32_t j = 0; j < x; j++) {
                if (c[j][i] == '#') {
                    count++;
                    //c[j][i] = count + 48;
                }
            }
        }

        *pos_count = count;
        return;
    }
    
    uint32_t n = 0;
    for (uint32_t i = 0; i < y; i++) {
        for (uint32_t j = 0; j < x; j++) {
            if (c[j][i] == '#') {
                positions[n][0] = j;
                positions[n][1] = i;
                n++;
            }
        }
    }
}

uint32_t pair_count(uint32_t num_positions) {
    if (num_positions == 0) {
        return 0;
    }
    return num_positions - 1 + pair_count(num_positions - 1);
}

uint32_t get_path(char** c, uint32_t* p1, uint32_t* p2, uint32_t factor, 
        bool* rows, bool* cols) {
    int dir[2] = {p2[0] - p1[0], p2[1] - p1[1]};

    uint32_t num_cols = 0;
    uint32_t num_rows = 0;
    
    if (dir[0] < 0) {
        dir[0] *= -1;
    }
    if (dir[1] < 0) {
        dir[1] *= -1;
    }

    uint32_t startx = p1[0];
    if (p2[0] < p1[0]) {
        startx = p2[0];
    }
    uint32_t endx = SIZE;
    if (dir[0] < endx) {
        endx = dir[0];
    }
    for (uint32_t x = startx; x < startx + endx; x++) {
        if (cols[x]) {
            num_cols++;
        }
    }

    uint32_t starty = p1[1];
    if (p2[1] < p1[1]) {
        starty = p2[1];
    }
    uint32_t endy = SIZE;
    if (dir[1] < endy) {
        endy = dir[1];
    }
    for (uint32_t y = starty; y < starty + endy; y++) {
        if (rows[y]) {
            num_rows++;
        }
    }

    uint32_t extra = 0;
    if (num_cols > 0) {
        extra += (num_cols) * (factor-1);
    }
    if (num_rows > 0) {
        extra += (num_rows) * (factor-1);
    }
    //printf("num_cols: %d\nnum_rows: %d\n", num_cols, num_rows);

    return dir[0] + dir[1] + extra;
}

int main(int argc, char** argv) {
    char* path = INPUT;
    uint32_t factor = 2;
    if (argc > 1) {
        path = argv[1];
    }
    if (argc > 2) {
        factor = strtol(argv[2], NULL, 10);
    }

    FILE* f = fopen(path, "r");
    if (f) {
        uint32_t xsize = 0, ysize = 0;
        uint32_t x = 0, y = 0;
        char** data = malloc(sizeof(char*) * SIZE);
        for (uint32_t i = 0; i < SIZE; i++) {
            data[i] = malloc(sizeof(char) * SIZE);
            memset(data[i], 0, sizeof(char) * SIZE);
        }
        char c;
        while ((c = getc(f)) != EOF) {
            if (c != '\n') {
                data[x][y] = c;
            }
            x++;
            if (c == '\n') {
                y++;
                xsize = x-1;
                x = 0;
            }
        }
        ysize = y;

        bool rows[ysize];
        bool cols[xsize];
        memset(rows, false, sizeof(bool) * ysize);
        memset(cols, false, sizeof(bool) * xsize);
        expand(data, &xsize, &ysize, rows, cols);

        uint32_t num_positions = 0;
        find_galaxies(data, xsize, ysize, &num_positions, NULL);
        uint32_t** positions = malloc(sizeof(uint32_t*) * num_positions);
        for (uint32_t i = 0; i < num_positions; i++) {
            positions[i] = malloc(sizeof(uint32_t) * 2);
            positions[i][0] = 0;
            positions[i][1] = 0;
        }
        
        find_galaxies(data, xsize, ysize, &num_positions, positions);
        //print_mat(data, xsize, ysize);

        uint32_t num_pairs = pair_count(num_positions);
        uint32_t p1 = 0, p2 = 1;
        uint64_t sum = 0;
        for (uint32_t i = 0; i < num_pairs; i++) {
            uint32_t num = get_path(data, positions[p1], positions[p2], factor, rows, cols);
            //printf("Path between %d,%d and %d,%d is %d\n", positions[p1][0],
            //    positions[p1][1], positions[p2][0], positions[p2][1], num);

            sum += num;
            p2++;

            if (p2 == num_positions) {
                p1++;
                p2 = p1+1;
            }
        }

        printf("The sum of the shortest paths is %lu\n", sum);

        for (uint32_t i = 0; i < num_positions; i++) {
            free(positions[i]); 
        }
        free(positions);
        for (uint32_t i = 0; i < SIZE; i++) {
            free(data[i]);
        }
        free(data);
    } else {
        fprintf(stderr, "Couldn't open the file\n");
    }
    fclose(f);
}
