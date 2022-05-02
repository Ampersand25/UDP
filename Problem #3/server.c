#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define BUFLEN (unsigned)1e+2
#define VOWELS "aeiou"
#define PORT 1234

bool vowel(char c)
{
    return strchr(VOWELS, tolower(c)) != NULL;
}

void transform(char* msg)
{
    /*
    unsigned short i = 0;

    while(i[msg] != '\0')
        if(vowel(*(msg + i)))
        {
            strcpy(msg + i + 3, msg + i + 1);
            msg[i + 1] = 'p';
            msg[i + 2] = msg[i];
            i += 3;
        }
        else
            ++i;
    */

    char tr[BUFLEN];
    memset(&tr, 0, sizeof(tr));

    uint16_t i;

    for(i = 0; msg[i] != '\0'; ++i)
    {
        char temp[4];
        temp[0] = msg[i];

        if(!vowel(i[msg]))
            temp[1] = '\0';
        else{
            temp[1] = 'p';
            temp[2] = msg[i];
            temp[3] = '\0';
        }

        strcat(tr, temp);
    }

    strcpy(msg, tr);
}

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    uint16_t n;
    socklen_t slen = sizeof(client);
    char* msg;

    if((msg = (char*)calloc(BUFLEN, sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 1; // exit(1);
    }

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului server!\n");
        return 2; // exit(2);
    }

    memset(&server, 0, slen);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(s, (struct sockaddr*)&server, slen) < 0)
    {
        perror("Eroare la bind!\n");
        return 3; // exit(3);
    }

    memset(&client, 0, slen);

    while(true)
    {
        recvfrom(s, &n, sizeof(n), MSG_WAITALL, (struct sockaddr*)&client, &slen);
        n = ntohs(n);
        recvfrom(s, msg, (n + 1) * sizeof(char), MSG_WAITALL, (struct sockaddr*)&client, &slen);

        if(!strcmp(msg, "STOP"))
            goto et;

        printf("Lungime mesaj  = %hu\n", n);
        printf("Mesajul primit = %s\n", msg);

        transform(msg);
        n = htons(strlen(msg));
        sendto(s, &n, sizeof(n), 0, (struct sockaddr*)&client, slen);
        n = ntohs(n);
        printf("Ii trimitem clientului mesajul cu %hu caractere: %s\n", n, msg);
        sendto(s, msg, (n + 1) * sizeof(char), 0, (struct sockaddr*)&client, slen);

        putchar('\n'); // puts("");
    }

    et:

    free(msg);

    close(s);

    return 0;
}