#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TWO_PI 6.28

int ws_row;
int ws_col;
const char cell_char = '#';

void draw_frame(char data[])
{
  printf("\x1b[H");
  for (int i = 0; ws_row * ws_col + 1 > i; i++)
    putchar(i % ws_col ? data[i] : '\n');
}

void clear_screen()
{
  printf("\x1b[2J");
}

bool is_alive(char cell) {
  return cell == cell_char;
}

char neighbour_count(char frame[], int cell_index)
{
  char count = 0;
  const int col = cell_index % ws_col;
  const int row = cell_index / ws_col;
  for (int rp = 0; rp < 3; ++rp) {
    for (int cp = 0; cp < 3; ++cp) {
      if (rp != 1 || cp != 1) {
        const int cr = row - 1 + rp;
        const int cc = col - 1 + cp;
        if (cr >= 0 && cr < ws_row) {
          if (cc >= 0 && cc < ws_col) {
            if (is_alive(frame[cr * ws_col + cc]))
              ++count;
          }
        }
      }
    }
  }
  return count;
}

void update_frame(char frame[])
{
  const int frame_size = ws_col * ws_row;
  char old_state[frame_size];
  memcpy(old_state, frame, frame_size);
  for (int i = 0; i < frame_size; ++i) {
    const char n = neighbour_count(old_state, i);
    const char cell = old_state[i];
    if (is_alive(cell)) {
      if (n != 2 && n != 3)
        frame[i] = ' ';
    } else if (n == 3) {
      frame[i] = cell_char;
    }
  }
}

void random_init(char frame[])
{
  const int alive_theshold = 6;
  const int frame_size = ws_col * ws_row;
  for (int i = 0; i < frame_size; ++i) {
    char c;
    if (rand() % 10 >= alive_theshold)
      c = cell_char;
    else
      c = ' ';
    frame[i] = c;
  }
}

void static_init(char frame[])
{
  // block
  frame[2 *ws_col + 6] = cell_char;
  frame[2 *ws_col + 7] = cell_char;
  frame[3 * ws_col + 6] = cell_char;
  frame[3 * ws_col + 7] = cell_char;

  // blinker
  const int ba = 10 * ws_col + ws_col / 2;
  frame[ba] = cell_char;
  frame[ba + 1] = cell_char;
  frame[ba + 2] = cell_char;

  // toad
  const int ta = 15 * ws_col + 20;
  frame[ta] = cell_char;
  frame[ta + 1] = cell_char;
  frame[ta + 2] = cell_char;
  frame[ws_col + ta + 1] = cell_char;
  frame[ws_col + ta + 2] = cell_char;
  frame[ws_col + ta + 3] = cell_char;

  // glider
  const int ga = 5 * ws_col + 8;
  frame[ga + 1] = cell_char;
  frame[ws_col + ga + 2] = cell_char;
  frame[2 * ws_col + ga] = cell_char;
  frame[2 * ws_col + ga + 1] = cell_char;
  frame[2 * ws_col + ga + 2] = cell_char;

  // gosper glider gun
  const int gg = 10 * ws_col + 60;
  if (ws_col > 95 && ws_row > 18) {
    frame[gg + 24] = cell_char;
    frame[ws_col + gg + 22] = cell_char;
    frame[ws_col + gg + 24] = cell_char;
    frame[2 * ws_col + gg + 12] = cell_char;
    frame[2 * ws_col + gg + 13] = cell_char;
    frame[2 * ws_col + gg + 20] = cell_char;
    frame[2 * ws_col + gg + 21] = cell_char;
    frame[2 * ws_col + gg + 34] = cell_char;
    frame[2 * ws_col + gg + 35] = cell_char;
    frame[3 * ws_col + gg + 11] = cell_char;
    frame[3 * ws_col + gg + 15] = cell_char;
    frame[3 * ws_col + gg + 20] = cell_char;
    frame[3 * ws_col + gg + 21] = cell_char;
    frame[3 * ws_col + gg + 34] = cell_char;
    frame[3 * ws_col + gg + 35] = cell_char;
    frame[4 * ws_col + gg] = cell_char;
    frame[4 * ws_col + gg + 1] = cell_char;
    frame[4 * ws_col + gg + 10] = cell_char;
    frame[4 * ws_col + gg + 16] = cell_char;
    frame[4 * ws_col + gg + 20] = cell_char;
    frame[4 * ws_col + gg + 21] = cell_char;
    frame[5 * ws_col + gg] = cell_char;
    frame[5 * ws_col + gg + 1] = cell_char;
    frame[5 * ws_col + gg + 10] = cell_char;
    frame[5 * ws_col + gg + 14] = cell_char;
    frame[5 * ws_col + gg + 16] = cell_char;
    frame[5 * ws_col + gg + 17] = cell_char;
    frame[5 * ws_col + gg + 22] = cell_char;
    frame[5 * ws_col + gg + 24] = cell_char;
    frame[6 * ws_col + gg + 10] = cell_char;
    frame[6 * ws_col + gg + 16] = cell_char;
    frame[6 * ws_col + gg + 24] = cell_char;
    frame[7 * ws_col + gg + 11] = cell_char;
    frame[7 * ws_col + gg + 15] = cell_char;
    frame[8 * ws_col + gg + 12] = cell_char;
    frame[8 * ws_col + gg + 13] = cell_char;
  }
}

int main(int argc, char *argv[])
{
  srand(time(NULL));
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 2E8;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  ws_col = w.ws_col;
  ws_row = w.ws_row;
  const int buffer_size = ws_col * ws_row;
  char frame[buffer_size];
  clear_screen();
  memset(frame, ' ', buffer_size);
  void (*init)(char[]) = &static_init;
  if (argc == 2) {
    if (strcmp(argv[1], "-r") == 0)
      init = &random_init;
  }
  (*init)(&frame);
  for (;;) {
    draw_frame(frame);
    update_frame(&frame);
    nanosleep(&ts, &ts);
  }
  return 0;
}
