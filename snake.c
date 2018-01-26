#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ncurses.h>

#include "snake.h"

void print_usage(void)
{
    printf("./snake [color_directive] or [print-usage]\n");
    printf("  Color-Directives:\n");
    printf("\t-c Cyan\n");
    printf("\t-y Yellow\n");
    printf("\t-m Magenta\n");
    printf("\t-w White\n");
    printf("  Print-Usage:\n");
    printf("\t-h Help\n");

    exit(1);
}

int main(int argc, char *argv[])
{
    short color = COLOR_BLUE;
    opterr = 0;

    switch (getopt(argc, argv, "+rgcymwh")) {

        case ('r'): {
            color = COLOR_RED;
            break;
        }

        case ('g'): {
            color = COLOR_GREEN;
            break;
        }

        case ('c'): {
            color = COLOR_CYAN;
            break;
        }

        case ('y'): {
            color = COLOR_YELLOW;
            break;
        }

        case ('m'): {
            color = COLOR_MAGENTA;
            break;
        }

        case ('w'): {
            color = COLOR_WHITE;
            break;
        }

        case ('h'): { 
            print_usage();
            break;
        }

        case ('?'):
            OPT_ERROR(argv[1][1]);
    }

    initscr();
    start_color();
    
    noecho();
    cbreak();
 
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    init_pair(1, color, COLOR_BLACK); 
    attron(COLOR_PAIR(1));

    mainloop();

    endwin();

    return 0;
}

static inline void update_segment_direction(struct Head *head)
{
    struct Snake *current = head->body;

    current->prev_direction = current->direction;
    current->direction = head->direction;

    while (current->next_segment != NULL) {
        int direction = current->prev_direction;
        current = current->next_segment;
        current->prev_direction = current->direction;
        current->direction = direction;
    }
}

static inline void update_segment_coordinates(struct Snake *segment)
{
    switch (segment->direction) {

        case (NORTH): {
            segment->y--;
            break;
        }

        case (SOUTH): {
            segment->y++;
            break;
        }

        case (EAST): {
            segment->x++;
            break;
        }

        case (WEST): {
            segment->x--;
            break;
        }
    }
}

void mainloop(void)
{
    int max_x, max_y;

    int game_speed = 100;

    getmaxyx(stdscr, max_y, max_x);

    struct Head *head = init_snake_head(max_x, max_y);

    while ( 1 ) {

        move_snake(head);

        if (draw_snake(head))
            break;

        refresh();

        if (head->ate) {
            head->ate = 0;
            place_snake_food(head);
            game_speed = (head->length - 4) % 5 == 0 && game_speed > 25 ? 
                          game_speed - 5 : game_speed;
        } else
            mvaddch(head->food_y, head->food_x, ACS_PI);

        switch (getch()) {

            case (KEY_UP): {
                if (head->direction != SOUTH) 
                    head->direction = NORTH;
                break;
            }

            case (KEY_DOWN): {
                if (head->direction != NORTH)
                    head->direction = SOUTH;
                break;
            }

            case (KEY_LEFT): {
                if (head->direction != EAST)
                    head->direction = WEST;
                break;
            }

            case (KEY_RIGHT): {
                if (head->direction != WEST)
                    head->direction = EAST;
                break;
            }

            case (KEY_P): {
                pause();
                break;
            }

            case (KEY_Q): {
                free_snake(head);
                return;
            }

            default: 
                break;
        }

        // flash/beep
        //delay_output(game_speed);
        napms(game_speed);
        update_segment_direction(head);

        getmaxyx(stdscr, max_y, max_x);
        head->max_x = max_x;
        head->max_y = max_y;

        clear();
    }

    int score = head->length - 5;
    int print_x = (head->max_x - 8) / 2;
    int print_y = head->max_y / 2;

    free_snake(head);

    print_score(score, print_x, print_y);
}

int draw_snake(struct Head *head)
{
    struct Snake *current = head->body;

    int max_x = head->max_x;
    int max_y = head->max_y;

    while (current != NULL) {

        if (current->x >= max_x || current->y >= max_y || 
            current->x < 0 || current->y < 0)
            return 1; 

        if (check_segment_intersections(head, current))
            return 1;

        mvaddch(current->y, current->x, ACS_DIAMOND);
        current = current->next_segment;
    }

    return 0;
}

void add_segment(struct Head *head)
{
    struct Snake *segment = head->body;
    head->length++;

    while (segment->next_segment != NULL)
        segment = segment->next_segment;

    int x = segment->x;
    int y = segment->y;

    switch (segment->direction) {

        case (NORTH): {
            y++;
            break;
        }

        case (SOUTH): {
            y--;
            break;
        }

        case (EAST): {
            x--;
            break;
        }

        case (WEST): {
            x++; 
            break;
        }
    }

    segment->next_segment = create_segment(segment->prev_direction, x, y);
}

int check_segment_intersections(struct Head *head, struct Snake *segment)
{
    struct Snake *current = head->body;

    int segment_x = segment->x;
    int segment_y = segment->y;

    while (current != NULL) {

        if (!(current == segment) &&
            segment_x == current->x && 
            segment_y == current->y)
            return 1;

        current = current->next_segment;
    }    

    return 0;
}

void move_snake(struct Head *head)
{
    struct Snake *current = head->body;
   
    int food_x = head->food_x;
    int food_y = head->food_y;

    while (current != NULL) {
        update_segment_coordinates(current);

        if (current->x == food_x && current->y == food_y) 
            head->ate = 1;

        current = current->next_segment;
    }

    if (head->ate)
        add_segment(head);
}

void place_snake_food(struct Head *head)
{
    struct Snake *segment = head->body;
    int food_x, food_y;

    while ( 1 ) {
        food_x = rand() % head->max_x;
        food_y = rand() % head->max_y;        

        if (is_valid_position(segment, food_x, food_y) == 0)
            break;
    }

    mvaddch(food_y, food_x, ACS_PI);
    head->food_x = food_x;
    head->food_y = food_y;
}

int is_valid_position(struct Snake *segment, int x, int y)
{
    for (; segment != NULL; segment=segment->next_segment)
        if (segment->x == x && segment->y == y)
            return 1;
    return 0;
}

struct Head *init_snake_head(int max_x, int max_y)
{
    struct Head *new_head = malloc(sizeof *new_head);
    new_head->max_x = max_x;
    new_head->max_y = max_y;
    new_head->direction = WEST;
    new_head->length = SNAKE_INIT_LEN;
    new_head->ate = 1;
    new_head->body = init_snake_body(max_x, max_y);

    return new_head;
}

struct Snake *init_snake_body(int max_x, int max_y)
{
    int start_x, start_y;

    start_x = max_x / 2;
    start_y = max_y / 2;

    struct Snake *segment = create_segment(WEST, start_x, start_y);

    struct Snake *current = segment;

    for (int i=0; i < SNAKE_INIT_LEN - 1; i++) {
        struct Snake *next = create_segment(current->prev_direction,
                                            current->x + 1, current->y);
        current->next_segment = next;
        current = current->next_segment;
    }

    return segment;
}

struct Snake *create_segment(int direction, int x, int y)
{
    struct Snake *segment = malloc(sizeof *segment);
    segment->next_segment = NULL;
    segment->prev_direction = direction;
    segment->direction = direction;
    segment->x = x;
    segment->y = y;

    return segment;
}

void free_snake(struct Head *snake_head)
{
    free_snake_body(snake_head->body);
    free(snake_head);
}

void free_snake_body(struct Snake *body)
{
    while (body != NULL) {
        struct Snake *prev_segment = body;
        body = body->next_segment;
        free(prev_segment);
    }
}

void pause(void)
{
    while ( 1 ) {

        int key = getch();

        switch (key) {

            case (KEY_P): {
                return;
            }

            case (KEY_Q): {
                ungetch(key);
                return;
            }
        }
    }
}

void print_score(int score, int x, int y)
{
    clear();
    mvprintw(y, x, "SCORE: %d", score);
    refresh();
    play_again(x, y + 2);
}

void play_again(int x, int y)
{
    mvprintw(y, x - 5, "PLAY AGAIN? (Y/N)");

    while ( 1 ) {
        switch (getch()) {

            case (KEY_Y): {
                clear();
                mainloop();            
                break;
            }

            case (KEY_N):
                return;

            default:
                break;
        }
    }
}

