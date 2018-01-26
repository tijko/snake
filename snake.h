#ifndef SNAKE_H
#define SNAKE_H


#define SNAKE_INIT_LEN 4

void print_usage(void);

<<<<<<< HEAD
#define OPT_ERROR(x)                          \
    do {                                      \
        printf("Invalid Option (-%c)!\n", x); \
        print_usage();                        \
    } while (1)                               \
=======
#define OPT_ERROR       \
    do {                \
        print_usage();  \
        exit(1);        \
    } while (1)         \
>>>>>>> 61475893edfdf17370d768c2a9f4f62cb6dd7288

enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
};

struct Snake {
    int direction:4;
    int prev_direction:4;
    int x:12;
    int y:12;
    struct Snake *next_segment;
};

struct Head {
    int ate;
    int speed;
    int max_x;
    int max_y;
    int food_x;
    int food_y;
    int length;
    int direction;
    struct Snake *body;
};

// Initialize the head of the snake object.
struct Head *init_snake_head(int max_x, int max_y);

// Initialize the body of the snake object.
struct Snake *init_snake_body(int max_x, int max_y);

// Main game loop.
void mainloop(void);

// Draws the snake body to window.
int draw_snake(struct Head *head);

// Randomly place a piece of snake food.
void place_snake_food(struct Head *head);

// Check if current coordinate is a valid position (i.e. no intersection).
int is_valid_position(struct Snake *segment, int x, int y);

// Check if any snake segments intersects with each other.
int check_segment_intersections(struct Head *head, struct Snake *segment);

// Calls for an update of all snake segments and checks if any food has been
// eaten.
void move_snake(struct Head *head);

// Creates new Snake structure to add to snake.
struct Snake *create_segment(int direction, int x, int y);

// Creates a new segment after snake eats some food.
void add_segment(struct Head *head);

// Frees memory of the snake parts.
void free_snake(struct Head *snake_head);

// Frees memory of the snakes body.
void free_snake_body(struct Snake *body);

// Prints the end game score.
void print_score(int score, int x, int y);

// Asks if another game is to be played.
void play_again(int x, int y);

// Pauses the game.
void pause(void);

#define KEY_N 110
#define KEY_P 112
#define KEY_Q 113
#define KEY_Y 121

#endif
