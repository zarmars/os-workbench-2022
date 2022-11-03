#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdbool.h>

void splash();
void print_key();
void game_process();
static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

extern bool game_over;
extern int init_x;  // initial x position in screen.
extern int init_y;  // initial y position in screen.
extern int last_x;  // x position in last frame.
extern int last_y;  // y position in last frame.
extern int vx_pixel;  // moving speed in x direction, unit pixel/frame.
extern int vy_pixel;  // moving speed in y direction, unit pixel/frame.
extern int delta_speed;  // speed change w.r.t. keyboard input.