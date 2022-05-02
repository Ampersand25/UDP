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
#define BUFLEN 7

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(server);
    char str_a[BUFLEN], str_b[BUFLEN], str_sum[BUFLEN];
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
        // scanf("%s", str_a);
        fgets(str_a, sizeof(str_a), stdin);
        str_a[strlen(str_a) - 1] = '\0';

        printf("b = ");
        // scanf("%s", str_b);
        fgets(str_b, sizeof(str_b), stdin);
        str_b[strlen(str_b) - 1] = '\0';

        sendto(c, str_a, sizeof(str_a), 0, (struct sockaddr*)&server, slen);
        sendto(c, str_b, sizeof(str_b), 0, (struct sockaddr*)&server, slen);

        sscanf(str_a, "%hu", &a);
        sscanf(str_b, "%hu", &b);

        if(!(a + b)) // if(!a && !b)
            break;

        recvfrom(c, str_sum, sizeof(str_sum), 0, (struct sockaddr*)&server, &slen);

        sum = atoi(str_sum);

        printf("%hu + %hu = %hu\n\n", a, b, sum);
    }

    close(c);

    return 0;
}