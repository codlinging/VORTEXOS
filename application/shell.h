#ifndef SHELL_H
#define SHELL_H
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 16
#define MAX_HISTORY 50
void init_shell(void);
void shell_loop(void);
void execute_command(char *input);
void print_prompt(void);
void draw_shell_icon(int x,int y);
#endif