#ifndef _TERM_SETUP_H
#define _TERM_SETUP_H

void panic(const char *fn_name);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

#endif
