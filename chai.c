#include <unistd.h>

#include "term_setup.h"

void main_loop() {
  char c;

  for (;;) {
    read(STDIN_FILENO, &c, 1);
    if (c == 'q')
      break;
  };
}


int main() {
  setup_term();

  main_loop();

  restore_term();
  return 0;
}
