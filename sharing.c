// This file is part of file_sharer_client.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_client is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_client is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_client. If not, see <https://www.gnu.org/licenses/>. 

#include<dirent.h>
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
    char matches[2601];
    size_t bufsz, fnlen = 0, fnbufsz = sizeof filename;
    --fnbufsz;
    char *slashp, ispref;
    char *ita, *itb;
    char *nextmatch;
    DIR *dir;
    struct dirent *den;
    unsigned matchcnt;
    FILE *fh;
    char fszbuf[8], bufszstr[13];
    char *envstr;
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
                slashp = strrchr(filename, '/');
                if(slashp == NULL)
                    dir = opendir("."), slashp = filename;
                else
                {
                    *slashp = '\0';
                    dir = opendir(filename);
                    *slashp = '/';
                    ++slashp;
                }
                if(dir == NULL)
                    perror("Autocomplete failed");
                else
                {
                    den = readdir(dir);
                    nextmatch = matches;
                    matchcnt = 0;
                    while(den != NULL)
                    {
                        if(0 == strcmp(den->d_name, ".") || strcmp(den->d_name, "..") == 0)
                            goto next;
                        for(ita = slashp, itb = den->d_name; (ispref = *ita == *itb) && *ita != '\0' && *itb != '\0'; ++ita, ++itb);
                        ispref = *ita == '\0';
                        if(ispref)
                        {
                            ++matchcnt;
                            strcpy(nextmatch, den->d_name);
                            for(; *nextmatch; ++nextmatch);
                            *nextmatch++ = ' ';
                            *nextmatch = '\0';
                        }
                        next:
                        den = readdir(dir);
                    }
                    if(matchcnt == 0)
                        ring;
                    else if(matchcnt == 1)
                    {
                        *--nextmatch = '\0';
                        strcpy(filename, matches);
                        printf("\r%s", filename);
                        fnlen = strlen(filename);
                    }
                    else
                        printf("\r%s\n%s", matches, filename);
                }
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
