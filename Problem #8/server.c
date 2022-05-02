#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT 1234
#define SERVER INADDR_ANY
#define BUFLEN 101

void countCharInString(char str[], char chr, uint16_t* cont)
{
    uint16_t i;

    *cont = i = 0;

    for(; i[str] != '\0';) // while(*(str + i) != '\0')
        if(chr == str[i++])
            ++(*cont);

    return;
}

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(client);
    char str[BUFLEN], chr;
    uint16_t cont;
    bool mic;

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

    do{
        recvfrom(s, &mic, sizeof(mic), 0, (struct sockaddr*)&client, &slen);

        if(!mic)
            break;

        recvfrom(s, str, sizeof(str), 0, (struct sockaddr*)&client, &slen);
        recvfrom(s, &chr, sizeof(chr), 0, (struct sockaddr*)&client, &slen);

        printf("Stringul primit de la client   = \"%s\"\n", str);
        printf("Caracterul primit de la client = '%c'\n", chr);

        countCharInString(str, chr, &cont);
        cont = htons(cont);

        printf("Ii trimitem clientului numarul %hu\n", ntohs(cont));

        sendto(s, &cont, sizeof(cont), 0, (struct sockaddr*)&client, slen);

        fputc('\n', stdout);
    }while(true);

    close(s);

    return 0;
}