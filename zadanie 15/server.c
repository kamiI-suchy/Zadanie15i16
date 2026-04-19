#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/zad15_unix_socket"
#define BUF_SIZE 256

static void to_uppercase(char *text) {
    for (size_t i = 0; text[i] != '\0'; ++i) {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

int main(void) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return -1;
    }

    printf("Serwer UNIX (zad15) nasluchuje na %s\n", SOCKET_PATH);
    fflush(stdout);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        char buffer[BUF_SIZE];
        ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (received > 0) {
            buffer[received] = '\0';
            to_uppercase(buffer);
            if (send(client_fd, buffer, strlen(buffer), 0) == -1) {             
                perror("send");
            }
        } else if (received == -1) {
            perror("recv");
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
