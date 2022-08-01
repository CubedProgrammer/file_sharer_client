// This file is part of file_sharer_client.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_client is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_client is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_client. If not, see <https://www.gnu.org/licenses/>. 

#ifndef Included_conrd_h
#define Included_conrd_h
#include<ctype.h>
#include<stdint.h>
#ifdef _WIN32
#include<conio.h>
#define RETURNKEY '\r'
#define BACKSPACE '\b'
#define gch getch()
#define gchlo tolower(getch())
#else
#define BACKSPACE 0177
#define RETURNKEY '\n'
#define gch getchar()
#define gchlo tolower(getchar())
#endif
#define HIDE 2
#define ring putchar('\a')

uint64_t rdnum(void);

#endif
