#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT 1234
#define SERVER "127.0.0.1"

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(server);
    uint16_t a, b, sum;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        exit(1); // return 1;
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER);

    for(; ;)
    {
        printf("a = ");
        scanf("%hu", &a);
        a = htons(a);

        printf("b = ");
        scanf("%hu", &b);
        b = htons(b);

        sendto(c, &a, sizeof(a), 0, (struct sockaddr*)&server, slen);
        sendto(c, &b, sizeof(b), 0, (struct sockaddr*)&server, slen);

        a = ntohs(a);
        b = ntohs(b);

        if(!(a + b)) // if(!a && !b)
            break;

        recvfrom(c, &sum, sizeof(sum), 0, (struct sockaddr*)&server, &slen);

        sum = ntohs(sum);

        printf("%hu + %hu = %hu%s", a, b, sum, "\n\n");
    }

    close(c);

    return 0;
}