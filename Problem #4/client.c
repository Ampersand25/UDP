#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 1234
#define SERVER "127.0.0.1"
#define TERNARY(a, b, c) \
    ((a) ? (b) : (c))

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(server);
    uint16_t n;
    bool ePrim;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER);

    while(true)
    {
        fprintf(stdout, "Dati un numar de la tastatura: ");
        fscanf(stdin, "%hu", &n);

        printf("Numarul introdus este %hu\n", n);

        n = htons(n);
        sendto(c, &n, sizeof(n), 0, (struct sockaddr*)&server, slen);

        n = ntohs(n);
        if(!n)
            break;

        recvfrom(c, &ePrim, sizeof(ePrim), 0, (struct sockaddr*)&server, &slen);

        TERNARY(ePrim, printf("Numarul %hu este prim!%c", n, '\n'), printf("Numarul %hu nu este prim!%s", n, "\n"));

        putchar('\n');
    }

    close(c);

    return 0;
}