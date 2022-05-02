#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT 1234
#define SERVER INADDR_ANY

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(struct sockaddr_in);
    uint16_t a, b, sum;

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "Eroare la crearea socketului server!\n");
        exit(1); // return 1;
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(SERVER);

    if(bind(s, (struct sockaddr*)&server, slen) < 0)
    {
        fputs("Eroare la bind!\n", stderr);
        exit(2); // return 2;
    }

    memset(&client, 0, slen);

    while(1)
    {
        recvfrom(s, &a, sizeof(a), MSG_WAITALL, (struct sockaddr*)&client, &slen);
        recvfrom(s, &b, sizeof(b), MSG_WAITALL, (struct sockaddr*)&client, &slen);

        a = ntohs(a), b = ntohs(b);

        printf("a = %hu\n", a);
        printf("b = %hu\n", b);

        if(!a && !b)
            goto final_et;

        sum = htons(a + b);
        printf("%hu + %hu = %hu%c%c", a, b, ntohs(sum), '\n', '\n');

        sendto(s, &sum, sizeof(sum), 0, (struct sockaddr*)&client, slen);
    }

    final_et:

    close(s);

    return 0;
}