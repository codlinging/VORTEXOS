
#include "../Drivers/keyboard.h"
#include "../Drivers/vesa.h"
#include "snakegame.h"
#include "../Drivers/string.h"
#include "../Drivers/colours.h"
#include "../Interrupts/timer.h"

void snake_init(snake_game_t *game)
{
    game->length = 3;
    game->body[0].x = GRID_WIDTH / 2;
    game->body[0].y = GRID_HEIGHT / 2;
    game->body[1].x = GRID_WIDTH / 2 - 1;
    game->body[1].y = GRID_HEIGHT / 2;
    game->body[2].x = GRID_WIDTH / 2 - 2;
    game->body[2].y = GRID_HEIGHT / 2;
    
    game->direction = DIRECTION_RIGHT;
    game->score = 0;
    game->game_over = 0;
    game->speed = 8;
    game->ticks = 0;
    game->needs_redraw = 1;
    
    snake_place_food(game);
}

void snake_update(snake_game_t *game) {
    if (game->game_over) {
        game->needs_redraw = 1;
        return;
    }
    
    game->ticks++;
    if (game->ticks < game->speed) return;
    game->ticks = 0;
    
    // Store old tail position before moving
    point_t old_tail = game->body[game->length - 1];
    
    // Move snake body (from tail to head)
    for (int i = game->length - 1; i > 0; i--) {
        game->body[i] = game->body[i - 1];
    }
    
    // Move head based on direction
    switch (game->direction) {
        case DIRECTION_UP:
            game->body[0].y--;
            break;
        case DIRECTION_DOWN:
            game->body[0].y++;
            break;
        case DIRECTION_LEFT:
            game->body[0].x--;
            break;
        case DIRECTION_RIGHT:
            game->body[0].x++;
            break;
    }
    
    // Check for collisions
    if (snake_check_collision(game)) {
        game->game_over = 1;
        game->needs_redraw = 1;
        return;
    }
    
    // Check if snake ate food
    if (game->body[0].x == game->food.x && game->body[0].y == game->food.y) {
        // Increase snake length
        if (game->length < SNAKE_MAX_LENGTH) {
            game->length++;
            // New segment starts at tail position
            game->body[game->length - 1] = old_tail;
        }
        
        game->score += 10;
        
        // Increase speed slightly
        if (game->speed > 4 && game->score % 50 == 0) {
            game->speed--;
        }
        
        snake_place_food(game);
        game->needs_redraw = 1;
    } else {
        // When not eating, erase the old tail
        fill_rect(old_tail.x * CELL_SIZE, 
                  old_tail.y * CELL_SIZE, 
                  CELL_SIZE - 1, CELL_SIZE - 1, COLOR_BLACK);
        game->needs_redraw = 1;
    }
}

int snake_check_collision(snake_game_t *game) {
    point_t head = game->body[0];
    
    // Convert grid position to pixel position for collision detection
    uint32_t head_pixel_x = head.x * CELL_SIZE;
    uint32_t head_pixel_y = head.y * CELL_SIZE;
    
    // Collision detection based on visual borders
    // Left border collision: head touches the 2-pixel thick left border
    if (head_pixel_x < 2) {
        return 1;
    }
    // Right border collision: head touches the 2-pixel thick right border
    // The right border starts at (GRID_WIDTH * CELL_SIZE) + 380
    // and the snake head is CELL_SIZE pixels wide
    if (head_pixel_x + CELL_SIZE > (GRID_WIDTH * CELL_SIZE) + 380) {
        return 1;
    }
    // Top border collision: head touches the 5-pixel thick top border
    if (head_pixel_y < 5) {
        return 1;
    }
    // Bottom border collision: head touches the 5-pixel thick bottom border
    // The bottom border starts at (GRID_HEIGHT * CELL_SIZE) + 150
    if (head_pixel_y + CELL_SIZE > (GRID_HEIGHT * CELL_SIZE) + 150) {
        return 1;
    }
    
    // Self collision (skip head)
    for (int i = 1; i < game->length; i++) {
        if (head.x == game->body[i].x && head.y == game->body[i].y) {
            return 1;
        }
    }
    
    return 0;
}

void snake_place_food(snake_game_t *game)
{
    int valid_pos = 0;
    int attempts = 0;
    const int max_attempts = 200;
    
    static uint32_t seed = 0;
    if (seed == 0) {
        seed = get_system_ticks();
    }
    
    while(!valid_pos && attempts < max_attempts) {
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
        
        // Ensure food spawns within playable area (not in border regions)
        // Calculate safe area based on visual borders
        int min_x = 1;  // At least 1 cell from left border
        int max_x = GRID_WIDTH - 2;  // At least 1 cell from right border
        int min_y = 1;  // At least 1 cell from top border  
        int max_y = GRID_HEIGHT - 2; // At least 1 cell from bottom border
        
        game->food.x = min_x + (seed % (max_x - min_x));
        game->food.y = min_y + ((seed * 7) % (max_y - min_y));
        
        valid_pos = 1;
        
        // Check if food overlaps with snake body
        for (int i = 0; i < game->length; i++) {
            if (game->food.x == game->body[i].x && game->food.y == game->body[i].y) {
                valid_pos = 0;
                break;
            }
        }
        
        attempts++;
    }
    
    // Fallback systematic search
    if (!valid_pos) {
        valid_pos = 0;
        for (int y = 1; y < GRID_HEIGHT - 1 && !valid_pos; y++) {
            for (int x = 1; x < GRID_WIDTH - 1 && !valid_pos; x++) {
                game->food.x = x;
                game->food.y = y;
                
                valid_pos = 1;
                for (int i = 0; i < game->length; i++) {
                    if (game->food.x == game->body[i].x && game->food.y == game->body[i].y) {
                        valid_pos = 0;
                        break;
                    }
                }
                
                if (valid_pos) break;
            }
        }
    }
    
    // Final fallback
    if (!valid_pos) {
        game->food.x = GRID_WIDTH / 2;
        game->food.y = GRID_HEIGHT / 2;
        
        for (int i = 0; i < game->length; i++) {
            if (game->food.x == game->body[i].x && game->food.y == game->body[i].y) {
                if (game->food.x + 1 < GRID_WIDTH - 1) {
                    game->food.x++;
                } else if (game->food.y + 1 < GRID_HEIGHT - 1) {
                    game->food.y++;
                }
                break;
            }
        }
    }
}

void snake_draw(snake_game_t *game)
{
    // Only redraw if needed
    if (!game->needs_redraw) return;
    
    // Draw snake head (always draw head as it moves every frame)
    fill_rect(game->body[0].x * CELL_SIZE, 
              game->body[0].y * CELL_SIZE, 
              CELL_SIZE - 1, CELL_SIZE - 1, 0xFF00FF00);
    
    // Draw second segment (this becomes visible after head moves)
    if (game->length > 1) {
        fill_rect(game->body[1].x * CELL_SIZE, 
                  game->body[1].y * CELL_SIZE, 
                  CELL_SIZE - 1, CELL_SIZE - 1, 0xFF008800);
    }
    
    // Draw food (only if it needs to be redrawn)
    fill_rect(game->food.x * CELL_SIZE, 
              game->food.y * CELL_SIZE, 
              CELL_SIZE - 1, CELL_SIZE - 1, COLOR_RED);
    
    // Draw score
    char score_text[32];
    mystrcpy(score_text, "Score: ");
    char score_num[16];
    int score = game->score;
    int pos = 0;
    
    if (score == 0) {
        score_num[pos++] = '0';
    } else {
        char temp[16];
        int temp_pos = 0;
        while (score > 0) {
            temp[temp_pos++] = '0' + (score % 10);
            score /= 10;
        }
        for (int i = temp_pos - 1; i >= 0; i--) {
            score_num[pos++] = temp[i];
        }
    }
    score_num[pos] = '\0';
    mystrcat(score_text, score_num);
    
    // Clear previous score area and redraw
    fill_rect(10, GRID_HEIGHT * CELL_SIZE + 160, 200, 30, COLOR_BLACK);
    draw_string_scaled(10, GRID_HEIGHT * CELL_SIZE + 160, score_text, COLOR_WHITE, 2);
    
    if (game->game_over) {
        // Clear the game over area first to avoid overlapping text
        uint32_t center_x = GRID_WIDTH * CELL_SIZE / 2;
        uint32_t center_y = GRID_HEIGHT * CELL_SIZE / 2;
        fill_rect(center_x - 120, center_y - 20, 240, 80, COLOR_BLACK);
        
        draw_string(center_x - 80, center_y - 10, 
                   "GAME OVER!", 0xFFFF0000);
        draw_string(center_x - 100, center_y + 10, 
                   "Press R to restart", 0xFFFFFF00);
        draw_string(center_x - 100, center_y + 30, 
                   "Press ESC to exit", 0xFFFFFF00);
    } else {
        draw_string_scaled(260, GRID_HEIGHT * CELL_SIZE + 170, "SNAKE GAME !!", COLOR_PASTEL_RED, 4);
    }
    
    game->needs_redraw = 0;
}

void snake_draw_borders(void) {
    uint32_t border_color = COLOR_DARK_RED;
    
    // Draw borders - these define the collision boundaries
    // Top border (5 pixels thick)
    fill_rect(0, 0, (GRID_WIDTH * CELL_SIZE) + 384, 5, border_color);
    // Bottom border (5 pixels thick)
    fill_rect(0, (GRID_HEIGHT * CELL_SIZE) + 150, (GRID_WIDTH * CELL_SIZE) + 384, 5, border_color);
    // Left border (2 pixels thick)
    fill_rect(0, 0, 2, (GRID_HEIGHT * CELL_SIZE) + 150, border_color);
    // Right border (2 pixels thick)
    fill_rect((GRID_WIDTH * CELL_SIZE) + 380, 0, 2, (GRID_HEIGHT * CELL_SIZE) + 150, border_color);
}
void snake_handle_input(snake_game_t *game, char key) {
    if (game->game_over) {
        if (key == 'r' || key == 'R') {
            // Completely clear and redraw the entire game area
            uint32_t total_width = (GRID_WIDTH * CELL_SIZE) + 384;
            uint32_t total_height = (GRID_HEIGHT * CELL_SIZE) + 155;
            
            // Clear everything inside the borders
            fill_rect(2, 2, total_width - 4, total_height - 4, COLOR_BLACK);
            
            // Redraw the borders
            snake_draw_borders();
            
            snake_init(game);
            game->needs_redraw = 1;
        }
        return;
    }
    
    switch (key) {
        case 'w': case 'W':
            if (game->direction != DIRECTION_DOWN)
                game->direction = DIRECTION_UP;
            break;
        case 's': case 'S':
            if (game->direction != DIRECTION_UP)
                game->direction = DIRECTION_DOWN;
            break;
        case 'a': case 'A':
            if (game->direction != DIRECTION_RIGHT)
                game->direction = DIRECTION_LEFT;
            break;
        case 'd': case 'D':
            if (game->direction != DIRECTION_LEFT)
                game->direction = DIRECTION_RIGHT;
            break;
    }
}

void snake_game_loop(void) {
    fill_screen(COLOR_BLACK);
    draw_string_scaled(270, 320, "PRESS ENTER", COLOR_BLUE, 6);
    draw_string_scaled(265, 315, "PRESS ENTER", COLOR_WHITE, 6);
    draw_string_scaled(375, 550, "Press ESC to exit", COLOR_PASTEL_RED, 2);

    while(1) {
        if(keyboard_has_input()) {   
            char c = keyboard_getchar();
            if(c == 0x1B) break;
            if(c == '\n') {
                snake_game_t game;
                snake_init(&game);
                
                fill_screen(COLOR_BLACK);
                snake_draw_borders();
                
                // Initial draw
                for(int i = 0; i < game.length; i++) {
                    uint32_t color = (i == 0) ? 0xFF00FF00 : 0xFF008800;
                    fill_rect(game.body[i].x * CELL_SIZE, 
                              game.body[i].y * CELL_SIZE, 
                              CELL_SIZE - 1, CELL_SIZE - 1, color);
                }
                fill_rect(game.food.x * CELL_SIZE, 
                          game.food.y * CELL_SIZE, 
                          CELL_SIZE - 1, CELL_SIZE - 1, COLOR_RED);
                
                uint32_t last_frame_time = get_system_ticks();
                
                while(1) {
                    // Handle input without blocking
                    while(keyboard_has_input()) {
                        char c = keyboard_getchar();
                        if(c == 0x1B) goto exit_game;
                        snake_handle_input(&game, c);
                    }
                    
                    // Frame rate limiting (adjust as needed)
                    uint32_t current_time = get_system_ticks();
                    if (current_time - last_frame_time >= 1) { // ~60 FPS
                        snake_update(&game);
                        snake_draw(&game);
                        last_frame_time = current_time;
                    }
                    
                    // Brief pause to prevent CPU hogging
                    asm volatile("hlt");
                }
                exit_game:
                break;
            }
        }
        asm volatile("hlt");
    }
}