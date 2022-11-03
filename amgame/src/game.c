#include <game.h>

bool game_over = false;
int init_x = 0;
int init_y = 0;
int last_x = 0;
int last_y = 0;
int vx_pixel = 0;
int vy_pixel = 0;
int delta_speed = 5;

// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");

  splash();

  puts("Press any key to see its key code...\n");
  while (!game_over) {
    game_process();
    print_key();
  }
  return 0;
}