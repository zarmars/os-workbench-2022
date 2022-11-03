#include <game.h>

#define SIDE 16
static int w, h;

static void init() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
  last_x = init_x = w / 2;
  last_y = init_y = h / 2;
  vx_pixel = 3;
  vy_pixel = 5;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  init();
  for (int x = 0; x * SIDE <= w; x++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

void game_process() {
  int x_update = last_x + vx_pixel;
  if (x_update <= 0) {
    x_update = 0;
    vx_pixel *= -1;
  } else if (x_update * SIDE >= w) {
    x_update = w / SIDE;
    vx_pixel *= -1;
  }
  int y_update = last_y + vy_pixel;
  if (y_update <= 0) {
    y_update = 0;
    vy_pixel *= -1;
  } else if (y_update * SIDE >= h) {
    y_update = h / SIDE;
    vy_pixel *= -1;
  }
  if ((last_x & 1) ^ (last_y & 1))
  {
    draw_tile(last_x * SIDE, last_y * SIDE, SIDE, SIDE, 0xffffff); // white
  } else {
    draw_tile(last_x * SIDE, last_y * SIDE, SIDE, SIDE, 0xffff0f); // white
  }
  last_x = x_update;
  last_y = y_update;

  draw_tile(x_update * SIDE, y_update * SIDE, SIDE, SIDE, 0xf0); // blue
}
