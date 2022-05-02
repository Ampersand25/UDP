#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/ip.h>

#define PORT htons(1234)
#define SERVER htonl(INADDR_ANY)
#define BUFLEN 101
#define SWAP(a, b, type) \
    {type aux = a; a = b; b = aux;}

typedef enum{false, true} bool;

void strrevUser(char* str)
{
    uint16_t len = strlen(str), i;

    for(i = 0; i < (len >> 1); ++i)
        SWAP(str[i], str[len - i - 1], char);
}

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(struct sockaddr_in);
    char* msg, *rev;

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fputs("Eroare la crearea socketului server!\n", stderr);
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = SERVER;

    if(bind(s, (struct sockaddr*)&server, slen) < 0)
    {
        fprintf(stderr, "Eroare la bind!\n");
        return 2; // exit(2);
    }

    memset(&client, 0, slen);

    if((msg = (char*)calloc(BUFLEN, sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 3; // exit(3);
    }

    if((rev = (char*)malloc(BUFLEN * sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 4; // exit(4);
    }

    while(true)
    {
        recvfrom(s, msg, BUFLEN * sizeof(char), MSG_WAITALL, (struct sockaddr*)&client, &slen);

        printf("Stringul primit de la client este: %s\n", msg);

        if(msg[0] == '\0') // if(!strcmp(msg, ""))
            break;

        strcpy(rev, msg);

        strrevUser(rev);

        printf("Stringul oglindit este: %s\n", rev);

        sendto(s, rev, (strlen(rev) + 1) * sizeof(char), 0, (struct sockaddr*)&client, slen);

        putchar('\n');
    }

    free(msg);
    free(rev);

    close(s);

    return 0;
}