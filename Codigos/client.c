#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Crear el socket del cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    // Enviar comandos al servidor
    while (1) {
        printf("Ingrese un comando (o 'salir' o 'exit' para desconectar): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Eliminar el carácter de nueva línea
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Salir si el usuario ingresa "salir" o "exit"
        if (strcmp(buffer, "salir") == 0 || strcmp(buffer, "exit") == 0) {
            break;
        }

        // Enviar el comando al servidor
        send(client_socket, buffer, strlen(buffer), 0);

        // Recibir y mostrar la salida del servidor
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
            printf("%s", buffer);
        } else {
            perror("Error al recibir la salida del servidor");
            break;
        }
    }

    // Cerrar el socket del cliente
    close(client_socket);

    return 0;
}

