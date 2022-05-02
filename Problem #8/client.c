#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PORT 1234
#define SERVER "127.0.0.1"
#define BUFLEN 101
#define STOP "stop"

bool cmpChars(char a, char b)
{
    return toupper(a) == toupper(b); // return tolower(a) == tolower(b);
}

bool cmpStrings(char* a, char* b)
{
    if(strlen(a) != strlen(b))
        return false;

    uint16_t i;

    for(i = 0; *(a + i) != '\0'; ++i)
        if(!cmpChars(i[a], b[i]))
            return false;

    return true;
}

int main(void)
{
    int c;
    struct sockaddr_in server;
    socklen_t slen = sizeof(server);
    char str[BUFLEN], chr;
    uint16_t cont;
    bool mic;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        return 1; // exit(1);
    }

    memset(&server, 0, slen);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER);

    for(; true;)
    {
        printf("Dati un sir de caractere de la tastatura: ");

        // char enter;
        // scanf("%c", &enter);

        scanf("\n");

        fgets(str, sizeof(str), stdin);
        str[strlen(str) - 1] = '\0';

        printf("Stringul citit este: \"%s\"\n", str);

        mic = (!strlen(str) || cmpStrings(str, STOP)) ? (false) : (true);

        sendto(c, &mic, sizeof(bool), 0, (struct sockaddr*)&server, slen);

        if(!mic)
            break;

        printf("Dati un caracter de la tastatura: ");
        chr = getchar();

        printf("Caracterul citit este: '%c'\n", chr);

        sendto(c, str, (strlen(str) + 1) * sizeof(char), 0, (struct sockaddr*)&server, slen);
        sendto(c, &chr, sizeof(chr), 0, (struct sockaddr*)&server, slen);

        recvfrom(c, &cont, sizeof(cont), 0, (struct sockaddr*)&server, &slen);

        cont = ntohs(cont);
        printf("Caracterul '%c' apare de %hu ori in stringul \"%s\"\n", chr, cont, str);

        putchar('\n');
    }

    close(c);

    return 0;
}