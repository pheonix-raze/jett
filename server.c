#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    int sd, cd;
    char buf[1000] = "";
    struct sockaddr_in ser;

    // Create a socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        printf("SOCKET NOT CREATED\n");
        return 1;
    }

    bzero(&ser, sizeof(struct sockaddr_in));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(1101);
    inet_aton("127.0.0.1", &ser.sin_addr);

    int b = bind(sd, (struct sockaddr *)&ser, sizeof(ser));
    if (b < 0) {
        printf("BIND FAILED\n");
        return 1;
    }
    printf("BIND VALUE: %d\n", b);

    if (listen(sd, 5) < 0) {
        printf("LISTEN FAILED\n");
        return 1;
    }

    while (1) {
        cd = accept(sd, NULL, NULL);
        if (cd < 0) {
            printf("ACCEPT FAILED\n");
            continue;
        }

        int pid = fork();
        if (pid < 0) {
            printf("FORK FAILED\n");
            close(cd);
            continue;
        }

        if (pid == 0) {  // Child process
            close(sd);  // Close unnecessary file descriptor
            printf("Accept value: %d\n", cd);
            ssize_t bytes_read = read(cd, buf, sizeof(buf) - 1);
            if (bytes_read > 0) {
                buf[bytes_read] = '\0';  // Null-terminate the string
                printf("MESSAGE FROM CLIENT: %s\n", buf);
                write(cd, "Message received", 16);
            }
            close(cd);
            exit(0);
        } else {  // Parent process
            close(cd);  // Close unnecessary file descriptor
            waitpid(-1, NULL, WNOHANG);  // Clean up zombie processes
        }
    }

    close(sd);
    return 0;
}
