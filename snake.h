#ifndef SNAKE_H
#define SNAKE_H


#define SNAKE_INIT_LEN 4

#define SEGMENT_WIDTH 5
#define SEGMENT_HEIGHT 5

#define GAME_SPEED 150

enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
};

struct Snake {
    int x;
    int y;
    int direction;
    int prev_direction;
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
void mainloop(struct Head *head);

// Draws the snake body to window.
int draw_snake(struct Head *head);

// Randomly place a piece of snake food.
void place_snake_food(struct Head *head);

// Check if current coordinate is a valid position (i.e. no intersection).
int is_valid_position(struct Snake *segment, int x, int y);

// Check if snake intersects with itself.
int check_segment_intersections(struct Head *head, struct Snake *segment);

// Move snake along.
void move_snake(struct Head *head);

// Change snake segment coordinates.
void update_segment_direction(struct Head *head);

// Changes the snakes position depending on direction.
void update_segment_coordinates(struct Snake *segment);

// Create segment of snake.
struct Snake *create_segment(int direction, int x, int y);

// Creates a new segment after snake eats some food.
void add_segment(struct Head *head);

// Frees memory of the snake parts.
void free_snake(struct Head *snake_head);

// Frees memory of the snakes body.
void free_snake_body(struct Snake *body);

#define KEY_Q 113

#endif
