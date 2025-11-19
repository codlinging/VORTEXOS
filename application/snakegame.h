#ifndef SNAKEGAME_H
#define SNAKEGAME_H
#include<stdint.h>
#define SNAKE_MAX_LENGTH 100
#define GRID_WIDTH 40
#define GRID_HEIGHT 30
#define CELL_SIZE 16
typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;
typedef struct {
    int x;
    int y;
}point_t;

typedef struct snakegame
{
    point_t body[SNAKE_MAX_LENGTH];
    int length;
    direction_t direction;
    point_t food;
    int score;
    int game_over;
    int speed;
    int ticks;
     int needs_redraw;
}snake_game_t;
void snake_init(snake_game_t *game);
void snake_handle_input(snake_game_t *game,char key);
void snake_update(snake_game_t *game);
void snake_draw(snake_game_t *game);
void snake_place_food(snake_game_t *game);
int snake_check_collision(snake_game_t *game);
void snake_draw_borders(void);
void snake_game_loop(void);

#endif
