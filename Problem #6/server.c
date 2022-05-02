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
#define BUFLEN 7

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(struct sockaddr_in);
    char str_a[BUFLEN], str_b[BUFLEN], str_sum[BUFLEN];
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
        recvfrom(s, str_a, sizeof(str_a), 0, (struct sockaddr*)&client, &slen);
        recvfrom(s, str_b, sizeof(str_b), 0, (struct sockaddr*)&client, &slen);

        sscanf(str_a, "%hu", &a);
        sscanf(str_b, "%hu", &b);

        printf("a = %hu\n", a);
        printf("b = %hu\n", b);

        if(!a && !b)
        {
            close(s);

            return 0; // exit(0);
        }

        sum = a + b;
        sprintf(str_sum, "%hu", sum);
        printf("%hu + %hu = %hu\n\n", a, b, sum);

        sendto(s, str_sum, sizeof(str_sum), 0, (struct sockaddr*)&client, slen);
    }
}