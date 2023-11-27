#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int execute_command(const char *command, char *output, int output_size) {
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error al ejecutar el comando");
        return -1;
    }

    // Leer la salida del comando
    size_t bytesRead = fread(output, 1, output_size - 1, fp);
    output[bytesRead] = '\0'; // Null-terminate the output

    // Cerrar el descriptor de archivo del comando
    pclose(fp);

    return 0;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket a la dirección y al puerto
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al vincular el socket a la dirección y al puerto");
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Error al intentar escuchar por conexiones entrantes");
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexiones en el puerto %d...\n", PORT);

    while (1) {
        // Aceptar la conexión entrante
        socklen_t client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }

        printf("Cliente conectado desde %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Recibir y ejecutar comandos del cliente
        while (1) {
            memset(buffer, 0, BUFFER_SIZE); // Limpiar el buffer

            // Recibir el comando del cliente
            if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1) {
                perror("Error al recibir el comando");
                break;
            }

            // Salir si el cliente envía "salir" o "exit"
            if (strcmp(buffer, "salir") == 0 || strcmp(buffer, "exit") == 0) {
                break;
            }

            // Ejecutar el comando y obtener la salida
            char output[BUFFER_SIZE];
            if (execute_command(buffer, output, BUFFER_SIZE) == 0) {
                // Enviar la salida al cliente
                send(client_socket, output, strlen(output), 0);
            } else {
                // Enviar mensaje de error al cliente
                send(client_socket, "Error: Comando incorrecto o no soportado.", strlen("Error: Comando incorrecto o no soportado."), 0);
            }
        }

        printf("Cliente desconectado.\n");
        close(client_socket);
    }

    // Cerrar el socket del servidor (Esto nunca se alcanzará en este ejemplo)
    close(server_socket);

    return 0;
}

