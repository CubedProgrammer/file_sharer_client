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
    for(ch = gch; ch != RETURNKEY; ch = gch)
    {
        if(ch >= '0' && ch <= '9')
        {
            if(ind == 16)
                ring;
            else
                cbuf[ind] = ch, ++ind;
        }
        else if(ch >= 'a' && ch <= 'f')
        {
        }
        else if(ch == HIDE)
        {
            if(hidden)
                printf("\033\133%zuD%s", ind, spacebar);
        }
        else if(ch == BACKSPACE)
        {
            cbuf[ind] = '\0';
            --ind;
            fputs("\b \b", stdout);
        }
        else
            ring;
    }
    return num;
}
