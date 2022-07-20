#ifndef Included_conrd_h
#define Included_conrd_h
#include<ctype.h>
#include<stdint.h>
#ifdef _WIN32
#include<conio.h>
#define RETURNKEY '\r'
#define BACKSPACE '\b'
#define gch tolower(getch())
#else
#define BACKSPACE 0177
#define RETURNKEY '\n'
#define gch tolower(getchar())
#endif
#define HIDE 1
#define ring putchar('\a')

uint64_t rdnum(void);

#endif
