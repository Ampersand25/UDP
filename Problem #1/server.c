#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFLEN (unsigned)1e+3
#define PORT 1234
#define VOWS "aeiou"

bool isVowel(char c)
{
    return strchr(VOWS, tolower(c)) != NULL;
}

bool isConson(char c)
{
    return isalpha(c) && strchr(VOWS, tolower(c)) == NULL;
}

int countString(char* buf, bool (*condition)(char))
{
    uint16_t i, cont = 0;

    for(i = 0; i[buf] != '\0'; ++i)
        if(condition(*(buf + i)))
            ++cont;

    return cont;
}

/*
bool containsAlpha(char* buf)
{
    uint16_t i;

    for(i = 0; buf[i] != '\0'; ++i)
        if(isalpha(i[buf]))
            return true;

    return false;
}
*/

int main(void)
{
    int s, n, cont = 0;
    struct sockaddr_in server, client;
    socklen_t slen = sizeof(struct sockaddr_in);
    char buf[BUFLEN], encoded[BUFLEN];
    bool mic;

    srand(time(NULL)); // srand(time(0));

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "Eroare la crearea socketului server!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(s, (struct sockaddr*)&server, slen) < 0)
    {
        fputs("Eroare la bind!\n", stderr);
        return 2; // exit(2);
    }

    memset(&client, 0, slen);

    while(true)
    {
        recvfrom(s, &mic, sizeof(mic), 0, (struct sockaddr*)&client, &slen);

        if(!mic)
            goto end;

        ++cont;

        memset(buf, 0, sizeof(buf));

        recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&client, &slen);

        printf("Stringul primit de la client este: %s\n", buf);

        memset(encoded, 0, sizeof(encoded));

        sprintf(encoded, "%d", cont);

        n = strlen(buf);

        char tmp[4];

        tmp[0] = buf[rand() % n];
        tmp[1] = buf[rand() % n];
        tmp[2] = buf[rand() % n];
        tmp[3] = '\0';

        strcat(encoded, tmp);

        sprintf(tmp, "%d%d", countString(buf, isVowel), countString(buf, isConson));
        strcat(encoded, tmp);

        n = strlen(encoded);
        printf("Trimitem la client stringul: %s\n", encoded);
        sendto(s, encoded, (n + 1) * sizeof(char), 0, (struct sockaddr*)&client, slen);

        putchar('\n');
    }

    end:

    puts("Terminare activitate server...");

    close(s);

    return 0;
}