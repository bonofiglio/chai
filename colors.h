#ifndef _COLORS_H
#define _COLORS_H

#define COLOR_STR_SIZE 5
#define RESET_STR_SIZE 4

static const char RED[COLOR_STR_SIZE] = {'\x1B', '[', '3', '1', 'm'};
static const char GRN[COLOR_STR_SIZE] = {'\x1B', '[', '3', '2', 'm'};
static const char BLU[COLOR_STR_SIZE] = {'\x1B', '[', '3', '4', 'm'};
static const char MAG[COLOR_STR_SIZE] = {'\x1B', '[', '3', '5', 'm'};
static const char CYN[COLOR_STR_SIZE] = {'\x1B', '[', '3', '6', 'm'};
static const char WHT[COLOR_STR_SIZE] = {'\x1B', '[', '3', '7', 'm'};
static const char RESET[RESET_STR_SIZE] = {'\x1B', '[', '0', 'm'};

#endif
