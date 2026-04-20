#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define SOCKET_PATH "/tmp/zad15_unix_socket"
#define BUF_SIZE 256
int main(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return 1;
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(fd);
        return 1;
    }
    const char *message = "Imię i nazwisko";
    if (send(fd, message, strlen(message), 0) == -1) {
        perror("send");
        close(fd);
        return 1;
    }
    char response[BUF_SIZE];
    ssize_t received = recv(fd, response, sizeof(response) - 1, 0);
    if (received == -1) {
        perror("recv");
        close(fd);
        return 1;
    }
    response[received] = '\0';
    printf("Odpowiedz serwera: %s\n", response);
    fflush(stdout);
    close(fd);
    return 0;
}
