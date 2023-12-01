#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//longest input is not even 80 chars, so this is plenty
#define CHUNK 256
#define INPUT "./day1/input"

void part1(const char* param) {
    FILE* input;
    input = fopen(param, "r");
    
    if (input) {
        int c, sum = 0;
        char line[CHUNK] = {0};
        int length = 0;
        do {
            c = getc(input);
            if (c != '\n') {
                //load line
                line[length] = c;
                length++;
            } else {
                //calc, add to sum, reset line and length
                char digit1 = -1, digit2 = -1;
                for (int i = 0; i < length; i++) {
                    if (isdigit(line[i])) {
                        digit1 = line[i];
                        break;
                    }
                }

                for (int i = length; i >= 0; i--) {
                    if (isdigit(line[i])) {
                        digit2 = line[i];
                        break;
                    }
                }
                //fprintf(stdout, "PART 1: %c, %c\n", digit1, digit2);

                if (digit1 == -1 || digit2 == -1) {
                    fprintf(stderr, "PART 1: error parsing digits from line\n");
                    continue;
                }
                
                //add to sum
                digit1 -= '0';
                digit2 -= '0';
                sum += (digit1 * 10) + digit2;

                //reset line and length
                memset(line, 0, sizeof(line));
                length = 0;
            }
        } while(c != EOF);
    
        fprintf(stdout, "PART 1: The sum of the calibration values is %d\n", sum);
        fclose(input);
    } else {
        fprintf(stderr, "PART 1: Couldn't open input file\n");
        exit(EXIT_FAILURE);
    }
}

bool check(char* line, int index, char c) {
    return line[index] == c;
}

//this is unsafe I think
bool checkStr(char* line, int index, char* str, int strlen) {
    for (int i = 0; i < strlen; i++) {
        if (line[index + i] != str[i]) {
            return false;
        }
    }

    return true;
}

char checkForDigit(char* line, int index) {
    switch(line[index]) {
        case 'o': 
            if (checkStr(line, index+1, "ne", 2)) { 
                return '1';
            }
            break;
        case 't':
            if (checkStr(line, index+1, "wo", 2)) { 
                return '2';
            } else if (checkStr(line, index+1, "hree", 4)) { 
                return '3';
            } 
            break;
        case 'f':
            if (checkStr(line, index+1, "our", 3)) {
                return '4';
            } else if (checkStr(line, index+1, "ive", 3)) {
                return '5';
            }
            break;
        case 's':
            if (checkStr(line, index+1, "ix", 2)) {
                return '6';
            } else if (checkStr(line, index+1, "even", 4)) {
                return '7';
            }
            break;
        case 'e':
            if (checkStr(line, index+1, "ight", 4)) {
                return '8';
            }
            break;
        case 'n':
            if (checkStr(line, index+1, "ine", 3)) {
                return '9';
            }
            break;
    }

    return -1;
}

void part2(const char* param) {
    FILE* input;
    input = fopen(param, "r");
    
    if (input) {
        int c, sum = 0;
        char line[CHUNK] = {0};
        int length = 0;
        do {
            c = getc(input);
            if (c != '\n') {
                //load line
                line[length] = c;
                length++;
            } else {
                //calc, add to sum, reset line and length
                char digit1 = -1, digit2 = -1;
                for (int i = 0; i < length; i++) {
                    if (isdigit(line[i])) {
                        digit1 = line[i];
                        break;
                    } else {
                        if ((digit1 = checkForDigit(line, i)) != -1) {
                            break;
                        }
                    }
                }

                for (int i = length; i >= 0; i--) {
                    if (isdigit(line[i])) {
                        digit2 = line[i];
                        break;
                    } else {
                        if ((digit2 = checkForDigit(line, i)) != -1) {
                            break;
                        }
                    }
                }
                fprintf(stdout, "PART 2: %c, %c\n", digit1, digit2);
                digit1 -= '0';
                digit2 -= '0';

                if (digit1 == -1 || digit2 == -1) {
                    fprintf(stderr, "PART 2: Error parsing digits from line\n");
                    exit(EXIT_FAILURE);
                }
                
                //add to sum
                sum += (digit1 * 10) + digit2;
                fprintf(stdout, "PART 2: %d, %d, %d\n", digit1, digit2, sum);

                //reset line and length
                length = 0;
                memset(line, 0, sizeof(line));
            }
        } while(c != EOF);
    
        fprintf(stdout, "PART 2: The sum of the calibration values is %d\n", sum);
        fclose(input);
    } else {
        fprintf(stderr, "PART 2: Couldn't open input file\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    char* param = INPUT;
    if (argc > 1) {
        param = argv[1];
    }
    part1(param);
    part2(param);
    exit(EXIT_SUCCESS);
}
