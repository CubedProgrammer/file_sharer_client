#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<termios.h>
#include<unistd.h>
#include"msg.h"
#ifndef PORT
#define PORT 2203
#endif

int conn(const char *host, int port);

int main(int argl, char *argv[])
{
    puts("Welcome to file_sharer.");
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
    else if(sock)
    {
        puts("Connection successful.");
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
        addrin.sin_port = port;
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
