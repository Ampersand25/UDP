#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFLEN (unsigned)1e+3
#define PORT 1234
#define SERVER "127.0.0.1"
#define SEP ";"

bool isDuplicate(char* all, char* str)
{
    char copy[strlen(all) + 1];
    strcpy(copy, all);

    char* p = strtok(copy, SEP);

    while(p != NULL)
    {
        if(!(strcmp(p, str)))
            return true;

        p = strtok(NULL, SEP); // p = strtok('\0', SEP);
    }

    return false;
}

int main(void)
{
    int c, n;
    struct sockaddr_in server;
    char all[BUFLEN], buf[BUFLEN];
    socklen_t slen = sizeof(server);
    bool mic;

    if((c = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Eroare la crearea socketului client!\n");
        return 1;
    }

    memset(&server, 0, slen);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER);

    memset(all, 0, sizeof(all));

    while(true)
    {
        memset(buf, 0, sizeof(buf));

        printf("Dati un string: ");
        fgets(buf, sizeof(buf), stdin);
        n = strlen(buf);
        buf[n - 1] = '\0';

        printf("Stringul introdus este: %s\n", buf);

        if(!strlen(buf) || isDuplicate(all, buf) || strchr(buf, SEP[0]) != NULL)
        {
            mic = false;
            sendto(c, &mic, sizeof(bool), 0, (struct sockaddr*)&server, slen);

            break;
        }

        mic = true;
        sendto(c, &mic, sizeof(bool), 0, (struct sockaddr*)&server, slen);

        sendto(c, buf, (n + 1) * sizeof(char), 0, (struct sockaddr*)&server, slen);

        strcat(all, buf);
        strcat(all, SEP);

        recvfrom(c, buf, sizeof(buf), 0, (struct sockaddr*)&server, &slen);

        printf("Stringul primit de la server este: %s\n", buf);

        putchar('\n');
    }

    all[strlen(all) - 1] = '\0';
    fprintf(stdout, "\nToate cuvintele introduse de client sunt: \"%s\"\n", all);
    fputs("Terminare activitate client...\n", stdout);

    close(c);

    return 0;
}