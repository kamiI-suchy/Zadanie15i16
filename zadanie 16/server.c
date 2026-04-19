#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5001
#define BUF_SIZE 256

static void to_uppercase(char *text) {
    for (size_t i = 0; text[i] != '\0'; ++i) {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

int main(void) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    printf("Serwer UDP (zad16) nasluchuje na porcie %d\n", SERVER_PORT);
    fflush(stdout);

    while (1) {
        char buffer[BUF_SIZE];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        ssize_t received = recvfrom(
            fd,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (received == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("recvfrom");
            continue;
        }

        buffer[received] = '\0';
        to_uppercase(buffer);

        if (sendto(fd, buffer, (size_t)received, 0, (const struct sockaddr *)&client_addr, client_len) == -1) {
            perror("sendto");
            continue;
        }

        char ip_buf[INET_ADDRSTRLEN];
        const char *ip_text = inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, sizeof(ip_buf));
        if (ip_text == NULL) {
            ip_text = "(nieznany_adres)";
        }

        printf("Obsłużono klienta %s:%d, tekst: %s\n", ip_text, ntohs(client_addr.sin_port), buffer);
        fflush(stdout);
    }

    close(fd);
    return 0;
}
