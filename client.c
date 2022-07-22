#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<termios.h>
#include<unistd.h>
#include"conrd.h"
#include"msg.h"
#include"sharing.h"
#ifndef PORT
#define PORT 2203
#endif

int conn(const char *host, int port);

int main(int argl, char *argv[])
{
    puts("Welcome to file_sharer.");
    const char *host = argv[1];
    int succ;
    char hostbuf[91];
    if(host == NULL)
    {
        nohost:
        fputs("Enter the internet address of the host: ", stdout);
        scanf("%s", hostbuf);
        host = hostbuf;
    }
    int sock = 0;
    if(*host != '\0')
        sock = conn(host, PORT);
    if(sock == -1)
    {
        puts("\033\13331mConnection failed.\033\133m");
        goto nohost;
    }
#ifdef _WIN32
    HANDLE ho = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cm;
    GetConsoleMode(ho, &cm);
    cm |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(ho, cm);
    if(!_isatty(_fileno(stdin)))
#else
    struct termios old, curr;
    tcgetattr(STDIN_FILENO, &old);
    curr = old;
    curr.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &curr);
    if(!isatty(STDIN_FILENO))
#endif
        fputs("Redirecting the input of this program is inadvisable.\n", stderr);
#ifdef _WIN32
    if(!_isatty(_fileno(stdout)))
#else
    if(!isatty(STDOUT_FILENO))
#endif
        fputs("Redirecting the output of this program is inadvisable.\n", stderr);
    if(sock)
    {
        puts("Connection successful.");
        puts("Press q to quit, c to create a room for upload, j to join to download.");
        puts("Press d to display the file when downloaded (default), s to save.");
        puts("Show file");
        fs_msg_t msgt;
        char msgnam[13];
        uint64_t rnum;
        uint32_t rnumpart;
        char display = 1;
        char cmd;
        for(cmd = gch; cmd != 'q'; cmd = gch)
        {
            switch(cmd)
            {
                case'c':
                    msgt = UPLOADER;
                    PUTOBJ(sock, msgt);
                    GETOBJ(sock, msgt);
                    if(msgt == ROOMNUM)
                    {
                        GETOBJ(sock, rnumpart);
                        rnum = htonl(rnumpart);
                        rnum <<= 32;
                        GETOBJ(sock, rnumpart);
                        rnum |= htonl(rnumpart);
                        printf("%lx is the room number.\n", rnum);
                        uploader(sock);
                    }
                    else
                    {
                        get_msg_name(msgt, msgnam);
                        printf("Server sent an invalid message, %s.\n", msgnam);
                    }
                    break;
                case'j':
                    printf("Press Ctrl+%c to hide or show.\n", HIDE + 'A' - 1);
                    fputs("Enter room number: ", stdout);
                    rnum = rdnum();
                    putchar('\n');
                    msgt = RECEIPIENT;
                    PUTOBJ(sock, msgt);
                    msgt = ROOMNUM;
                    PUTOBJ(sock, msgt);
                    rnumpart = rnum >> 32;
                    rnumpart = htonl(rnumpart);
                    PUTOBJ(sock, rnumpart);
                    rnumpart = htonl(rnum);
                    PUTOBJ(sock, rnumpart);
                    GETOBJ(sock, msgt);
                    if(msgt == JOINSUCC)
                        receipient(sock);
                    else if(msgt == JOINFAIL)
                        puts("Room doesn't exist.");
                    else
                        puts("Joining failed.");
                    break;
                case'd':
                    display = 1;
                    puts("\033\133FShow file");
                    break;
                case's':
                    display = 0;
                    puts("\033\133FSave file");
                    break;
                default:
                    ring;
            }
        }
        msgt = QUIT;
        PUTOBJ(sock, msgt);
        puts("Exiting");
    }
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
#endif
    return succ;
}

int conn(const char *host, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock > 0)
    {
        struct sockaddr_in addrin;
        addrin.sin_family = AF_INET;
        addrin.sin_port = htons(port);
        int succ = inet_aton(host, &addrin.sin_addr);
        if(succ)
        {
            succ = connect(sock, (struct sockaddr *)&addrin, sizeof addrin);
            if(succ)
            {
                close(sock);
                sock = -1;
                puts("Could not connect to server");
            }
        }
        else
        {
            puts("Could not interpret hostname");
            close(sock);
            sock = -1;
        }
    }
    return sock;
}
