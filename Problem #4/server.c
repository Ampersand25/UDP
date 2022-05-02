#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT htons(1234)
#define SERVER htonl(INADDR_ANY)
#define LIM (65535 + 1)

bool* prim;

void computeSieve()
{
    unsigned d;

    for(d = 3; d < LIM; d += 2)
        if(!prim[d])
        {
            unsigned k;

            for(k = 2; d * k < LIM; prim[d * k] = true, ++k);
        }
}

bool testPrim(uint16_t n)
{
    if(n == 2)
        return true;

    if(n <= 1 || !(n % 2))
        return false;

    /*
    uint16_t d;

    for(d = 3; d * d <= n; d += 2)
        if(!(n % d))
            return false;

    return true;
    */

    return !prim[n];
}

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(struct sockaddr_in);
    uint16_t n;
    char str[8];
    bool ePrim;

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "Eroare la crearea socketului server!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = SERVER;

    if(bind(s, (struct sockaddr*)&server, slen) < 0)
    {
        fputs("Eroare la bind!\n", stderr);
        return 2; // exit(2);
    }

    memset(&client, 0, slen);

    if((prim = (bool*)calloc(LIM, sizeof(bool))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 3; // exit(3);
    }

    // memset(prim, false, LIM * sizeof(bool));
    // memset(prim, 0, LIM * sizeof(bool));

    computeSieve();

    for(; ;)
    {
        recvfrom(s, &n, sizeof(n), 0, (struct sockaddr*)&client, &slen);

        n = ntohs(n);
        printf("Am primit de la client numarul %hu\n", n);

        if(!n)
            break;

        ePrim = testPrim(n);

        memset(str, 0, sizeof(str));

        switch(ePrim)
        {
            case true:
                strcpy(str, "este");
                break;
            default: // case false:
                strcpy(str, "nu este");
        }

        printf("Numarul %hu %s prim!\n", n, str);

        sendto(s, &ePrim, sizeof(ePrim), 0, (struct sockaddr*)&client, slen);

        puts("");
    }

    free(prim);

    close(s);

    return 0;
}