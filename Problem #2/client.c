#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 1234
#define SERVER "127.0.0.1"
#define BUFLEN (uint16_t)1e+3

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(server);
    char chr, *str;
    uint16_t n, nr, i;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER);

    if((str = (char*)calloc(BUFLEN, sizeof(char))) == NULL)
    {
        perror("Heap Overflow!\n");
        return 2; // exit(2);
    }

    while(true)
    {
        memset(str, 0, BUFLEN * sizeof(char));

        printf("Introduceti un string de la tastatura: ");

        fgets(str, BUFLEN * sizeof(char), stdin);
        n = strlen(str);
        str[n - 1] = '\0';

        fprintf(stdout, "Stringul dat este: %s\n", str);

        sendto(c, str, (n + 1) * sizeof(char), 0, (struct sockaddr*)&server, slen);

        if(!strcmp("STOP", str))
            break;

        recvfrom(c, &chr, sizeof(chr), 0, (struct sockaddr*)&server, &slen);
        recvfrom(c, &nr, sizeof(nr), 0, (struct sockaddr*)&server, &slen);

        nr = ntohs(nr);

        fprintf(stdout, "Cel mai frecvent caracter din string este %c cu %hu aparitii\n", chr, nr);

        uint16_t pos[nr];
        memset(pos, 0, nr * sizeof(uint16_t));

        recvfrom(c, pos, sizeof(pos), 0, (struct sockaddr*)&server, &slen);

        fputs("Pozitiile pe care acesta apare in string sunt: ", stdout);

        for(i = 0; i < nr; printf("%hu,", ntohs(pos[i++])));

        printf("%s%c", "\b ", '\n');

        putchar('\n');
    }

    free(str);

    puts("\nTerminare activitate client...");

    close(c);

    return 0;
}