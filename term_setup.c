#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios default_attrs;

void panic(const char *fn_name) {
  perror(fn_name);
  exit(1);
}

void enable_raw_mode() {

  // load the terminal config into term_attrs
  struct termios term_attrs = default_attrs;

  int input_features_toggle =
      IXON  | // disable software control flow
      ICRNL   // prevent carriage return (ctrl+m) from being translated to newline on input
      ;

  int output_features_toggle =
      OPOST   // disable all output processing
      ;

  int local_features_toggle =
      ECHO  | // disable printing of the keys when they are pressed
      ICANON| // read byte by byte instead of lines
      ISIG  | // disable INTR, QUIT, SUSP, or DSUSP signals
      IEXTEN| // disable ctrl+v. Also disables ctrl+o (MacOS only)
      BRKINT| // probably not needed in this day and age, but it's there for peace of mind
      INPCK | // same as above
      ISTRIP  // same as above
      ;

  // toggle the different flag bits to turn off the features
  term_attrs.c_lflag &= ~local_features_toggle;
  term_attrs.c_oflag &= ~output_features_toggle;
  term_attrs.c_iflag &= ~input_features_toggle;

  // set character size to 8 bytes (probably the default already)
  term_attrs.c_cflag |= CS8;

  // set the minimum requirement of chars for the read function to return to 0
  term_attrs.c_cc[VMIN] = 0;

  // update the terminal config
  int result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attrs);

  if (result == -1) { panic("tcsetattr"); }
}

void init_default_attrs() { 
    int result = tcgetattr(STDIN_FILENO, &default_attrs); 

    if (result == -1) { panic("tcgetattr"); }
}

void restore_term() { 
    int result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_attrs); 
    
    if (result == -1) { panic("tcsetattr"); }
}

void setup_term() {
  init_default_attrs();

  enable_raw_mode();

  atexit(restore_term);
}

