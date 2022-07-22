#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"conrd.h"
#include"msg.h"

void receipient(int sock)
{
    puts("You are waiting to receive a file from your host.");
}

void uploader(int sock)
{
    puts("Type in the name of a file and hit enter.");
    puts("Press Ctrl+X to close room.");
    fs_msg_t msgt;
    char filename[2601];
    size_t bufsz, fnlen = 0, fnbufsz = sizeof filename;
    --fnbufsz;
    FILE *fh;
    char fszbuf[8], bufszstr[13], *envstr;
    void *buf;
    long beginpos, endpos;
    long bu, br;
    long nxupd;
    int ch;
    for(ch = gch; ch != 24; ch = gch)
    {
        switch(ch)
        {
            case'\t':
                break;
            case BACKSPACE:
                if(fnlen)
                {
                    --fnlen;
                    filename[fnlen] = '\0';
                    fputs("\b \b", stdout);
                }
                else
                    ring;
                break;
            case RETURNKEY:
                putchar('\n');
                fh = fopen(filename, "r");
                if(fh != NULL)
                {
                    printf("Reading file %s\n", filename);
                    beginpos = ftell(fh);
                    fseek(fh, 0, SEEK_END);
                    endpos = ftell(fh);
                    fseek(fh, 0, SEEK_SET);
                    envstr = getenv("SHAREBUFSZ");
                    endpos -= beginpos;
                    if(envstr == NULL)
                        bufsz = 16384;
                    else
                    {
                        strcpy(bufszstr, envstr);
                        bufsz = atoi(bufszstr);
                    }
                    if((long)bufsz > endpos)
                        bufsz = endpos;
                    buf = malloc(bufsz);
                    if(buf == NULL)
                        fputs("Memory allocation failed.\n", stderr);
                    else
                    {
                        msgt = SENDFILE;
                        PUTOBJ(sock, msgt);
                        nxupd = bu = 0;
                        while(!feof(fh))
                        {
                            br = fread(buf, 1, bufsz, fh);
                            bu += write(sock, buf, br);
                            if(bu > nxupd)
                            {
                                nxupd += 1048576;
                                printf("\rUploaded %ld bytes.", bu);
                            }
                        }
                        printf("\rUploaded %ld bytes.\n", bu);
                        free(buf);
                    }
                    fclose(fh);
                }
                else
                    perror("Could not open file");
                fnlen = 0;
                filename[fnlen] = '\0';
                break;
            default:
                if(ch >= ' ')
                {
                    if(fnlen < fnbufsz)
                    {
                        filename[fnlen] = ch;
                        ++fnlen;
                        filename[fnlen] = '\0';
                        putchar(ch);
                    }
                    else
                        ring;
                }
                else
                    ring;
        }
    }
    msgt = CLOSEROOM;
    PUTOBJ(sock, msgt);
    puts("You are no longer sharing.");
}
