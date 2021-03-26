#ifndef _M65_CONIO_H_
#define _M65_CONIO_H_

#define COLOR_BLACK        0
#define COLOR_WHITE        1
#define COLOR_RED          2
#define COLOR_CYAN         3
#define COLOR_PURPLE       4
#define COLOR_GREEN        5
#define COLOR_BLUE         6
#define COLOR_YELLOW       7
#define COLOR_ORANGE       8
#define COLOR_BROWN        9
#define COLOR_PINK         10
#define COLOR_GRAY1        11
#define COLOR_DARKGRAY     11
#define COLOR_GRAY2        12
#define COLOR_LIGHTGREEN   13
#define COLOR_LIGHTBLUE    14
#define COLOR_GRAY3        15

#define cprintf printf

void clrscr(void); 

void cputs(const unsigned char* s);

unsigned char cgetc (void);

void cclear(unsigned char length);

#endif // _M65_CONIO_H_
