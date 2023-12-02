#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define INPUT "./day2/input"
#define CHUNK 256

#define RED 12
#define GREEN 13
#define BLUE 14

int discardId(const char* line) {
    int i = 0;
    while (line[i] != ':') {
        i++;
    }
    return i+1;
}

void skipWhitespace(const char* line, int* index) {
    while (line[*index] == ' ') {
        *index += 1;
    }
}

void printColorCount(int* rgb) {
    fprintf(stdout, "R: %d, G: %d, B: %d\n\n", rgb[0], rgb[1], rgb[2]);
}

void parseColor(const char* line, int* index, int* colors) {
    skipWhitespace(line, index);
    char buffer [16] = {0};
    int i = 0;
    while (line[*index] != ',' && line[*index] != ';' && line[*index] != '\n') {
        buffer[i] = line[*index];
        i += 1;
        *index +=1;
    }
    skipWhitespace(line, index);

    char numBuffer[4] = {0};
    int numLen = 0;
    for (int j = 0; j < i; j++) {
        if (!isalpha(buffer[j])) {
            if (!isblank(buffer[j])) {
                numBuffer[numLen] = buffer[j];
                numLen += 1;
            }
            continue;
        }
        int actualNum = 0;
        if (numLen == 1) {
            actualNum = numBuffer[0] - '0';
        } else if (numLen == 2) {
            actualNum = ((numBuffer[0] - '0') * 10) + (numBuffer[1] - '0');
        } else {
            fprintf(stderr, "PART 1: Error parsing an RGB number\n");
        }
        switch (buffer[j]) {
            case 'r':
                colors[0] += actualNum;
                break;
            case 'g':
                colors[1] += actualNum;
                break;
            case 'b':
                colors[2] += actualNum;
                break;
            default: 
                fprintf(stderr, "PART 1: Unreachable triggered in parseColor\n");
        }
        break;
    }
}

//NOTE: actually read the prompt before trying to solve the problem idiot
bool isPossibleAggregate(const char* line, const int length, int r, int g, int b) {
    int rgb[3] = {0};
    for (int i = discardId(line); i < length; i++) {
        parseColor(line, &i, rgb);
    }
    bool red = (r - rgb[0]) >= 0;
    bool green = (g - rgb[1]) >= 0;
    bool blue = (b - rgb[2]) >= 0;
    return red && green && blue;
}

bool isPossible(const char* line, const int length, int r, int g, int b) {
    int rgb[3] = {0};
    for (int i = discardId(line); i < length; i++) {
        parseColor(line, &i, rgb);
        bool red = (r - rgb[0]) >= 0;
        bool green = (g - rgb[1]) >= 0;
        bool blue = (b - rgb[2]) >= 0;
        if (!(red && green && blue)) {
            return false;
        }
        memset(rgb, 0, sizeof(rgb));
    }
    return true;
}

void part1(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        char c;
        char line[CHUNK] = {0};
        int length = 0;
        int sum = 0;
        int game_id = 1;
        do {
            c = getc(input);
            if (c != '\n') {
                line[length] = c;
                length++;
            } else {
                //append \n as an end of line marker 
                line[length] = c;
                length++;

                if (isPossible(line, length, RED, GREEN, BLUE)) {
                    sum += game_id;
                    //fprintf(stdout, "PART 1: Game %d is possible!\n", game_id);
                } else {
                    //fprintf(stdout, "PART 1: Game %d is not possible...\n", game_id);
                }

                memset(line, 0, sizeof(line));
                length = 0;
                game_id += 1;
            }
        } while (c != EOF);
        fprintf(stdout, "PART 1: The sum of the possible game's IDs is %d\n", sum);
    }
}

int power_of_minset(const char* line, const int length, int r, int g, int b) {
    int rgb[3] = {0};
    int minrgb[3] = {0};
    for (int i = discardId(line); i < length; i++) {
        parseColor(line, &i, rgb);
        for (int j = 0; j < 3; j++) {
            if (rgb[j] > minrgb[j]) {
                minrgb[j] = rgb[j];
            }
        }

        memset(rgb, 0, sizeof(rgb));
    }

    int power = 1;
    for (int i = 0; i < 3; i++) {
        power *= minrgb[i];
    }

    return power;
}

void part2(const char* path) {
    FILE* input;
    input = fopen(path, "r");

    if (input) {
        char c;
        char line[CHUNK] = {0};
        int length = 0;
        int sum = 0;
        do {
            c = getc(input);
            if (c != '\n') {
                line[length] = c;
                length++;
            } else {
                //append \n as an end of line marker 
                line[length] = c;
                length++;

                sum += power_of_minset(line, length, RED, GREEN, BLUE);

                memset(line, 0, sizeof(line));
                length = 0;
            }
        } while (c != EOF);
        fprintf(stdout, "PART 2: The sum of the powers of the minimum cube sets is %d\n", sum);
    }
}

int main(int argc, char** argv) {
    char* input = INPUT;
    if (argc > 1) {
        input = argv[1];
    }
    part1(input);
    part2(input);
}
