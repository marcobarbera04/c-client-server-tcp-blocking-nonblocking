#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "fibonacci.h"

#define PORT 8080
#define BUFFER_SIZE 128

int main()
{
    int server_fd = 0;
    int client_fd = 0;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    size_t server_address_lenght = sizeof(server_address);
    size_t client_address_lenght = sizeof(client_address);

    int buffer[BUFFER_SIZE] = {0};  
    char *server_message = "Number received";
    
    // create socket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Could not create the socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind
    if(bind(server_fd, (struct sockaddr *)&server_address, server_address_lenght) < 0)
    {
        perror("Could not bind the address to socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen
    if(listen(server_fd, 1) < 0)
    {
        perror("Could not start the listening");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port: %d\n", PORT);

    // accept
    if((client_fd = accept(server_fd, (struct sockaddr *)&server_address, (socklen_t *)&server_address_lenght)) < 0)
    {
        perror("Could not accept any client");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // reading all the numbers sent from client
    int number_received = 0;
    int numbers_counter = 0;
    while(true)
    {
        ssize_t bytes_read = recv(client_fd, &number_received, sizeof(number_received), 0);
        if(bytes_read > 0)
        {
            if(number_received == -1)
            {
                printf("End of sequence\n");
                break;
            }
            buffer[numbers_counter] = number_received;
            numbers_counter++;
            printf("[CLIENT]: %d\n", buffer[numbers_counter-1]);
            
            send(client_fd, server_message, strlen(server_message), 0);
            printf("Message sent to the client\n");
        }
        else
        {
            printf("Disconnection\n");
            break;
        }
    }

    // print the buffer of numbers received
    printf("BUFFER:\n");
    for(size_t i = 0; i < numbers_counter; i++){
        printf("%d\t", buffer[i]);
    }
    printf("\n");
    
    // verify if the sequence received match with fibonacci
    bool is_fibonacci = true;
    for(int i = 0; i < numbers_counter; i++)
    {
        if(buffer[i] != fibonacci(i))
        {
            is_fibonacci = false;
            break;
        }
    }

    const char *match = "The sequence received match with the fibonacci sequence";
    const char *no_match = "The sequence does not match with the fibonacci sequece";
    const char *result = (is_fibonacci) ? match : no_match;

    printf("%s\n", result);
    send(client_fd, result, strlen(result)+1, 0);

    close(client_fd);
    close(server_fd);

    return 0;
}