#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 1234
#define BUFLEN (uint16_t)1e+3

bool isAlpha(char chr)
{
    char lwr = tolower(chr);

    return lwr >= 'a' && lwr <= 'z';
}

void mostFreqChar(char* str, char* mfc, uint16_t* maxFreq)
{
    uint16_t* freq, i, currFreq;
    char chr;

    if((freq = (uint16_t*)calloc(26, sizeof(uint16_t))) == NULL)
    {
        perror("Heap Overflow!\n");
        exit(4);
    }

    *maxFreq = 1;
    *mfc = *str;

    for(i = 0; i[str] != '\0'; ++i)
    {
        if(!isAlpha(str[i]))
            continue;

        chr = tolower(*(str + i));
        currFreq = ++freq[chr - 'a'];

        if(currFreq > *maxFreq)
        {
            *maxFreq = currFreq;
            *mfc = chr;
        }
    }
}

int main(void)
{
    int s;
    struct sockaddr_in client, server;
    socklen_t slen = sizeof(struct sockaddr_in);
    char* str, mfc, *p, *curr;
    uint16_t maxFreq, idx;
    //uint16_t i;

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

    if((str = (char*)malloc(BUFLEN * sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 3; // exit(3);
    }

    memset(str, 0, BUFLEN * sizeof(char));

    while(true)
    {
        recvfrom(s, str, BUFLEN * sizeof(char), 0, (struct sockaddr*)&client, &slen);

        if(!strcmp(str, "STOP"))
            break;

        fprintf(stdout, "Am primit de la client stringul: %s\n", str);

        mostFreqChar(str, &mfc, &maxFreq);

        printf("Caracterul cel mai frecvent din string este %c cu %hu aparitii\n", mfc, maxFreq);

        uint16_t pos[maxFreq];
        memset(pos, 0, sizeof(pos));
        idx = 0;

        /*
        for(i = 0; i[str] != '\0'; ++i)
            if(str[i] == mfc)
            {
                pos[idx++] = htons(i);

                if(idx == maxFreq)
                    break;
            }
        */

        curr = str;

        do{
            p = strchr(curr, mfc);

            pos[idx++] = htons(p - str);

            if(p != NULL)
                curr = p + 1;
        }while(p != NULL);

        maxFreq = htons(maxFreq);

        sendto(s, &mfc, sizeof(mfc), 0, (struct sockaddr*)&client, slen);
        sendto(s, &maxFreq, sizeof(maxFreq), 0, (struct sockaddr*)&client, slen);
        sendto(s, pos, maxFreq * sizeof(uint16_t), 0, (struct sockaddr*)&client, slen);

        fputc('\n', stdout);
    }

    free(str);

    printf("Terminare activitate server...%c", '\n');

    close(s);

    return 0;
}