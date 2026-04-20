#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" //do zmiany na ip płytki (serwera), sprawdzane ip a bądź ip addr
#define SERVER_PORT 5001
#define BUF_SIZE 256

int main(void) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Niepoprawny adres IP serwera: %s\n", SERVER_IP);
        close(fd);
        return 1;
    }

    const char *message = "Imię i nazwisko";
    if (sendto(fd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        close(fd);
        return 1;
    }

    char response[BUF_SIZE];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    ssize_t received = recvfrom(fd, response, sizeof(response) - 1, 0, (struct sockaddr *)&from_addr, &from_len);
    if (received == -1) {
        perror("recvfrom");
        close(fd);
        return 1;
    }

    response[received] = '\0';
    printf("Odpowiedz serwera UDP: %s\n", response);
    fflush(stdout);

    close(fd);
    return 0;
}
