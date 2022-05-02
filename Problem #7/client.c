#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT htons(1234)
#define SERVER inet_addr("127.0.0.1")
#define BUFLEN (uint16_t)(1e+2 + 1)

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(struct sockaddr_in);
    char* msg, *rev;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = SERVER;

    if((msg = (char*)malloc(BUFLEN * sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 2; // exit(2);
    }

    if((rev = (char*)calloc(BUFLEN, sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 3; // exit(3);
    }

    while(420)
    {
        memset(msg, 0, BUFLEN * sizeof(char));
        memset(rev, 0, BUFLEN * sizeof(char));

        printf("Introduceti un string: ");
        fgets(msg, BUFLEN * sizeof(char), stdin);
        msg[strlen(msg) - 1] = '\0';

        printf("Stringul dat este: ");
        puts(msg);

        sendto(c, msg, (strlen(msg) + 1) * sizeof(char), 0, (struct sockaddr*)&server, slen);

        if(!strlen(msg)) // if(*msg == '\0')
            break;

        recvfrom(c, rev, BUFLEN * sizeof(char), 0, (struct sockaddr*)&server, &slen);

        fprintf(stdout, "Stringul \"%s\" oglindit este: \"%s\"\n", msg, rev);

        fputc('\n', stdout);
    }

    free(msg), free(rev);

    close(c);

    return 0;
}