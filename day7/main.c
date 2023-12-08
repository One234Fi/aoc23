#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INPUT "./day7/test"

//could (prob should) also just use plain chars but thats boring
//this also allows direct comparision of cards via </>/==/!=
typedef enum card {
    Ace,
    King,
    Queen,
    Jack,
    Ten,
    Nine,
    Eight,
    Seven,
    Six,
    Five,
    Four,
    Three,
    Two,
    CARD_SIZE,
} card;

typedef enum card_2 {
    Ace_2,
    King_2,
    Queen_2,
    Ten_2,
    Nine_2,
    Eight_2,
    Seven_2,
    Six_2,
    Five_2,
    Four_2,
    Three_2,
    Two_2,
    Joker_2,
    CARD_SIZE_2,
} card_2;

card get_card(char c) {
    switch (c) {
        case 'A': return Ace;
        case 'K': return King;
        case 'Q': return Queen;
        case 'J': return Jack;
        case 'T': return Ten;
        case '9': return Nine;
        case '8': return Eight;
        case '7': return Seven;
        case '6': return Six;
        case '5': return Five;
        case '4': return Four;
        case '3': return Three;
        case '2': return Two;
        default:
            fprintf(stderr, "Attempted to parse an invalid card: %c\n", c);
    }

    return CARD_SIZE;
}

card_2 get_card_2(char c) {
    switch (c) {
        case 'A': return Ace_2;
        case 'K': return King_2;
        case 'Q': return Queen_2;
        case 'T': return Ten_2;
        case '9': return Nine_2;
        case '8': return Eight_2;
        case '7': return Seven_2;
        case '6': return Six_2;
        case '5': return Five_2;
        case '4': return Four_2;
        case '3': return Three_2;
        case '2': return Two_2;
        case 'J': return Joker_2;
        default:
            fprintf(stderr, "Attempted to parse an invalid card: %c\n", c);
    }

    return CARD_SIZE_2;
}

typedef enum type {
    FiveOfAKind,
    FourOfAKind,
    FullHouse,
    ThreeOfAKind,
    TwoPair,
    OnePair,
    HighCard,
    TYPE_SIZE
} type;

type get_type_2(card_2* cards) {
    card_2 types[13] = {0};
    for (uint32_t i = 0; i < 5; i++) {
        types[cards[i]] += 1;
    }
    uint32_t max = 0;
    uint32_t num_twos = 0;
    uint32_t num_ones = 0;
    uint32_t num_wilds = types[12];
    for (uint32_t i = 0; i < 12; i++) {
        if (types[i] > max) {
            max = types[i];
        }
        if (types[i] == 2) {
            num_twos += 1;
        }
        if (types[i] == 1) {
            num_ones += 1;
        }
    }

    switch (max) {
        case 5: return FiveOfAKind;

        case 4: 
                switch (num_wilds) {
                    case 1: return FiveOfAKind;
                    default: return FourOfAKind;
                }

        case 3: 
                switch (num_wilds) {
                    case 2: return FiveOfAKind;
                    case 1: return FourOfAKind;
                    case 0: switch (num_twos) {
                                case 0: return ThreeOfAKind;
                                case 1: return FullHouse;
                            }
                            break;
                }
                break;

        case 2:
                switch (num_wilds) {
                    case 3: return FiveOfAKind;
                    case 2: return FourOfAKind;
                    case 1: switch (num_twos) {
                                case 1: return ThreeOfAKind;
                                case 2: return FullHouse;
                            }
                            break;
                    case 0: switch (num_twos) {
                                case 1: return OnePair;
                                case 2: return TwoPair;
                            }
                            break;
                }
                break;

        case 1: 
                switch (num_wilds) {
                    case 4: return FiveOfAKind;
                    case 3: return FourOfAKind;
                    case 2: return ThreeOfAKind;
                    case 1: return OnePair;
                    case 0: return HighCard;
                }
                break;

        case 0: //All Jokers
                return FiveOfAKind;
    }

    fprintf(stderr, "Attempted to parse an invalid hand:\n");
    for (uint32_t i = 0; i < 5; i++) {
        fprintf(stderr, "%d, ", cards[i]);
    }
    fprintf(stderr, "\n");
    return TYPE_SIZE;
}

type get_type(card* cards) {
    card types[13] = {0};
    for (uint32_t i = 0; i < 5; i++) {
        types[cards[i]] += 1;
    }
    uint32_t max = 0;
    uint32_t num_twos = 0;
    for (uint32_t i = 0; i < 13; i++) {
        if (types[i] > max) {
            max = types[i];
        }
        if (types[i] == 2) {
            num_twos += 1;
        }
    }

    switch(max) {
        case 5: return FiveOfAKind;
        case 4: return FourOfAKind;
        case 3:
            switch (num_twos) {
                case 0: return ThreeOfAKind;
                case 1: return FullHouse;
                default:
                        ;
            }
            break;
        case 2:
            switch (num_twos) {
                case 1: return OnePair;
                case 2: return TwoPair;
                default:
                        ;
            }
            break;
        case 1: return HighCard;
        default:
            ;
    }

    fprintf(stderr, "Attempted to parse an invalid hand\n");
    return TYPE_SIZE;
}

typedef struct hand {
    uint32_t cards[5];
    type label;
    uint32_t bid;
} hand;

//allocs to heap
hand* init_hand(char* line, bool game_2) {
    hand* h = malloc(sizeof(hand));

    if (game_2) {
        for (uint32_t i = 0; i < 5; i++) {
            h->cards[i] = get_card_2(line[i]);
        }
        h->label = get_type_2(h->cards);
    } else {
        for (uint32_t i = 0; i < 5; i++) {
            h->cards[i] = get_card(line[i]);
        }
        h->label = get_type(h->cards);
    }
    h->bid = strtol(&line[6], NULL, 10);

    return h;
}

void free_hand(hand* h) {
    free(h);
}

//allocs to heap
char* readLine(FILE* file, bool* eof) {
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

    if (c == EOF) {
        *eof = true;
    }

    return string;
}

//return h1 < h2
bool compare_hands(hand* h1, hand* h2) {
    if (h1->label == h2->label) {
        for (uint32_t i = 0; i < 5; i++) {
            if (h1->cards[i] != h2->cards[i]) {
                return h1->cards[i] < h2->cards[i];
            }
        }
    } else {
        return h1->label < h2->label;
    }

    //equal hands
    return false;
}

typedef struct bin_tree_node {
    hand* data;
    struct bin_tree_node* left;
    struct bin_tree_node* right;
} node;

node* init_node(hand* h) {
    node* n = malloc(sizeof(node));
    n->data = h;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void free_node(node* n) {
    free_hand(n->data);
    free(n);
}

void insert_node(node* n, node* h) {
    //n < h
    if (compare_hands(n->data, h->data)) {
        if (n->left == NULL) {
            n->left = h;
            return;
        } 
        insert_node(n->left, h);
        return;
    } 

    if (n->right == NULL) {
        n->right = h;
        return;
    }
    insert_node(n->right, h);
    return;
}

void in_order_traverse(node* n, void func(node*, uint32_t*), uint32_t* sum) {
    if (n==NULL) {
        return;
    }
    in_order_traverse(n->left, func, sum);
    func(n, sum);
    in_order_traverse(n->right, func, sum);
}

static uint32_t times_called = 0;
void calc_node_score(node* n, uint32_t* sum) {
    *sum += n->data->bid * (++times_called);
}

void printNode(node* n, uint32_t* dud) {
    printf("%d, %d, ", n->data->label, n->data->bid);
    for (int i = 0; i < 5; i++) {
        printf("%d, ", n->data->cards[i]);
    }
    printf("\n");
}

void part1(const char* path) {
    FILE* input = fopen(path, "r");

    if (input) {
        bool eof = false;
        uint32_t numLines = 0;
        char* lines[1024] = {0};
        node* nodes[1024] = {0};

        lines[0] = readLine(input, NULL);
        node* hands = init_node(init_hand(lines[0], false));
        nodes[0] = hands;
        numLines++;
        while (!eof) {
            lines[numLines] = readLine(input, &eof);
            if (!eof) {
                node* new_node = init_node(init_hand(lines[numLines], false));
                insert_node(hands, new_node);
                nodes[numLines] = new_node;
                numLines++;
            }
        }
        uint32_t sum = 0;
        in_order_traverse(hands, calc_node_score, &sum);

        printf("PART 1: The total winnings are %d\n", sum);

        //in_order_traverse(hands, free_node_wrapper, NULL);
        for (uint32_t i = 0; i < numLines; i++) {
            free(lines[i]);
            free_node(nodes[i]);
        }
        free(lines[numLines]);
    } else {
        fprintf(stderr, "PART 1: Couldn't open file\n");
    }
    fclose(input);
}


void part2(const char* path) {
    FILE* input = fopen(path, "r");

    if (input) {
        bool eof = false;
        uint32_t numLines = 0;
        char* lines[1024] = {0};
        node* nodes[1024] = {0};

        lines[0] = readLine(input, NULL);
        node* hands = init_node(init_hand(lines[0], true));
        nodes[0] = hands;
        numLines++;
        while (!eof) {
            lines[numLines] = readLine(input, &eof);
            if (!eof) {
                node* new_node = init_node(init_hand(lines[numLines], true));
                insert_node(hands, new_node);
                nodes[numLines] = new_node;
                numLines++;
            }
        }
        uint32_t sum = 0;
        //in_order_traverse(hands, printNode, NULL);
        times_called = 0;
        in_order_traverse(hands, calc_node_score, &sum);

        printf("PART 2: The total winnings are %d\n", sum);

        for (uint32_t i = 0; i < numLines; i++) {
            free(lines[i]);
            free_node(nodes[i]);
        }
        free(lines[numLines]);
    } else {
        fprintf(stderr, "PART 2: Couldn't open file\n");
    }
    fclose(input);
}

int main(int argc, char** argv) {
    char* input = INPUT;
    if (argc > 1) {
        input = argv[1];
    }

    part1(input);
    part2(input);
}
