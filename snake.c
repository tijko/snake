#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>

#include "snake.h"


int main(int argc, char *argv[])
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    struct Head *snake_head = init_snake_head(max_x, max_y);

    mainloop(snake_head);
    print_score(snake_head);
    free_snake(snake_head);

    endwin();
    
    return 0;
}

void mainloop(struct Head *head)
{
    int run_loop = 1;

    while ( run_loop ) {

        move_snake(head);

        if (draw_snake(head))
            break;

        refresh();

        if (head->ate) {
            head->ate = 0;
            place_snake_food(head);
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

            case (KEY_Q): {
                run_loop = 0;
                break;
            }

            default: 
                break;
        }

        // flash/beep
        delay_output(GAME_SPEED);
        update_segment_direction(head);

        clear();
    }
}

int draw_snake(struct Head *head)
{
    struct Snake *current = head->body;

    while (current != NULL) {

        if (current->x >= head->max_x || current->y >= head->max_y ||
            current->x < 0 || current->y < 0)
            return 1;

        if (check_segment_intersections(head, current))
            return 1;

        mvaddch(current->y, current->x, ACS_BLOCK);
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

    while (current != NULL) {

        if (segment == current) {
            current = current->next_segment;
            continue;
        }

        if (segment->x == current->x && segment->y == current->y)
            return 1;

        current = current->next_segment;
    }    

    return 0;
}

void move_snake(struct Head *head)
{
    struct Snake *current = head->body;
   
    while (current != NULL) {
        update_segment_coordinates(current);

        if (current->x == head->food_x && current->y == head->food_y) 
            head->ate = 1;

        current = current->next_segment;
    }

    if (head->ate)
        add_segment(head);
}

void update_segment_direction(struct Head *head)
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

void update_segment_coordinates(struct Snake *segment)
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

void place_snake_food(struct Head *head)
{
    struct Snake *segment = head->body;
    int food_x, food_y;

    while ( 1 ) {
        food_x = rand() % head->max_x;
        food_y = rand() % head->max_y;        

        if (is_valid_position(segment, food_x, food_y) == 0);
            break;
    }

    mvaddch(food_y, food_x, ACS_PI);
    head->food_x = food_x;
    head->food_y = food_y;
}

int is_valid_position(struct Snake *segment, int x, int y)
{
    while (segment != NULL) {

        if (segment->x == x && segment->y == y)
            return 1;

        segment = segment->next_segment;
    }

    return 0;
}

struct Head *init_snake_head(int max_x, int max_y)
{
    struct Head *new_head = malloc(sizeof *new_head);
    new_head->max_x = max_x;
    new_head->max_y = max_y;
    new_head->direction = WEST;
    new_head->length = SNAKE_INIT_LEN;
    new_head->length = 4;
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
        struct Snake *next = body->next_segment;
        free(body);
        body = next;
    }
}

void print_score(struct Head *head)
{
    clear();
    mvprintw(head->max_y / 2, (head->max_x  - 8)/ 2, "SCORE: %d", head->length - 4);
    refresh();
    sleep(2);
}
