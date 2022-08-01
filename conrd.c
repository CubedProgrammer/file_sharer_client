// This file is part of file_sharer_client.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_client is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_client is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_client. If not, see <https://www.gnu.org/licenses/>. 

#include<stdio.h>
#include"conrd.h"

uint64_t rdnum(void)
{
    uint64_t num = 0;
    char cbuf[17];
    cbuf[0] = '\0';
    char spacebar[] = "                   ";
    int ch, hidden = 1;
    size_t ind = 0;
    for(ch = gchlo; ch != RETURNKEY; ch = gchlo)
    {
        if((ch >= '0' && ch <= '9')||(ch >= 'a' && ch <= 'f'))
        {
            if(ind == 16)
                ring;
            else
            {
                cbuf[ind] = ch;
                if(!hidden)
                    putchar(ch);
                ++ind;
                cbuf[ind] = '\0';
            }
        }
        else if(ch == HIDE)
        {
            hidden = !hidden;
            if(hidden)
                printf("\033\133%zuD%s\033\133%zuD", ind, spacebar, sizeof(spacebar) - 1);
            else
                printf("%s", cbuf);
        }
        else if(ch == BACKSPACE && ind)
        {
            --ind;
            cbuf[ind] = '\0';
            if(!hidden)
                fputs("\b \b", stdout);
        }
        else
            ring;
    }
    for(const char *it = cbuf; *it != '\0'; ++it)
    {
        ch = *it;
        num *= 16;
        if(ch >= '0' && ch <= '9')
            num += ch - '0';
        else if(ch >= 'a' && ch <= 'f')
            num += ch - 'a' + 10;
    }
    return num;
}
