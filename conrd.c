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
            cbuf[ind] = '\0';
            --ind;
            if(!hidden)
                fputs("\b \b", stdout);
        }
        else
            ring;
    }
    printf("\033\133%zuD%s", ind, spacebar);
    for(const char *it = cbuf; *it != '\0'; ++it)
    {
        ch = *it;
        num *= 10;
        if(ch >= '0' && ch <= '9')
            num += ch - '0';
        else if(ch >= 'a' && ch <= 'f')
            num += ch - 'a' + 10;
    }
    return num;
}
