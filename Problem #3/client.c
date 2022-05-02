#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUFLEN (unsigned)1e+2
#define PORT 1234
#define SERVER "127.0.0.1"

int main(void)
{
    int c;
    struct sockaddr_in server;
    uint16_t n;
    char* msg;
    socklen_t slen = sizeof(server);

    if((msg = (char*)malloc(BUFLEN * sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 1; // exit(1);
    }

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "Eroare la crearea socketului client!\n");
        return 2; // exit(2);
    }

    memset(&server, 0, slen);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER);

    while(true)
    {
        printf("Introduceti mesajul dorit: ");
        fgets(msg, BUFLEN, stdin);
        n = strlen(msg);
        msg[--n] = '\0';

        n = htons(n);
        sendto(c, &n, sizeof(n), 0, (struct sockaddr*)&server, slen);
        sendto(c, msg, (n + 1) * sizeof(char), 0, (struct sockaddr*)&server, slen);

        if(!strcmp("STOP", msg))
            break;

        n = ntohs(n);
        fprintf(stdout, "Mesajul introdus de %hu caractere este: %s\n", n, msg);

        recvfrom(c, &n, sizeof(n), 0, (struct sockaddr*)&server, &slen);
        n = ntohs(n);
        recvfrom(c, msg, (n + 1) * sizeof(char), 0, (struct sockaddr*)&server, &slen);
        printf("Mesajul primit cu %hu caractere de la server este: %s\n\n", n, msg);
    }

    free(msg);

    close(c);

    return 0;
}