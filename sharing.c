// This file is part of file_sharer_client.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_client is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_client is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_client. If not, see <https://www.gnu.org/licenses/>. 

#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/select.h>
#include<time.h>
#include<unistd.h>
#include"conrd.h"
#include"msg.h"

void receipient(int sock, char display)
{
    puts("You are waiting to receive a file from your host.");
    int in = STDIN_FILENO;
    struct timeval tv, *tvp = &tv;
    fd_set fds, *fdsp = &fds;
    int ready;
    char unsigned szstr[8];
    char bufszstr[13], *rawbufsz;
    void *buf;
    size_t fsz, tot;
    size_t displaymark, bufsz;
    double prog;
    ssize_t bc;
    const char fname[] = "file_sharer_temporary_file";
    FILE *fh;
    char cmd, msgt;
    char msgnam[13];
    char ended = 0;
    while(!ended)
    {
        tv.tv_usec = 0;
        tv.tv_sec = 1800;
        FD_ZERO(fdsp);
        FD_SET(in, fdsp);
        FD_SET(sock, fdsp);
        ready = select(sock + 1, fdsp, NULL, NULL, tvp);
        if(FD_ISSET(in, fdsp))
        {
            cmd = gchlo;
            if(cmd == 'q')
            {
                msgt = QUIT;
                PUTOBJ(sock, msgt);
                ended = 1;
            }
            else
                ring;
        }
        if(FD_ISSET(sock, fdsp))
        {
            GETOBJ(sock, msgt);
            if(msgt == SENDFILE)
            {
                bc = read(sock, szstr, sizeof szstr);
                if(bc < 8)
                    puts("Server did not send enough data.");
                else
                {
                    rawbufsz = getenv("SHAREBUFSZ");
                    if(rawbufsz == NULL)
                        bufsz = 16384;
                    else
                    {
                        strcpy(bufszstr, rawbufsz);
                        bufsz = atoi(bufszstr);
                    }
                    fsz = 0;
                    tot = 0;
                    for(ssize_t i = 0; i < bc; ++i)
                        fsz <<= 8, fsz += szstr[i];
                    displaymark = 1048576;
                    if(bufsz < fsz)
                        bufsz = fsz;
                    buf = malloc(bufsz);
                    if(buf == NULL)
                        perror("Memory allocation failed.");
                    else
                    {
                        if(display)
                            fh = stdout;
                        else
                        {
                            fh = fopen(fname, "w");
                            putchar('\n');
                            if(fh == NULL)
                                perror("Could not open file for writing");
                        }
                        bc = read(sock, buf, bufsz);
                        tot += bc;
                        fwrite(buf, 1, bufsz, fh);
                        while(bc == bufsz)
                        {
                            bc = read(sock, buf, bufsz);
                            tot += bc;
                            fwrite(buf, 1, bc, fh);
                            if(!display && tot > displaymark)
                            {
                                prog = tot;
                                prog /= fsz;
                                printf("\r%zu/%zu, %.1f%% complete.\n", tot, fsz, prog * 100);
                                displaymark += 1048576;
                            }
                        }
                        if(!display)
                        {
                            prog = tot;
                            prog /= fsz;
                            printf("\r%zu/%zu, %.1f%% complete.\n", tot, fsz, prog * 100);
                            fclose(fh);
                        }
                        free(buf);
                    }
                }
            }
            else
            {
                get_msg_name(msgt, msgnam);
                printf("Server sent invalid message: %s\n", msgnam);
            }
        }
    }
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
    char unsigned fszbuf[8];
    char bufszstr[13];
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
                    for(size_t i = 0; i < sizeof(fszbuf); ++i)
                        fszbuf[i] = (long unsigned)endpos >> (sizeof(fszbuf) - i - 1) * 8;
                    buf = malloc(bufsz);
                    if(buf == NULL)
                        fputs("Memory allocation failed.\n", stderr);
                    else
                    {
                        msgt = SENDFILE;
                        PUTOBJ(sock, msgt);
                        write(sock, fszbuf, sizeof fszbuf);
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
