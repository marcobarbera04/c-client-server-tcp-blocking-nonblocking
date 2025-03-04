#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int client_fd;
    struct sockaddr_in client_addr;
    size_t client_addr_lenght = sizeof(client_addr);

    char buffer[BUFFER_SIZE] = {0};

    // create socket
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Could not create the socket");
        exit(EXIT_FAILURE);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_fd, (struct sockaddr *)&client_addr, client_addr_lenght) < 0)
    {
        perror("Could not connect to the server");
        exit(EXIT_FAILURE);
    }

    int numbers[] = {0, 1, 1, 2, 3, 5, 8};      // fibonacci sequence n6
    for(size_t i = 0; i < 7; i++)
    {
        send(client_fd, &numbers[i], sizeof(numbers[i]), 0);

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        buffer[bytes_read] = '\n';
        printf("[SERVER]: %s\n", buffer);
    }

    // send end signal
    int end_signal = -1;
    send(client_fd, &end_signal, sizeof(end_signal), 0);

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
    if(bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        printf("[SERVER]%s\n", buffer);
    }
    else
    {
        perror("Could not read the result message");
    }

    close(client_fd);

    return 0;
}

