#include <game.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
  AM_KEYS(KEYNAME)
};

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
    switch (event.keycode) {
      case AM_KEY_ESCAPE:
        game_over = true;
        break;
      case AM_KEY_INSERT:
        vy_pixel += delta_speed;
        break;
      case AM_KEY_DELETE:
        vy_pixel -= delta_speed;
        break;
      case AM_KEY_HOME:
        vx_pixel -= delta_speed;
        break;
      case AM_KEY_END:
        vx_pixel += delta_speed;
        break;
      default:
        break;
    }
  }
}
