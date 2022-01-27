#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
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

void initialize(char frame[])
{
  const int a = 10 * ws_col + ws_col / 2;
  frame[a] = cell_char;
  frame[a + 1] = cell_char;
  frame[a + 2] = cell_char;
}

int main(int argc, char *argv[])
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  ws_col = w.ws_col;
  ws_row = w.ws_row;
  const int buffer_size = ws_col * ws_row;
  char frame[buffer_size];
  clear_screen();
  memset(frame, ' ', buffer_size);
  initialize(&frame);
  for (;;) {
    draw_frame(frame);
    update_frame(&frame);
    sleep(1);
  }
  return 0;
}
